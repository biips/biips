//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file ConjugateMNormal.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "samplers/ConjugateMNormal.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "common/cholesky.hpp"

namespace Biips
{

  const String ConjugateMNormal::NAME_ =
      "Conjugate Multivariate Normal (with known precision matrix)";

  void ConjugateMNormal::formLikeParamContrib(NodeId likeId,
                                              NumArray::Array & likeParamContribValues)
  {
    VectorRef like_mean(likeParamContribValues[0]);
    MatrixRef like_prec(likeParamContribValues[1]);

    GraphTypes::ParentIterator it_parents = graph_.GetParents(likeId).first;

    NodeId prec_id = *(++it_parents);

    Matrix prec_i_mat(getNodeValue(prec_id, graph_, *this));

    NumArray obs_i(graph_.GetNode(likeId).DimPtr().get(),
                   graph_.GetValues()[likeId].get());
    VectorRef obs_i_vec(obs_i);

    like_mean += ublas::prod(prec_i_mat, obs_i_vec);
    like_prec += prec_i_mat;
  }

  NumArray::Array ConjugateMNormal::postParam(const NumArray::Array & priorParamValues,
                                              const NumArray::Array & likeParamContribValues) const
  {
    static Matrix post_prec;
    post_prec = Matrix(priorParamValues[1]);

    static Vector post_mean;
    post_mean = ublas::prod(post_prec, Vector(priorParamValues[0]))
        + Vector(likeParamContribValues[0]);

    post_prec += Matrix(likeParamContribValues[1]);

    Matrix post_cov = post_prec;
    if (!ublas::cholesky_factorize(post_cov))
      throw LogicError("ConjugateMNormal::postParam: matrix post_cov is not positive-semidefinite.");
    ublas::cholesky_invert(post_cov);

    post_mean = ublas::prod(post_cov, post_mean);

    static NumArray::Array post_param_values(2);
    static DimArray dim_mean(1);
    dim_mean[0] = post_mean.size();
    post_param_values[0] = NumArray(&dim_mean, &post_mean.data());
    static DimArray dim_prec(2);
    dim_prec[0] = post_prec.size1();
    dim_prec[1] = post_prec.size2();
    post_param_values[1] = NumArray(&dim_prec, &post_prec.data());
    return post_param_values;
  }

  Scalar ConjugateMNormal::computeLogIncrementalWeight(const NumArray & sampledData,
                                                       const NumArray::Array & priorParamValues,
                                                       const NumArray::Array & postParamValues,
                                                       const NumArray::Array & LikeParamContrib)
  {
    static Matrix norm_const_prec;
    norm_const_prec = Matrix(LikeParamContrib[1]);
    if (!ublas::cholesky_factorize(norm_const_prec))
      throw LogicError("ConjugateMNormal::computeLogIncrementalWeight: matrix norm_const_prec is not positive-semidefinite.");
    ublas::cholesky_invert(norm_const_prec);

    static Vector norm_const_mean;
    norm_const_mean = ublas::prod(norm_const_prec, Vector(LikeParamContrib[0]));

    Matrix prior_prec(priorParamValues[1]);
    if (!ublas::cholesky_factorize(prior_prec))
      throw LogicError("ConjugateMNormal::computeLogIncrementalWeight: matrix prior_prec is not positive-semidefinite.");
    ublas::cholesky_invert(prior_prec);

    norm_const_prec += prior_prec;
    if (!ublas::cholesky_factorize(norm_const_prec))
      throw LogicError("ConjugateMNormal::computeLogIncrementalWeight: matrix norm_const_prec is not positive-semidefinite.");
    ublas::cholesky_invert(norm_const_prec);

    static NumArray::Array norm_const_param_values(2);
    static DimArray dim_mean(1);
    dim_mean[0] = norm_const_mean.size();
    norm_const_param_values[0] = NumArray(&dim_mean, &norm_const_mean.data());
    static DimArray dim_prec(2);
    dim_prec[0] = norm_const_prec.size1();
    dim_prec[1] = norm_const_prec.size2();
    norm_const_param_values[1] = NumArray(&dim_prec, &norm_const_prec.data());

    Scalar log_incr_weight =
        DMNorm::Instance()->LogDensity(priorParamValues[0],
                                       norm_const_param_values,
                                       NULL_NUMARRAYPAIR); // FIXME Boundaries
    if (isNan(log_incr_weight))
      throw RuntimeError("Failure to calculate log incremental weight.");

    return log_incr_weight;
  }

}
