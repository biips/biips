//                                               -*- C++ -*-
/*
 * BiiPS software is a set of libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
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

/*! \file ConjugateMNormalVar.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "samplers/ConjugateMNormalVar.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "common/cholesky.hpp"

namespace Biips
{

  const String ConjugateMNormalVar::NAME_ =
      "Conjugate Multivariate Normal (with known covariance matrix)";

  void ConjugateMNormalVar::formLikeParamContrib(NodeId likeId,
                                                 NumArray::Array & likeParamContribValues)
  {
    VectorRef like_mean(likeParamContribValues[0]);
    MatrixRef like_prec(likeParamContribValues[1]);

    GraphTypes::ParentIterator it_parents = graph_.GetParents(likeId).first;

    NodeId var_id = *(++it_parents);

    Matrix prec_i_mat(getNodeValue(var_id, graph_, *this));

    if (!ublas::cholesky_factorize(prec_i_mat))
      throw LogicError("ConjugateMNormalVar::formLikeParamContrib: matrix prec_i_mat is not positive-semidefinite.");
    ublas::cholesky_invert(prec_i_mat);

    NumArray obs_i(graph_.GetNode(likeId).DimPtr().get(),
                   graph_.GetValues()[likeId].get());
    VectorRef obs_i_vec(obs_i);

    like_mean += ublas::prod(prec_i_mat, obs_i_vec);
    like_prec += prec_i_mat;
  }

  NumArray::Array ConjugateMNormalVar::postParam(const NumArray::Array & priorParamValues,
                                                 const NumArray::Array & likeParamContribValues) const
  {
    static Matrix post_cov;
    post_cov = Matrix(priorParamValues[1]);

    if (!ublas::cholesky_factorize(post_cov))
      throw LogicError("ConjugateMNormalVar::postParam: matrix post_cov is not positive-semidefinite.");
    ublas::cholesky_invert(post_cov);

    static Vector post_mean;
    post_mean = ublas::prod(post_cov, Vector(priorParamValues[0]))
        + Vector(likeParamContribValues[0]);

    post_cov += Matrix(likeParamContribValues[1]);
    if (!ublas::cholesky_factorize(post_cov))
      throw LogicError("ConjugateMNormalVar::postParam: matrix post_cov is not positive-semidefinite.");
    ublas::cholesky_invert(post_cov);

    post_mean = ublas::prod(post_cov, post_mean);

    static NumArray::Array post_param_values(2);
    static DimArray dim_mean(1);
    dim_mean[0] = post_mean.size();
    post_param_values[0] = NumArray(&dim_mean, &post_mean.data());
    static DimArray dim_prec(2);
    dim_prec[0] = post_cov.size1();
    dim_prec[1] = post_cov.size2();
    post_param_values[1] = NumArray(&dim_prec, &post_cov.data());
    return post_param_values;
  }

  Scalar ConjugateMNormalVar::computeLogIncrementalWeight(const NumArray & sampledData,
                                                          const NumArray::Array & priorParamValues,
                                                          const NumArray::Array & postParamValues,
                                                          const NumArray::Array & LikeParamContrib)
  {
    static Matrix norm_const_cov;
    norm_const_cov = Matrix(LikeParamContrib[1]);
    if (!ublas::cholesky_factorize(norm_const_cov))
      throw LogicError("ConjugateMNormalVar::computeLogIncrementalWeight: matrix norm_const_cov is not positive-semidefinite.");
    ublas::cholesky_invert(norm_const_cov);

    static Vector norm_const_mean;
    norm_const_mean = ublas::prod(norm_const_cov, Vector(LikeParamContrib[0]));
    norm_const_cov += Matrix(priorParamValues[1]);

    static NumArray::Array norm_const_param_values(2);
    static DimArray dim_mean(1);
    dim_mean[0] = norm_const_mean.size();
    norm_const_param_values[0] = NumArray(&dim_mean, &norm_const_mean.data());
    static DimArray dim_cov(2);
    dim_cov[0] = norm_const_cov.size1();
    dim_cov[1] = norm_const_cov.size2();
    norm_const_param_values[1] = NumArray(&dim_cov, &norm_const_cov.data());

    Scalar log_incr_weight =
        DMNormVar::Instance()->LogDensity(priorParamValues[0],
                                          norm_const_param_values,
                                          NULL_NUMARRAYPAIR); // FIXME Boundaries
    if (isNan(log_incr_weight))
      throw RuntimeError("Failure to calculate log incremental weight.");

    return log_incr_weight;
  }

}

