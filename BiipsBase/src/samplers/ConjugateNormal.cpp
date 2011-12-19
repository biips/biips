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


  MultiArray::Array ConjugateNormal::initLikeParamContrib() const
  {
    MultiArray::Array paramContribValues(PriorDistType::Instance()->NParam());
    for (Size i = 0; i < paramContribValues.size(); ++i)
      paramContribValues[i] = MultiArray(0.0);
    return paramContribValues;
  }


  void ConjugateNormal::formLikeParamContrib(NodeId likeId,
      MultiArray::Array & likeParamContribValues)
  {
    GraphTypes::ParentIterator it_parents = graph_.GetParents(likeId).first;

    NodeId prec_id = *(++it_parents);
    Scalar like_prec = getNodeValue(prec_id, graph_, *this).ScalarView();
    likeParamContribValues[0].ScalarView() += graph_.GetValues()[likeId]->ScalarView() * like_prec;
    likeParamContribValues[1].ScalarView() += like_prec;
  }


  MultiArray::Array ConjugateNormal::postParam(const MultiArray::Array & priorParamValues,
      const MultiArray::Array & likeParamContribValues) const
  {
    Scalar post_prec = priorParamValues[1].ScalarView() + likeParamContribValues[1].ScalarView();
    Scalar post_mean = ( priorParamValues[0].ScalarView() * priorParamValues[1].ScalarView() + likeParamContribValues[0].ScalarView() ) / post_prec;

    MultiArray::Array post_param_values(2);
    post_param_values[0] = MultiArray(post_mean);
    post_param_values[1] = MultiArray(post_prec);
    return post_param_values;
  }


  Scalar ConjugateNormal::computeLogIncrementalWeight(const MultiArray & sampledData,
      const MultiArray::Array & priorParamValues,
      const MultiArray::Array & postParamValues,
      const MultiArray::Array & LikeParamContrib)
  {
    Scalar like_mean_contrib = LikeParamContrib[0].ScalarView();
    Scalar like_prec_contrib = LikeParamContrib[1].ScalarView();
    Scalar prior_prec = priorParamValues[1].ScalarView();
    MultiArray::Array norm_const_param_values(2);
    norm_const_param_values[0] = MultiArray(like_mean_contrib / like_prec_contrib);
    norm_const_param_values[1] = MultiArray( 1.0 / (1.0 / prior_prec + 1.0 / like_prec_contrib) );

    Scalar log_incr_weight = DNorm::Instance()->LogDensity(priorParamValues[0], norm_const_param_values, NULL_MULTIARRAYPAIR); // FIXME Boundaries
    if (isNan(log_incr_weight))
      throw RuntimeError("Failure to calculate log incremental weight.");

    return log_incr_weight;
  }


}
