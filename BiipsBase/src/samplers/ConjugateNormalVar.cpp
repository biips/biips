//                                               -*- C++ -*-
/*! \file ConjugateNormalVar.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "samplers/ConjugateNormalVar.hpp"
#include "sampler/GetNodeValueVisitor.hpp"

namespace Biips
{

  const String ConjugateNormalVar::NAME_ = "Conjugate Normal (with known variance)";


  MultiArray::Array ConjugateNormalVar::initLikeParamContrib() const
  {
    MultiArray::Array paramContribValues(PriorDistType::Instance()->NParam());
    for (Size i = 0; i < paramContribValues.size(); ++i)
      paramContribValues[i] = MultiArray(0.0);
    return paramContribValues;
  }

  void ConjugateNormalVar::formLikeParamContrib(NodeId likeId,
      MultiArray::Array & likeParamContribValues)
  {
    GraphTypes::DirectParentNodeIdIterator it_parents = pGraph_->GetParents(likeId).first;

    NodeId var_id = *(++it_parents);
    Scalar like_var = getNodeValue(var_id, pGraph_, this).ScalarView();
    likeParamContribValues[0].ScalarView() += pGraph_->GetValues()[likeId]->ScalarView() / like_var;
    likeParamContribValues[1].ScalarView() += 1.0 / like_var;
  }


  MultiArray::Array ConjugateNormalVar::postParam(const MultiArray::Array & priorParamValues,
      const MultiArray::Array & likeParamContribValues) const
  {
    Scalar post_var = 1.0 / (1.0 / priorParamValues[1].ScalarView() + likeParamContribValues[1].ScalarView());
    Scalar post_mean = post_var * ( priorParamValues[0].ScalarView() / priorParamValues[1].ScalarView() + likeParamContribValues[0].ScalarView() );

    MultiArray::Array post_param_values(2);
    post_param_values[0] = MultiArray(post_mean);
    post_param_values[1] = MultiArray(post_var);
    return post_param_values;
  }


  Scalar ConjugateNormalVar::computeLogWeight(const MultiArray & sampledData,
      const MultiArray::Array & priorParamValues,
      const MultiArray::Array & postParamValues,
      const MultiArray::Array & LikeParamContrib)
  {
    Scalar like_mean_contrib = LikeParamContrib[0].ScalarView();
    Scalar like_var_inv_contrib = LikeParamContrib[1].ScalarView();
    Scalar prior_var = priorParamValues[1].ScalarView();
    MultiArray::Array norm_const_param_values(2);
    norm_const_param_values[0] = MultiArray(like_mean_contrib / like_var_inv_contrib);
    norm_const_param_values[1] = MultiArray(prior_var + 1.0 / like_var_inv_contrib);
    return DNormVar::Instance()->LogPdf(priorParamValues[0], norm_const_param_values);
  }


}
