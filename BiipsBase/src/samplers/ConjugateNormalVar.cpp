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


  DataType::Array ConjugateNormalVar::initLikeParamContrib() const
  {
    DataType::Array paramContribValues(PriorDistType::Instance()->NParam());
    for (Size i = 0; i < paramContribValues.size(); ++i)
      paramContribValues[i] = DataType(0.0);
    return paramContribValues;
  }

  void ConjugateNormalVar::formLikeParamContrib(NodeId likeId,
      DataType::Array & likeParamContribValues)
  {
    GraphTypes::DirectParentNodeIdIterator it_parents = pGraph_->GetParents(likeId).first;

    NodeId var_id = *(++it_parents);
    Scalar like_var = getNodeValue(var_id, pGraph_, this).ScalarView();
    likeParamContribValues[0].ScalarView() += pGraph_->GetValues()[likeId]->ScalarView() / like_var;
    likeParamContribValues[1].ScalarView() += 1.0 / like_var;
  }


  DataType::Array ConjugateNormalVar::postParam(const DataType::Array & priorParamValues,
      const DataType::Array & likeParamContribValues) const
  {
    Scalar post_var = 1.0 / (1.0 / priorParamValues[1].ScalarView() + likeParamContribValues[1].ScalarView());
    Scalar post_mean = post_var * ( priorParamValues[0].ScalarView() / priorParamValues[1].ScalarView() + likeParamContribValues[0].ScalarView() );

    DataType::Array post_param_values(2);
    post_param_values[0] = DataType(post_mean);
    post_param_values[1] = DataType(post_var);
    return post_param_values;
  }


  Scalar ConjugateNormalVar::computeLogWeight(const DataType & sampledData,
      const DataType::Array & priorParamValues,
      const DataType::Array & postParamValues,
      const DataType::Array & LikeParamContrib)
  {
    Scalar like_mean_contrib = LikeParamContrib[0].ScalarView();
    Scalar like_var_inv_contrib = LikeParamContrib[1].ScalarView();
    Scalar prior_var = priorParamValues[1].ScalarView();
    DataType::Array norm_const_param_values(2);
    norm_const_param_values[0] = DataType(like_mean_contrib / like_var_inv_contrib);
    norm_const_param_values[1] = DataType(prior_var + 1.0 / like_var_inv_contrib);
    return DNormVar::Instance()->LogUnnormPdf(priorParamValues[0], norm_const_param_values);
  }


}
