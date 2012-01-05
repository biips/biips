//                                               -*- C++ -*-
/*! \file ConjugateNormal.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "samplers/ConjugateNormal.hpp"
#include "sampler/GetNodeValueVisitor.hpp"

namespace Biips
{

  const String ConjugateNormal::NAME_ =
      "Conjugate Normal (with known precision)";

  NumArray::Array ConjugateNormal::initLikeParamContrib() const
  {
    static NumArray::Array paramContribValues(2);
    static ValArray mean_val(1);
    mean_val[0] = 0.0;
    static ValArray prec_val(1);
    prec_val[0] = 0.0;
    paramContribValues[0].SetPtr(P_SCALAR_DIM.get(), &mean_val);
    paramContribValues[1].SetPtr(P_SCALAR_DIM.get(), &prec_val);

    return paramContribValues;
  }

  void ConjugateNormal::formLikeParamContrib(NodeId likeId,
                                             NumArray::Array & likeParamContribValues)
  {
    GraphTypes::ParentIterator it_parents = graph_.GetParents(likeId).first;

    NodeId prec_id = *(++it_parents);
    Scalar like_prec = getNodeValue(prec_id, graph_, *this).ScalarView();
    likeParamContribValues[0].ScalarView()
        += graph_.GetValues()[likeId]->ScalarView() * like_prec;
    likeParamContribValues[1].ScalarView() += like_prec;
  }

  NumArray::Array ConjugateNormal::postParam(const NumArray::Array & priorParamValues,
                                             const NumArray::Array & likeParamContribValues) const
  {
    static ValArray post_prec_val(1);
    post_prec_val[0] = priorParamValues[1].ScalarView()
        + likeParamContribValues[1].ScalarView();
    static ValArray post_mean_val(1);
    post_mean_val[0] = (priorParamValues[0].ScalarView()
        * priorParamValues[1].ScalarView()
        + likeParamContribValues[0].ScalarView()) / post_prec_val[0];

    static NumArray::Array post_param_values(2);
    post_param_values[0].SetPtr(P_SCALAR_DIM.get(), &post_mean_val);
    post_param_values[1].SetPtr(P_SCALAR_DIM.get(), &post_prec_val);
    return post_param_values;
  }

  Scalar ConjugateNormal::computeLogIncrementalWeight(const NumArray & sampledData,
                                                      const NumArray::Array & priorParamValues,
                                                      const NumArray::Array & postParamValues,
                                                      const NumArray::Array & LikeParamContrib)
  {
    Scalar like_mean_contrib = LikeParamContrib[0].ScalarView();
    Scalar like_prec_contrib = LikeParamContrib[1].ScalarView();
    Scalar prior_prec = priorParamValues[1].ScalarView();
    static NumArray::Array norm_const_param_values(2);
    static ValArray norm_const_mean_val(1);
    norm_const_mean_val[0] = like_mean_contrib / like_prec_contrib;
    norm_const_param_values[0].SetPtr(P_SCALAR_DIM.get(), &norm_const_mean_val);
    static ValArray norm_const_prec_val(1);
    norm_const_prec_val[0] = 1.0 / (1.0 / prior_prec + 1.0 / like_prec_contrib);
    norm_const_param_values[1].SetPtr(P_SCALAR_DIM.get(), &norm_const_prec_val);

    Scalar log_incr_weight =
        DNorm::Instance()->LogDensity(priorParamValues[0],
                                      norm_const_param_values,
                                      NULL_NUMARRAYPAIR); // FIXME Boundaries
    if (isNan(log_incr_weight))
      throw RuntimeError("Failure to calculate log incremental weight.");

    return log_incr_weight;
  }

}
