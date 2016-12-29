#ifndef BIIPS_CONJUGATENORMALVAR_HPP_
#define BIIPS_CONJUGATENORMALVAR_HPP_

#include "distributions/DNormVar.hpp"
#include "samplers/ConjugateSampler.hpp"

namespace Biips
{

  class ConjugateNormalVar: public ConjugateSampler<DNormVar, DNormVar, 0>
  {
  public:
    typedef ConjugateNormalVar SelfType;
    typedef ConjugateSampler<DNormVar, DNormVar, 0> BaseType;

    static const String NAME_;

  protected:
    friend class ConjugateSamplerFactory<SelfType> ;

    explicit ConjugateNormalVar(const Graph & graph) :
      BaseType(graph)
    {
    }

    virtual MultiArray::Array initLikeParamContrib() const;
    virtual void
    formLikeParamContrib(NodeId likeId,
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

  typedef ConjugateSamplerFactory<ConjugateNormalVar> ConjugateNormalVarFactory;

}

#endif /* BIIPS_CONJUGATENORMALVAR_HPP_ */
