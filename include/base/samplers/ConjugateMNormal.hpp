#ifndef BIIPS_CONJUGATEMNORMAL_HPP_
#define BIIPS_CONJUGATEMNORMAL_HPP_

#include "distributions/DMNorm.hpp"
#include "samplers/ConjugateSampler.hpp"

namespace Biips
{

  class ConjugateMNormal: public ConjugateSampler<DMNorm, DMNorm, 0>
  {
  public:
    typedef ConjugateMNormal SelfType;
    typedef ConjugateSampler<DMNorm, DMNorm, 0> BaseType;

    static const String NAME_;

  protected:
    friend class ConjugateSamplerFactory<SelfType> ;

    explicit ConjugateMNormal(const Graph & graph) :
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

  typedef ConjugateSamplerFactory<ConjugateMNormal> ConjugateMNormalFactory;

}

#endif /* BIIPS_CONJUGATEMNORMAL_HPP_ */
