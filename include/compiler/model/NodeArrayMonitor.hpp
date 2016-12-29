#ifndef BIIPS_NODEARRAYMONITOR_HPP_
#define BIIPS_NODEARRAYMONITOR_HPP_

#include "model/Monitor.hpp"
#include "common/MultiArray.hpp"
#include "common/IndexRange.hpp"

namespace Biips
{
  class NodeArray;
  class Graph;
  class SymbolTable;

  class NodeArrayMonitor
  {
  protected:
    String name_;
    IndexRange range_;
    Size nParticles_;
    MultiArray values_;
    MultiArray weights_;
    MultiArray ess_;
    MultiArray iterations_;
    MultiArray nodeIds_;
    MultiArray discrete_;
    Types<Types<NodeId>::Array>::Array conditionalNodeIds_;
    Types<Types<String>::Array>::Array conditionalNodeNames_;

    template<typename StorageOrderType>
    void addObservedNode(NodeId id,
                         const IndexRange & subRange,
                         const Graph & graph);
    template<typename StorageOrderType>
    void addMonitoredNode(NodeId id,
                          const IndexRange & subRange,
                          const Monitor* pMonitor);

    void setMembers(const NodeArray & nodeArray,
                    const std::map<NodeId, Monitor*> & monitorsMap,
                    const Graph & graph);
    void setMembers(const NodeArray & nodeArray,
                    const Monitor* pMonitor,
                    const Graph & graph);
    void setConditionalNodeIds(const NodeArray & nodeArray,
                             const std::map<NodeId, Monitor*> & monitorsMap,
                             const Graph & graph);
    void setConditionalNodeNames(const SymbolTable & symtab);

  public:
    // constructor for filtering monitors
    NodeArrayMonitor(const NodeArray & nodeArray,
                     const IndexRange & range,
                     const std::map<NodeId, Monitor*> & monitorsMap,
                     Size nParticles,
                     const Graph & graph,
                     const SymbolTable & symtab);
    // constructor for backward smoothing monitors
    NodeArrayMonitor(const NodeArray & nodeArray,
                     const IndexRange & range,
                     const std::map<NodeId, Monitor*> & monitorsMap,
                     Size nParticles,
                     const Graph & graph,
                     const SymbolTable & symtab,
                     const Types<NodeId>::Array & condNodes);
    // constructor for smoothing monitors
    NodeArrayMonitor(const NodeArray & nodeArray,
                     const IndexRange & range,
                     const Monitor* pMonitor,
                     Size nParticles,
                     const Graph & graph,
                     const SymbolTable & symtab);

    const String & GetName() const
    {
      return name_;
    }
    const IndexRange & GetRange() const
    {
      return range_;
    }
    const MultiArray & GetNodeIds() const
    {
      return nodeIds_;
    }
    const MultiArray & GetIterations() const
    {
      return iterations_;
    }
    const MultiArray & GetESS() const
    {
      return ess_;
    }
    const MultiArray & GetWeights() const
    {
      return weights_;
    }
    const MultiArray & GetValues() const
    {
      return values_;
    }
    const MultiArray & GetDiscrete() const
    {
      return discrete_;
    }
    const Types<Types<NodeId>::Array>::Array & GetConditionalNodeIds() const
    {
      return conditionalNodeIds_;
    }
    const Types<Types<String>::Array>::Array & GetConditionalNodeNames() const
    {
      return conditionalNodeNames_;
    }
  };

  class NodeArrayValue
  {
  protected:
    IndexRange range_;
    MultiArray value_;

    template<typename StorageOrderType>
    void addObservedNode(NodeId id,
                         const IndexRange & subRange,
                         const Graph & graph);
    template<typename StorageOrderType>
    void addMonitoredNode(NodeId id,
                          const IndexRange & subRange,
                          const Monitor* pMonitor,
                          Size particleIndex);

  public:
    NodeArrayValue(const NodeArray & nodeArray,
                   const IndexRange & range,
                   const Monitor* pMonitor,
                   Size particleIndex,
                   const Graph & graph);

    const MultiArray & GetValue() const
    {
      return value_;
    }
  };

}

#endif /* BIIPS_NODEARRAYMONITOR_HPP_ */
