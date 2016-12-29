#ifndef BIIPS_CONJUGATEMNORMALCOV_HPP_
#define BIIPS_CONJUGATEMNORMALCOV_HPP_

#include "distributions/DMNormVar.hpp"
#include "samplers/ConjugateSampler.hpp"

namespace Biips
{

  class ConjugateMNormalCov: public ConjugateSampler<DMNormVar, DMNormVar, 0>
  {
  public:
    typedef ConjugateMNormalCov SelfType;
    typedef ConjugateSampler<DMNormVar, DMNormVar, 0> BaseType;

    static const String NAME_;

  protected:
    friend class ConjugateSamplerFactory<SelfType> ;

    explicit ConjugateMNormalCov(const Graph & graph) :
      BaseType(graph)
    {
    }

    virtual void formLikeParamContrib(NodeId likeId,
                                      MultiArray::Array & likeParamContribValues);
    virtual MultiArray::Array
    postParam(const NumArray::Array & priorParamContribValues,
              const MultiArray::Array & likeParamContribValues) const;
    virtual Scalar
    computeLogIncrementalWeight(const NumArray & sampledData,
                                const NumArray::Array & priorParamValues,
                                const NumArray::Array & postParamValues,
                                const MultiArray::Array & likeParamContrib);

  public:
    virtual const String & Name() const
    {
      return NAME_;
    }

  };

  typedef ConjugateSamplerFactory<ConjugateMNormalCov>
      ConjugateMNormalCovFactory;

}

#endif /* BIIPS_CONJUGATEMNORMALCOV_HPP_ */
