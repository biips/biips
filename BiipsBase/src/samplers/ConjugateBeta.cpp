//                                               -*- C++ -*-
/*! \file ConjugateBeta.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "samplers/ConjugateBeta.hpp"
#include "sampler/GetNodeValueVisitor.hpp"

namespace Biips
{

  const String ConjugateBeta::NAME_ = "ConjugateBeta";


  void ConjugateBeta::formLikeParamContrib(NodeId likeId,
      MultiArray::Array & likeParamContribValues)
  {
    GraphTypes::DirectParentNodeIdIterator it_parents = pGraph_->GetParents(likeId).first;

    NodeId trials_id = *(it_parents);

    likeParamContribValues[0].ScalarView() += getNodeValue(trials_id, pGraph_, this).ScalarView();
    likeParamContribValues[1].ScalarView() += pGraph_->GetValues()[likeId]->ScalarView();
  }


  MultiArray::Array ConjugateBeta::postParam(const MultiArray::Array & priorParamValues,
      const MultiArray::Array & likeParamContribValues) const
  {
    Scalar post_alpha = priorParamValues[0].ScalarView() + likeParamContribValues[1].ScalarView();
    Scalar post_beta = priorParamValues[1].ScalarView() + likeParamContribValues[0].ScalarView() - likeParamContribValues[1].ScalarView();

    MultiArray::Array post_param_values(2);
    post_param_values[0] = MultiArray(post_alpha);
    post_param_values[1] = MultiArray(post_beta);
    return post_param_values;
  }


  Scalar ConjugateBeta::computeLogWeight(const MultiArray & sampledData,
      const MultiArray::Array & priorParamValues,
      const MultiArray::Array & postParamValues,
      const MultiArray::Array & LikeParamContrib)
  {
    Scalar logWeight = DBeta::Instance()->LogUnnormPdf(sampledData, priorParamValues);
    logWeight -= DBeta::Instance()->LogUnnormPdf(sampledData, postParamValues);

    MultiArray::Array like_param_values(2);
    like_param_values[0] = MultiArray(LikeParamContrib[0].ScalarView());
    like_param_values[1] = sampledData;
    logWeight += DBin::Instance()->LogUnnormPdf(MultiArray(LikeParamContrib[1].ScalarView()), like_param_values);

    return logWeight;
  }

}
