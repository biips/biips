//                                               -*- C++ -*-
/*! \file GetNodeValueVisitor.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "sampler/GetNodeValueVisitor.hpp"
#include "graph/NodeVisitor.hpp"
#include "graph/Graph.hpp"
#include "sampler/NodeSampler.hpp"
#include "graph/ConstantNode.hpp"
#include "graph/LogicalNode.hpp"
#include "graph/StochasticNode.hpp"
#include "model/Monitor.hpp"

namespace Biips
{
  // -----------------------------------------------------------------
  // Get Node Value
  // -----------------------------------------------------------------

  class GetNodeValueVisitor : public ConstNodeVisitor
  {
  protected:
    const Graph & graph_;
    NodeSampler & nodeSampler_;
    MultiArray value_;

    virtual void visit(const ConstantNode & node);
    virtual void visit(const StochasticNode & node);
    virtual void visit(const LogicalNode & node);

  public:

    const MultiArray & GetValue() const { return value_; };

    GetNodeValueVisitor(const Graph & graph, NodeSampler & nodeSampler)
      : graph_(graph), nodeSampler_(nodeSampler) {};
  };


  void GetNodeValueVisitor::visit(const ConstantNode & node)
  {
     value_ = MultiArray(node.DimPtr(), graph_.GetValues()[nodeId_]);
  }


  void GetNodeValueVisitor::visit(const StochasticNode & node)
  {
    if ( !nodeSampler_.sampledFlagsMap_[nodeId_] )
      throw LogicError("GetNodeValueVisitor can not visit StochasticNode: node has not been sampled.");

    if (graph_.GetObserved()[nodeId_])
      value_ = MultiArray(node.DimPtr(), graph_.GetValues()[nodeId_]);
    else
      value_ = MultiArray(node.DimPtr(), nodeSampler_.nodeValuesMap_[nodeId_]);
  }


  void GetNodeValueVisitor::visit(const LogicalNode & node)
  {
    if ( !nodeSampler_.sampledFlagsMap_[nodeId_] )
    {
      NodeId backup_node_id = nodeSampler_.nodeId_;
      nodeSampler_.SetNodeId(nodeId_);
      node.AcceptVisitor(nodeSampler_);
      nodeSampler_.SetNodeId(backup_node_id);
    }

    if (graph_.GetObserved()[nodeId_])
      value_ = MultiArray(node.DimPtr(), graph_.GetValues()[nodeId_]);
    else
      value_ = MultiArray(node.DimPtr(), nodeSampler_.nodeValuesMap_[nodeId_]);
  }


  MultiArray getNodeValue(NodeId nodeId, const Graph & graph, NodeSampler & nodeSampler)
  {
    GetNodeValueVisitor get_val_vis(graph, nodeSampler);
    graph.VisitNode(nodeId, get_val_vis);
    return get_val_vis.GetValue();
  }


  MultiArray getNodeValue(NodeId nodeId, const Graph & graph, const Monitor & monitor, Size particleIndex)
  {
    if (graph.GetObserved()[nodeId])
      return MultiArray(graph.GetNode(nodeId).DimPtr(), graph.GetValues()[nodeId]);
    else
      return MultiArray(graph.GetNode(nodeId).DimPtr(), monitor.GetNodeValues(nodeId)[particleIndex]);
  }


  // -----------------------------------------------------------------
  // Is Bounded
  // -----------------------------------------------------------------

  class IsBoundedVisitor : public ConstStochasticNodeVisitor
  {
  protected:
    Bool lower_;
    Bool upper_;

    virtual void visit(const StochasticNode & node);

  public:
    Bool IsLowerBounded() const { return lower_; };
    Bool IsUpperBounded() const { return upper_; };
    Bool IsBounded() const { return lower_ || upper_; };

    IsBoundedVisitor() : lower_(false), upper_(false) {};
  };


  void IsBoundedVisitor::visit(const StochasticNode & node)
  {
    lower_ = node.IsLowerBounded();
    upper_ = node.IsUpperBounded();
  }


//  Bool isBounded(NodeId nodeId, const Graph & graph)
//  {
//    IsBoundedVisitor is_bounded_vis;
//    graph.VisitNode(nodeId, is_bounded_vis);
//    return is_bounded_vis.IsBounded();
//  }


  // -----------------------------------------------------------------
  // Get Parameters Values
  // -----------------------------------------------------------------

  class GetParamValuesVisitor : public ConstNodeVisitor
  {
  protected:
    const Graph & graph_;
    NodeSampler & nodeSampler_;
    MultiArray::Array values_;

    virtual void visit(const ConstantNode & node) {};
    virtual void visit(const StochasticNode & node);
    virtual void visit(const LogicalNode & node);

  public:

    const MultiArray::Array & GetParamValues() const { return values_; };

    explicit GetParamValuesVisitor(const Graph & graph, NodeSampler & nodeSampler) : graph_(graph), nodeSampler_(nodeSampler) {};
  };


  void GetParamValuesVisitor::visit(const StochasticNode & node)
  {
    GraphTypes::DirectParentNodeIdIterator it_param, it_param_end;
    boost::tie(it_param, it_param_end) = graph_.GetParents(nodeId_);

    if (node.IsUpperBounded())
      --it_param_end;
    if (node.IsLowerBounded())
      --it_param_end;

    MultiArray::Array param_values(std::distance(it_param, it_param_end));
    Size i = 0;
    while (it_param != it_param_end)
    {
      param_values[i] = getNodeValue(*it_param, graph_, nodeSampler_);
      ++it_param;
      ++i;
    }

    values_.SetPtr(param_values);
  }


  void GetParamValuesVisitor::visit(const LogicalNode & node)
  {
    GraphTypes::DirectParentNodeIdIterator it_param, it_param_end;
    boost::tie(it_param, it_param_end) = graph_.GetParents(nodeId_);

    MultiArray::Array param_values(std::distance(it_param, it_param_end));
    Size i = 0;
    while (it_param != it_param_end)
    {
      param_values[i] = getNodeValue(*it_param, graph_, nodeSampler_);
      ++it_param;
      ++i;
    }
    values_.SetPtr(param_values);
  }


  MultiArray::Array getParamValues(NodeId nodeId, const Graph & graph, NodeSampler & nodeSampler)
  {
    GetParamValuesVisitor get_param_val_vis(graph, nodeSampler);
    graph.VisitNode(nodeId, get_param_val_vis);
    return get_param_val_vis.GetParamValues();
  }


  MultiArray::Array getParamValues(NodeId nodeId, const Graph & graph, const Monitor & monitor, Size particleIndex)
  {
    GraphTypes::DirectParentNodeIdIterator it_param, it_param_end;
    boost::tie(it_param, it_param_end) = graph.GetParents(nodeId);

    IsBoundedVisitor is_bounded_vis;
    graph.VisitNode(nodeId, is_bounded_vis);
    if (is_bounded_vis.IsUpperBounded())
      --it_param_end;
    if (is_bounded_vis.IsLowerBounded())
      --it_param_end;

    MultiArray::Array param_values(std::distance(it_param, it_param_end));
    Size i = 0;
    while (it_param != it_param_end)
    {
      param_values[i] = getNodeValue(*it_param, graph, monitor, particleIndex);
      ++it_param;
      ++i;
    }
    return param_values;
  }


  // -----------------------------------------------------------------
  // Get Boundaries Values
  // -----------------------------------------------------------------

  class GetBoundValuesVisitor : public ConstStochasticNodeVisitor
  {
  protected:
    const Graph & graph_;
    NodeSampler & nodeSampler_;
    MultiArray::Pair values_;

    virtual void visit(const StochasticNode & node);

  public:

    const MultiArray::Pair & GetBoundValues() const { return values_; };

    explicit GetBoundValuesVisitor(const Graph & graph, NodeSampler & nodeSampler) : graph_(graph), nodeSampler_(nodeSampler) {};
  };


  void GetBoundValuesVisitor::visit(const StochasticNode & node)
  {
    GraphTypes::DirectParentNodeIdIterator it_param, it_param_end;
    boost::tie(it_param, it_param_end) = graph_.GetParents(nodeId_);

    if (node.IsUpperBounded())
    {
      --it_param_end;
      values_.second.SetPtr(getNodeValue(*it_param_end, graph_, nodeSampler_));
    }
    if (node.IsLowerBounded())
    {
      --it_param_end;
      values_.first.SetPtr(getNodeValue(*it_param_end, graph_, nodeSampler_));
    }
  }


  MultiArray::Pair getBoundValues(NodeId nodeId, const Graph & graph, NodeSampler & nodeSampler)
  {
    GetBoundValuesVisitor get_bound_val_vis(graph, nodeSampler);
    graph.VisitNode(nodeId, get_bound_val_vis);
    return get_bound_val_vis.GetBoundValues();
  }


  MultiArray::Pair getBoundValues(NodeId nodeId, const Graph & graph, const Monitor & monitor, Size particleIndex)
  {
    GraphTypes::DirectParentNodeIdIterator it_param, it_param_end;
    boost::tie(it_param, it_param_end) = graph.GetParents(nodeId);

    MultiArray::Pair bound_values;

    IsBoundedVisitor is_bounded_vis;
    graph.VisitNode(nodeId, is_bounded_vis);

    if (is_bounded_vis.IsUpperBounded())
    {
      --it_param_end;
      bound_values.second.SetPtr(getNodeValue(*it_param_end, graph, monitor, particleIndex));
    }
    if (is_bounded_vis.IsLowerBounded())
    {
      --it_param_end;
      bound_values.first.SetPtr(getNodeValue(*it_param_end, graph, monitor, particleIndex));
    }

    return bound_values;
  }


  // -----------------------------------------------------------------
  // Unbounded Support
  // -----------------------------------------------------------------

  class UnboundedSupportVisitor : public ConstStochasticNodeVisitor
  {
  protected:
    MultiArray::Array paramValues_;
    MultiArray::Pair supportValues_;

    virtual void visit(const StochasticNode & node);

  public:
    const MultiArray::Pair & GetSupportValues() { return supportValues_; }

    UnboundedSupportVisitor(const MultiArray::Array & paramValues) : paramValues_(paramValues) {};
  };


  void UnboundedSupportVisitor::visit(const StochasticNode & node)
  {
    supportValues_.SetPtr(node.UnboundedSupport(paramValues_));
  }


  // -----------------------------------------------------------------
  // Get Support Values
  // -----------------------------------------------------------------

  class GetSupportValuesVisitor : public ConstStochasticNodeVisitor
  {
  protected:
    const Graph & graph_;
    NodeSampler & nodeSampler_;
    MultiArray::Pair values_;

    virtual void visit(const StochasticNode & node);

  public:

    const MultiArray::Pair & GetSupportValues() const { return values_; };

    explicit GetSupportValuesVisitor(const Graph & graph, NodeSampler & nodeSampler) : graph_(graph), nodeSampler_(nodeSampler) {};
  };


  void GetSupportValuesVisitor::visit(const StochasticNode & node)
  {
    values_.SetPtr(node.UnboundedSupport(getParamValues(nodeId_, graph_, nodeSampler_)));

    if (node.IsBounded())
    {
      MultiArray::Pair bound_values = getBoundValues(nodeId_, graph_, nodeSampler_);

      if (!bound_values.first.IsNULL())
      {
        for (Size i = 0; i < bound_values.first.Length(); ++i)
        {
          if (values_.first.Values()[i] < bound_values.first.Values()[i])
            values_.first.Values()[i] = bound_values.first.Values()[i];
        }
      }
      if (!bound_values.second.IsNULL())
      {
        for (Size i = 0; i < bound_values.second.Length(); ++i)
        {
          if (values_.second.Values()[i] > bound_values.second.Values()[i])
            values_.second.Values()[i] = bound_values.second.Values()[i];
        }
      }
    }
  }


  MultiArray::Pair getSupportValues(NodeId nodeId, const Graph & graph, NodeSampler & nodeSampler)
  {
    GetSupportValuesVisitor get_support_val_vis(graph, nodeSampler);
    graph.VisitNode(nodeId, get_support_val_vis);
    return get_support_val_vis.GetSupportValues();
  }


  MultiArray::Pair getSupportValues(NodeId nodeId, const Graph & graph, const Monitor & monitor, Size particleIndex)
  {
    MultiArray::Array param_values = getParamValues(nodeId, graph, monitor, particleIndex);
    UnboundedSupportVisitor unbound_support_vis(param_values);
    graph.VisitNode(nodeId, unbound_support_vis);

    MultiArray::Pair support_values = unbound_support_vis.GetSupportValues();

    IsBoundedVisitor is_bounded_vis;
    graph.VisitNode(nodeId, is_bounded_vis);

    if (is_bounded_vis.IsBounded())
    {
      MultiArray::Pair bound_values = getBoundValues(nodeId, graph, monitor, particleIndex);

      if (!bound_values.first.IsNULL())
      {
        for (Size i = 0; i < bound_values.first.Length(); ++i)
        {
          if (support_values.first.Values()[i] < bound_values.first.Values()[i])
            support_values.first.Values()[i] = bound_values.first.Values()[i];
        }
      }
      if (!bound_values.second.IsNULL())
      {
        for (Size i = 0; i < bound_values.second.Length(); ++i)
        {
          if (support_values.second.Values()[i] > bound_values.second.Values()[i])
            support_values.second.Values()[i] = bound_values.second.Values()[i];
        }
      }
    }

    return support_values;
  }


  // -----------------------------------------------------------------
  // Is Support Fixed
  // -----------------------------------------------------------------

  class IsSupportFixedVisitor : public ConstStochasticNodeVisitor
  {
  protected:
    const Graph & graph_;
    Bool fixed_;

    virtual void visit(const StochasticNode & node);

  public:

    Bool IsSupportFixed() const { return fixed_; }

    explicit IsSupportFixedVisitor(const Graph & graph) : graph_(graph) {};
  };


  void IsSupportFixedVisitor::visit(const StochasticNode & node)
  {
    GraphTypes::DirectParentNodeIdIterator it_parent, it_parent_end;
    boost::tie(it_parent, it_parent_end) = graph_.GetParents(nodeId_);
    if (node.IsUpperBounded())
    {
      --it_parent_end;
      if (!graph_.GetObserved()[*it_parent_end])
      {
        fixed_ = false;
        return;
      }
    }
    if (node.IsLowerBounded())
    {
      --it_parent_end;
      if (!graph_.GetObserved()[*it_parent_end])
      {
        fixed_ = false;
        return;
      }
    }
    Flags fixmask(std::distance(it_parent, it_parent_end));
    for (Size i=0; it_parent != it_parent_end; ++it_parent, ++i)
       fixmask[i] = graph_.GetObserved()[*it_parent];

    fixed_ = node.PriorPtr()->IsSupportFixed(fixmask);
  }


  Bool isSupportFixed(NodeId nodeId, const Graph & graph)
  {
    IsSupportFixedVisitor is_support_fix_vis(graph);
    graph.VisitNode(nodeId, is_support_fix_vis);
    return is_support_fix_vis.IsSupportFixed();
  }

}
