#ifndef BIIPS_CONJUGATEMNORMALCOVLINEAR_HPP_
#define BIIPS_CONJUGATEMNORMALCOVLINEAR_HPP_

#include "sampler/NodeSampler.hpp"

namespace Biips
{

  class ConjugateMNormalCovLinear: public NodeSampler
  {
  public:
    typedef ConjugateMNormalCovLinear SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef NodeSampler BaseType;

  protected:
    static const String NAME_;

    friend class ConjugateMNormalCovLinearFactory;

    explicit ConjugateMNormalCovLinear(const Graph & graph) :
      BaseType(graph)
    {
    }

    virtual void sample(const StochasticNode & node);

  public:
    virtual const String & Name() const
    {
      return NAME_;
    }

    virtual ~ConjugateMNormalCovLinear()
    {
    }
  };

  class ConjugateMNormalCovLinearFactory: public NodeSamplerFactory
  {
  public:
    typedef ConjugateMNormalCovLinearFactory SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef ConjugateMNormalCovLinear CreatedType;
    typedef NodeSamplerFactory BaseType;

  protected:
    static Ptr pConjugateMNormalCovLinearFactoryInstance_;
    ConjugateMNormalCovLinearFactory()
    {
    }

  public:
    static BaseType::Ptr Instance()
    {
      return pConjugateMNormalCovLinearFactoryInstance_;
    }
    virtual Bool Create(const Graph & graph,
                        NodeId nodeId,
                        BaseType::CreatedPtr & pNodeSamplerInstance) const;
  };

}

#endif /* BIIPS_CONJUGATEMNORMALCOVLINEAR_HPP_ */
