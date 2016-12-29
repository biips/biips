#include "samplers/ConjugateMNormalCov.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "common/cholesky.hpp"

namespace Biips
{

  const String ConjugateMNormalCov::NAME_ =
      "ConjugateMNormal_knownCov_linearMean";

  void ConjugateMNormalCov::formLikeParamContrib(NodeId likeId,
                                                 MultiArray::Array & likeParamContribValues)
  {
    VectorRef like_mean(likeParamContribValues[0]);
    MatrixRef like_prec(likeParamContribValues[1]);

    GraphTypes::ParentIterator it_parents = graph_.GetParents(likeId).first;

    NodeId var_id = *(++it_parents);

    Matrix prec_i_mat(getNodeValue(var_id, graph_, *this));

    if (!ublas::cholesky_factorize(prec_i_mat))
      throw LogicError("ConjugateMNormalCov::formLikeParamContrib: matrix prec_i_mat is not positive-semidefinite.");
    ublas::cholesky_invert(prec_i_mat);

    NumArray obs_i(graph_.GetNode(likeId).DimPtr().get(),
                   graph_.GetValues()[likeId].get());
    VectorRef obs_i_vec(obs_i);

    like_mean += ublas::prod(prec_i_mat, obs_i_vec);
    like_prec += prec_i_mat;
  }

  MultiArray::Array ConjugateMNormalCov::postParam(const NumArray::Array & priorParamValues,
                                                 const MultiArray::Array & likeParamContribValues) const
  {
    MultiArray::Array post_param_values(2);
    ValArray::Ptr post_mean_val(new ValArray(likeParamContribValues[0].Values()));
    post_param_values[0].SetPtr(likeParamContribValues[0].DimPtr(), post_mean_val);

    ValArray::Ptr post_cov_val(new ValArray(priorParamValues[1].Values()));
    post_param_values[1].SetPtr(likeParamContribValues[1].DimPtr(), post_cov_val);

    VectorRef post_mean(post_param_values[0]);
    MatrixRef post_cov(post_param_values[1]);

    if (!ublas::cholesky_factorize(post_cov))
      throw LogicError("ConjugateMNormalCov::postParam: matrix post_cov_mat is not positive-semidefinite.");
    ublas::cholesky_invert(post_cov);

    post_mean += ublas::prod(post_cov, Vector(priorParamValues[0]));

    post_cov += Matrix(likeParamContribValues[1]);

    if (!ublas::cholesky_factorize(post_cov))
      throw LogicError("ConjugateMNormal::postParam: matrix post_cov is not positive-semidefinite.");
    ublas::cholesky_invert(post_cov);

    post_mean = ublas::prod(post_cov, post_mean);

    return post_param_values;
  }

  Scalar ConjugateMNormalCov::computeLogIncrementalWeight(const NumArray & sampledData,
                                                          const NumArray::Array & priorParamValues,
                                                          const NumArray::Array & postParamValues,
                                                          const MultiArray::Array & likeParamContrib)
  {
    Matrix norm_const_cov;
    norm_const_cov = Matrix(likeParamContrib[1]);
    if (!ublas::cholesky_factorize(norm_const_cov))
      throw LogicError("ConjugateMNormalCov::computeLogIncrementalWeight: matrix norm_const_cov is not positive-semidefinite.");
    ublas::cholesky_invert(norm_const_cov);

    Vector norm_const_mean;
    norm_const_mean = ublas::prod(norm_const_cov, Vector(likeParamContrib[0]));
    norm_const_cov += Matrix(priorParamValues[1]);

    NumArray::Array norm_const_param_values(2);
    DimArray dim_mean(1);
    dim_mean[0] = norm_const_mean.size();
    norm_const_param_values[0] = NumArray(&dim_mean, &norm_const_mean.data());
    DimArray dim_cov(2);
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

