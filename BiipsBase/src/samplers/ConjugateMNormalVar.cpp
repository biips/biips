//                                               -*- C++ -*-
/*! \file ConjugateMNormalVar.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "samplers/ConjugateMNormalVar.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "common/cholesky.hpp"

namespace Biips
{

  const String ConjugateMNormalVar::NAME_ = "Conjugate Multivariate Normal (with known covariance matrix)";


  void ConjugateMNormalVar::formLikeParamContrib(NodeId likeId,
      DataType::Array & likeParamContribValues)
  {
    VectorRef like_mean(likeParamContribValues[0]);
    MatrixRef like_prec(likeParamContribValues[1]);

    GraphTypes::DirectParentNodeIdIterator it_parents = pGraph_->GetParents(likeId).first;

    NodeId var_id = *(++it_parents);

    Matrix prec_i_mat(getNodeValue(var_id, pGraph_, this));

    ublas::cholesky_factorize(prec_i_mat);
    ublas::cholesky_invert(prec_i_mat);

    DataType obs_i(pGraph_->GetNode(likeId).DimPtr(), pGraph_->GetValues()[likeId]);
    VectorRef obs_i_vec(obs_i);

    like_mean += ublas::prod(prec_i_mat, obs_i_vec);
    like_prec += prec_i_mat;
  }


  DataType::Array ConjugateMNormalVar::postParam(const DataType::Array & priorParamValues,
      const DataType::Array & likeParamContribValues) const
  {
    Matrix post_cov(priorParamValues[1]);

    ublas::cholesky_factorize(post_cov);
    ublas::cholesky_invert(post_cov);

    Vector post_mean = ublas::prod(post_cov, Vector(priorParamValues[0])) + Vector(likeParamContribValues[0]);

    post_cov += Matrix(likeParamContribValues[1]);
    ublas::cholesky_factorize(post_cov);
    ublas::cholesky_invert(post_cov);

    post_mean = ublas::prod(post_cov, post_mean);

    DataType::Array post_param_values(2);
    post_param_values[0] = DataType(post_mean);
    post_param_values[1] = DataType(post_cov);
    return post_param_values;
  }


  Scalar ConjugateMNormalVar::computeLogWeight(const DataType & sampledData,
      const DataType::Array & priorParamValues,
      const DataType::Array & postParamValues,
      const DataType::Array & LikeParamContrib)
  {
    Matrix norm_const_cov(LikeParamContrib[1]);
    ublas::cholesky_factorize(norm_const_cov);
    ublas::cholesky_invert(norm_const_cov);

    Vector norm_const_mean = ublas::prod(norm_const_cov, Vector(LikeParamContrib[0]));
    norm_const_cov += Matrix(priorParamValues[1]);

    DataType::Array norm_const_param_values(2);
    norm_const_param_values[0] = DataType(norm_const_mean);
    norm_const_param_values[1] = DataType(norm_const_cov);
    return DMNormVar::Instance()->LogUnnormPdf(priorParamValues[0], norm_const_param_values);
  }


}

