#ifndef BIIPS_DATANODESAMPLER_HPP_
#define BIIPS_DATANODESAMPLER_HPP_

#include "sampler/NodeSampler.hpp"
namespace Biips
{

  class Graph;

  class DataNodeSampler: public NodeSampler
  {
  public:
    typedef DataNodeSampler SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef NodeSampler BaseType;

  protected:
    static const String NAME_;

    virtual void sample(const StochasticNode & node);

  public:
    explicit DataNodeSampler(const Graph & graph)
        : BaseType(graph)
    {
    }

    virtual ~DataNodeSampler()
    {
    }
  };

} /* namespace Biips */
#endif /* BIIPS_DATANODESAMPLER_HPP_ */
