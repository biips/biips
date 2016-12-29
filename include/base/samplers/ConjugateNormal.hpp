#ifndef BIIPS_CONJUGATENORMAL_HPP_
#define BIIPS_CONJUGATENORMAL_HPP_

#include "distributions/DNorm.hpp"
#include "samplers/ConjugateSampler.hpp"

namespace Biips
{

  class ConjugateNormal: public ConjugateSampler<DNorm, DNorm, 0>
  {
  public:
    typedef ConjugateNormal SelfType;
    typedef ConjugateSampler<DNorm, DNorm, 0> BaseType;

    static const String NAME_;

  protected:
    friend class ConjugateSamplerFactory<SelfType> ;

    explicit ConjugateNormal(const Graph & graph) :
      BaseType(graph)
    {
    }

    virtual MultiArray::Array initLikeParamContrib() const;
    virtual void formLikeParamContrib(NodeId likeId,
                                      MultiArray::Array & likeParamContribValues);
    virtual MultiArray::Array
    postParam(const NumArray::Array & priorParamContribValues,
              const MultiArray::Array & likeParamContribValues) const;
    virtual Scalar
    computeLogIncrementalWeight(const NumArray & sampledData,
                                const NumArray::Array & priorParamValues,
                                const NumArray::Array & postParamValues,
                                const MultiArray::Array & LikeParamContrib);

  public:
    virtual const String & Name() const
    {
      return NAME_;
    }

  };

  typedef ConjugateSamplerFactory<ConjugateNormal> ConjugateNormalFactory;

}

#endif /* BIIPS_CONJUGATENORMAL_HPP_ */
