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

  const String ConjugateNormal::NAME_ = "Conjugate Normal (with known precision)";


  DataType::Array ConjugateNormal::initLikeParamContrib() const
  {
    DataType::Array paramContribValues(PriorDistType::Instance()->NParam());
    for (Size i = 0; i < paramContribValues.size(); ++i)
      paramContribValues[i] = DataType(0.0);
    return paramContribValues;
  }


  void ConjugateNormal::formLikeParamContrib(NodeId likeId,
      DataType::Array & likeParamContribValues)
  {
    GraphTypes::DirectParentNodeIdIterator it_parents = pGraph_->GetParents(likeId).first;

    NodeId prec_id = *(++it_parents);
    Scalar like_prec = getNodeValue(prec_id, pGraph_, this).ScalarView();
    likeParamContribValues[0].ScalarView() += pGraph_->GetValues()[likeId]->ScalarView() * like_prec;
    likeParamContribValues[1].ScalarView() += like_prec;
  }


  DataType::Array ConjugateNormal::postParam(const DataType::Array & priorParamValues,
      const DataType::Array & likeParamContribValues) const
  {
    Scalar post_prec = priorParamValues[1].ScalarView() + likeParamContribValues[1].ScalarView();
    Scalar post_mean = ( priorParamValues[0].ScalarView() * priorParamValues[1].ScalarView() + likeParamContribValues[0].ScalarView() ) / post_prec;

    DataType::Array post_param_values(2);
    post_param_values[0] = DataType(post_mean);
    post_param_values[1] = DataType(post_prec);
    return post_param_values;
  }


  Scalar ConjugateNormal::computeLogWeight(const DataType & sampledData,
      const DataType::Array & priorParamValues,
      const DataType::Array & postParamValues,
      const DataType::Array & LikeParamContrib)
  {
    Scalar like_mean_contrib = LikeParamContrib[0].ScalarView();
    Scalar like_prec_contrib = LikeParamContrib[1].ScalarView();
    Scalar prior_prec = priorParamValues[1].ScalarView();
    DataType::Array norm_const_param_values(2);
    norm_const_param_values[0] = DataType(like_mean_contrib / like_prec_contrib);
    norm_const_param_values[1] = DataType( 1.0 / (1.0 / prior_prec + 1.0 / like_prec_contrib) );
    return DNorm::Instance()->LogUnnormPdf(priorParamValues[0], norm_const_param_values);
  }


}
