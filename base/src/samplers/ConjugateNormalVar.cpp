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

/*! \file ConjugateNormalVar.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "samplers/ConjugateNormalVar.hpp"
#include "sampler/GetNodeValueVisitor.hpp"

namespace Biips
{

  const String ConjugateNormalVar::NAME_ =
      "Conjugate Normal (with known variance)";

  NumArray::Array ConjugateNormalVar::initLikeParamContrib() const
  {
    static NumArray::Array paramContribValues(2);
    static ValArray mean_val(1);
    mean_val[0] = 0.0;
    static ValArray var_val(1);
    var_val[0] = 0.0;
    paramContribValues[0].SetPtr(P_SCALAR_DIM.get(), &mean_val);
    paramContribValues[1].SetPtr(P_SCALAR_DIM.get(), &var_val);

    return paramContribValues;
  }

  void ConjugateNormalVar::formLikeParamContrib(NodeId likeId,
                                                NumArray::Array & likeParamContribValues)
  {
    GraphTypes::ParentIterator it_parents = graph_.GetParents(likeId).first;

    NodeId var_id = *(++it_parents);
    Scalar like_var = getNodeValue(var_id, graph_, *this).ScalarView();
    likeParamContribValues[0].ScalarView()
        += graph_.GetValues()[likeId]->ScalarView() / like_var;
    likeParamContribValues[1].ScalarView() += 1.0 / like_var;
  }

  NumArray::Array ConjugateNormalVar::postParam(const NumArray::Array & priorParamValues,
                                                const NumArray::Array & likeParamContribValues) const
  {
    static ValArray post_var(1);
    post_var[0] = 1.0 / (1.0 / priorParamValues[1].ScalarView()
        + likeParamContribValues[1].ScalarView());
    static ValArray post_mean(1);
    post_mean[0] = post_var[0] * (priorParamValues[0].ScalarView()
        / priorParamValues[1].ScalarView()
        + likeParamContribValues[0].ScalarView());

    static NumArray::Array post_param_values(2);
    post_param_values[0].SetPtr(P_SCALAR_DIM.get(), &post_mean);
    post_param_values[1].SetPtr(P_SCALAR_DIM.get(), &post_var);
    return post_param_values;
  }

  Scalar ConjugateNormalVar::computeLogIncrementalWeight(const NumArray & sampledData,
                                                         const NumArray::Array & priorParamValues,
                                                         const NumArray::Array & postParamValues,
                                                         const NumArray::Array & LikeParamContrib)
  {
    Scalar like_mean_contrib = LikeParamContrib[0].ScalarView();
    Scalar like_var_inv_contrib = LikeParamContrib[1].ScalarView();
    Scalar prior_var = priorParamValues[1].ScalarView();
    static NumArray::Array norm_const_param_values(2);
    static ValArray norm_const_mean_val(1);
    norm_const_mean_val[0] = like_mean_contrib / like_var_inv_contrib;
    norm_const_param_values[0].SetPtr(P_SCALAR_DIM.get(), &norm_const_mean_val);
    static ValArray norm_const_var_val(1);
    norm_const_var_val[0] = prior_var + 1.0 / like_var_inv_contrib;
    norm_const_param_values[1].SetPtr(P_SCALAR_DIM.get(), &norm_const_var_val);

    Scalar log_incr_weight =
        DNormVar::Instance()->LogDensity(priorParamValues[0],
                                         norm_const_param_values,
                                         NULL_NUMARRAYPAIR); // FIXME Boundaries
    if (isNan(log_incr_weight))
      throw RuntimeError("Failure to calculate log incremental weight.");

    return log_incr_weight;
  }

}
