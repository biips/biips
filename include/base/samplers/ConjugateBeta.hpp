#ifndef BIIPS_CONJUGATEBETA_HPP_
#define BIIPS_CONJUGATEBETA_HPP_

#include "distributions/DBeta.hpp"
#include "distributions/DBin.hpp"
#include "samplers/ConjugateSampler.hpp"

namespace Biips
{

  class ConjugateBeta: public ConjugateSampler<DBeta, DBin, 1>
  {
  public:
    typedef ConjugateBeta SelfType;
    typedef ConjugateSampler<DBeta, DBin, 1> BaseType;

    static const String NAME_;

  protected:
    friend class ConjugateSamplerFactory<SelfType> ;

    explicit ConjugateBeta(const Graph & graph) :
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

  typedef ConjugateSamplerFactory<ConjugateBeta> ConjugateBetaFactory;
}

#endif /* BIIPS_CONJUGATEBETA_HPP_ */
