#include "samplers/ConjugateNormalVar.hpp"
#include "sampler/GetNodeValueVisitor.hpp"

namespace Biips
{

  const String ConjugateNormalVar::NAME_ =
      "ConjugateNormal_knownVar";

  MultiArray::Array ConjugateNormalVar::initLikeParamContrib() const
  {
    MultiArray::Array paramContribValues(2);
    ValArray::Ptr mean_valptr(new ValArray(1, 0.0));
    ValArray::Ptr var_valptr(new ValArray(1, 0.0));
    paramContribValues[0].SetPtr(P_SCALAR_DIM, mean_valptr);
    paramContribValues[1].SetPtr(P_SCALAR_DIM, var_valptr);

    return paramContribValues;
  }

  void ConjugateNormalVar::formLikeParamContrib(NodeId likeId,
                                                MultiArray::Array & likeParamContribValues)
  {
    GraphTypes::ParentIterator it_parents = graph_.GetParents(likeId).first;

    NodeId var_id = *(++it_parents);
    Scalar like_var = getNodeValue(var_id, graph_, *this).ScalarView();
    likeParamContribValues[0].ScalarView()
        += graph_.GetValues()[likeId]->ScalarView() / like_var;
    likeParamContribValues[1].ScalarView() += 1.0 / like_var;
  }

  MultiArray::Array ConjugateNormalVar::postParam(const NumArray::Array & priorParamValues,
                                                const MultiArray::Array & likeParamContribValues) const
  {
    ValArray::Ptr post_var(new ValArray(1));
    (*post_var)[0] = 1.0 / (1.0 / priorParamValues[1].ScalarView()
        + likeParamContribValues[1].ScalarView());
    ValArray::Ptr post_mean(new ValArray(1));
    (*post_mean)[0] = (*post_var)[0] * (priorParamValues[0].ScalarView()
        / priorParamValues[1].ScalarView()
        + likeParamContribValues[0].ScalarView());

    MultiArray::Array post_param_values(2);
    post_param_values[0].SetPtr(P_SCALAR_DIM, post_mean);
    post_param_values[1].SetPtr(P_SCALAR_DIM, post_var);
    return post_param_values;
  }

  Scalar ConjugateNormalVar::computeLogIncrementalWeight(const NumArray & sampledData,
                                                         const NumArray::Array & priorParamValues,
                                                         const NumArray::Array & postParamValues,
                                                         const MultiArray::Array & likeParamContrib)
  {
    Scalar like_mean_contrib = likeParamContrib[0].ScalarView();
    Scalar like_var_inv_contrib = likeParamContrib[1].ScalarView();
    Scalar prior_var = priorParamValues[1].ScalarView();
    NumArray::Array norm_const_param_values(2);
    ValArray norm_const_mean_val(1);
    norm_const_mean_val[0] = like_mean_contrib / like_var_inv_contrib;
    norm_const_param_values[0].SetPtr(P_SCALAR_DIM.get(), &norm_const_mean_val);
    ValArray norm_const_var_val(1);
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
