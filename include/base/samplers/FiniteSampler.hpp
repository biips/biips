#ifndef BIIPS_FINITESAMPLER_HPP_
#define BIIPS_FINITESAMPLER_HPP_

#include "sampler/NodeSampler.hpp"

namespace Biips
{

  class FiniteSampler: public NodeSampler
  {
  public:
    typedef FiniteSampler SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    static const String NAME_;
    int lower_, upper_;

    friend class FiniteSamplerFactory;

    explicit FiniteSampler(const Graph & graph, int lower, int upper) :
      NodeSampler(graph), lower_(lower), upper_(upper)
    {
    }

    virtual void sample(const StochasticNode & node);

  public:
    virtual const String & Name() const
    {
      return NAME_;
    }

    virtual ~FiniteSampler()
    {
    }
  };


  class FiniteSamplerFactory: public NodeSamplerFactory
  {
  public:
    typedef FiniteSamplerFactory SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef FiniteSampler CreatedType;
    typedef NodeSamplerFactory BaseType;

  protected:
    static Ptr pFiniteSamplerFactoryInstance_;
    FiniteSamplerFactory()
    {
    }

  public:
    static BaseType::Ptr Instance()
    {
      return pFiniteSamplerFactoryInstance_;
    }
    virtual Bool Create(const Graph & graph,
                        NodeId nodeId,
                        BaseType::CreatedPtr & pNodeSamplerInstance) const;
  };

}

#endif /* BIIPS_FINITESAMPLER_HPP_ */
