//                                               -*- C++ -*-
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

  const String ConjugateMNormal::NAME_ = "Conjugate Multivariate Normal (with known precision matrix)";


  void ConjugateMNormal::formLikeParamContrib(NodeId likeId,
      DataType::Array & likeParamContribValues)
  {
    VectorRef like_mean(likeParamContribValues[0]);
    MatrixRef like_prec(likeParamContribValues[1]);

    GraphTypes::DirectParentNodeIdIterator it_parents = pGraph_->GetParents(likeId).first;

    NodeId prec_id = *(++it_parents);

    Matrix prec_i_mat(getNodeValue(prec_id, pGraph_, this));

    DataType obs_i(pGraph_->GetNode(likeId).DimPtr(), pGraph_->GetValues()[likeId]);
    VectorRef obs_i_vec(obs_i);

    like_mean += ublas::prod(prec_i_mat, obs_i_vec);
    like_prec += prec_i_mat;
  }


  DataType::Array ConjugateMNormal::postParam(const DataType::Array & priorParamValues,
      const DataType::Array & likeParamContribValues) const
  {
    Matrix post_prec(priorParamValues[1]);

    Vector post_mean = ublas::prod(post_prec, Vector(priorParamValues[0])) + Vector(likeParamContribValues[0]);

    post_prec += Matrix(likeParamContribValues[1]);

    Matrix post_cov = post_prec;
    ublas::cholesky_factorize(post_cov);
    ublas::cholesky_invert(post_cov);

    post_mean = ublas::prod(post_cov, post_mean);

    DataType::Array post_param_values(2);
    post_param_values[0] = DataType(post_mean);
    post_param_values[1] = DataType(post_prec);
    return post_param_values;
  }


  Scalar ConjugateMNormal::computeLogWeight(const DataType & sampledData,
      const DataType::Array & priorParamValues,
      const DataType::Array & postParamValues,
      const DataType::Array & LikeParamContrib)
  {
    Matrix norm_const_prec(LikeParamContrib[1]);
    ublas::cholesky_factorize(norm_const_prec);
    ublas::cholesky_invert(norm_const_prec);

    Vector norm_const_mean = ublas::prod(norm_const_prec, Vector(LikeParamContrib[0]));

    Matrix prior_prec(priorParamValues[1]);
    ublas::cholesky_factorize(prior_prec);
    ublas::cholesky_invert(prior_prec);

    norm_const_prec += prior_prec;
    ublas::cholesky_factorize(norm_const_prec);
    ublas::cholesky_invert(norm_const_prec);

    DataType::Array norm_const_param_values(2);
    norm_const_param_values[0] = DataType(norm_const_mean);
    norm_const_param_values[1] = DataType(norm_const_prec);
    return DMNorm::Instance()->LogUnnormPdf(priorParamValues[0], norm_const_param_values);
  }


}
