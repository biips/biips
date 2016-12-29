//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Biips is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of Biips.
 *
 * Biips is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

  class GetNodeValueVisitor: public ConstNodeVisitor
  {
  protected:
    const Graph & graph_;
    NodeSampler & nodeSampler_;
    NumArray value_;

    virtual void visit(const ConstantNode & node);
    virtual void visit(const StochasticNode & node);
    virtual void visit(const LogicalNode & node);

  public:

    const NumArray & GetValue() const
    {
      return value_;
    }

    GetNodeValueVisitor(const Graph & graph, NodeSampler & nodeSampler) :
      graph_(graph), nodeSampler_(nodeSampler)
    {
    }
  };

  void GetNodeValueVisitor::visit(const ConstantNode & node)
  {
    value_ = NumArray(node.DimPtr().get(), graph_.GetValues()[nodeId_].get());
  }

  void GetNodeValueVisitor::visit(const StochasticNode & node)
  {
    if (!nodeSampler_.sampledFlagsMap()[nodeId_])
      throw LogicError("GetNodeValueVisitor can not visit StochasticNode: node has not been sampled.");

    if (graph_.GetObserved()[nodeId_])
      value_ = NumArray(node.DimPtr().get(), graph_.GetValues()[nodeId_].get());
    else
      value_ = NumArray(node.DimPtr().get(),
                        nodeSampler_.nodeValuesMap()[nodeId_].get());
  }

  void GetNodeValueVisitor::visit(const LogicalNode & node)
  {
    if (!nodeSampler_.sampledFlagsMap()[nodeId_])
    {
      NodeId backup_node_id = nodeSampler_.nodeId_;
      nodeSampler_.SetNodeId(nodeId_);
      nodeSampler_.Visit(node);
      nodeSampler_.SetNodeId(backup_node_id);
    }

    if (graph_.GetObserved()[nodeId_])
      value_ = NumArray(node.DimPtr().get(), graph_.GetValues()[nodeId_].get());
    else
      value_ = NumArray(node.DimPtr().get(),
                        nodeSampler_.nodeValuesMap()[nodeId_].get());
  }

  NumArray getNodeValue(NodeId nodeId,
                        const Graph & graph,
                        NodeSampler & nodeSampler)
  {
    GetNodeValueVisitor get_val_vis(graph, nodeSampler);
    graph.VisitNode(nodeId, get_val_vis);
    return get_val_vis.GetValue();
  }

  NumArray getNodeValue(NodeId nodeId,
                        const Graph & graph,
                        const Monitor & monitor,
                        Size particleIndex)
  {
    if (graph.GetObserved()[nodeId])
      return NumArray(graph.GetNode(nodeId).DimPtr().get(),
                      graph.GetValues()[nodeId].get());
    else
      return NumArray(graph.GetNode(nodeId).DimPtr().get(),
                      monitor.GetNodeValues(nodeId)[particleIndex].get());
  }

  // -----------------------------------------------------------------
  // Is Bounded
  // -----------------------------------------------------------------

  class IsBoundedVisitor: public ConstNodeVisitor
  {
  protected:
    Bool lower_;
    Bool upper_;

    virtual void visit(const StochasticNode & node);

  public:
    Bool IsLowerBounded() const
    {
      return lower_;
    }
    Bool IsUpperBounded() const
    {
      return upper_;
    }
    Bool IsBounded() const
    {
      return lower_ || upper_;
    }

    IsBoundedVisitor() :
      lower_(false), upper_(false)
    {
    }
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

  class GetParamValuesVisitor: public ConstNodeVisitor
  {
  protected:
    const Graph & graph_;
    NodeSampler & nodeSampler_;
    NumArray::Array values_;

    virtual void visit(const ConstantNode & node)
    {
    }

    virtual void visit(const StochasticNode & node);
    virtual void visit(const LogicalNode & node);

  public:

    const NumArray::Array & GetParamValues() const
    {
      return values_;
    }

    explicit GetParamValuesVisitor(const Graph & graph,
                                   NodeSampler & nodeSampler) :
      graph_(graph), nodeSampler_(nodeSampler)
    {
    }
  };

  void GetParamValuesVisitor::visit(const StochasticNode & node)
  {
    GraphTypes::ParentIterator it_param, it_param_end;
    boost::tie(it_param, it_param_end) = graph_.GetParents(nodeId_);

    if (node.IsUpperBounded())
      --it_param_end;
    if (node.IsLowerBounded())
      --it_param_end;

    NumArray::Array param_values(std::distance(it_param, it_param_end));
    Size i = 0;
    while (it_param != it_param_end)
    {
      param_values[i] = getNodeValue(*it_param, graph_, nodeSampler_);
      ++it_param;
      ++i;
    }

    values_ = param_values;
  }

  void GetParamValuesVisitor::visit(const LogicalNode & node)
  {
    GraphTypes::ParentIterator it_param, it_param_end;
    boost::tie(it_param, it_param_end) = graph_.GetParents(nodeId_);

    NumArray::Array param_values(std::distance(it_param, it_param_end));
    Size i = 0;
    while (it_param != it_param_end)
    {
      param_values[i] = getNodeValue(*it_param, graph_, nodeSampler_);
      ++it_param;
      ++i;
    }
    values_ = param_values;
  }

  NumArray::Array getParamValues(NodeId nodeId,
                                 const Graph & graph,
                                 NodeSampler & nodeSampler)
  {
    GetParamValuesVisitor get_param_val_vis(graph, nodeSampler);
    graph.VisitNode(nodeId, get_param_val_vis);
    return get_param_val_vis.GetParamValues();
  }

  NumArray::Array getParamValues(NodeId nodeId,
                                 const Graph & graph,
                                 const Types<Monitor*>::Array & monitors,
                                 Size particleIndex)
  {
    GraphTypes::ParentIterator it_param, it_param_end;
    boost::tie(it_param, it_param_end) = graph.GetParents(nodeId);

    IsBoundedVisitor is_bounded_vis;
    graph.VisitNode(nodeId, is_bounded_vis);
    if (is_bounded_vis.IsUpperBounded())
      --it_param_end;
    if (is_bounded_vis.IsLowerBounded())
      --it_param_end;

    Size n_par = std::distance(it_param, it_param_end);

    if (monitors.size() != n_par)
      throw LogicError("getParamValues: incorrect monitors size.");

    NumArray::Array param_values(n_par);
    for (Size i = 0; it_param != it_param_end; ++it_param, ++i)
    {
      if (!monitors[i])
        throw LogicError("getParamValues: null monitor pointer.");

      param_values[i] = getNodeValue(*it_param,
                                     graph,
                                     *monitors[i],
                                     particleIndex);
    }
    return param_values;
  }

  // -----------------------------------------------------------------
  // Get Boundaries Values
  // -----------------------------------------------------------------

  class GetBoundValuesVisitor: public ConstNodeVisitor
  {
  protected:
    const Graph & graph_;
    NodeSampler & nodeSampler_;
    NumArray::Pair values_;

    virtual void visit(const StochasticNode & node);

  public:

    const NumArray::Pair & GetBoundValues() const
    {
      return values_;
    }

    explicit GetBoundValuesVisitor(const Graph & graph,
                                   NodeSampler & nodeSampler) :
      graph_(graph), nodeSampler_(nodeSampler)
    {
    }
  };

  void GetBoundValuesVisitor::visit(const StochasticNode & node)
  {
    GraphTypes::ParentIterator it_param, it_param_end;
    boost::tie(it_param, it_param_end) = graph_.GetParents(nodeId_);

    if (node.IsUpperBounded())
    {
      --it_param_end;
      values_.second = getNodeValue(*it_param_end, graph_, nodeSampler_);
    }
    if (node.IsLowerBounded())
    {
      --it_param_end;
      values_.first = getNodeValue(*it_param_end, graph_, nodeSampler_);
    }
  }

  NumArray::Pair getBoundValues(NodeId nodeId,
                                const Graph & graph,
                                NodeSampler & nodeSampler)
  {
    GetBoundValuesVisitor get_bound_val_vis(graph, nodeSampler);
    graph.VisitNode(nodeId, get_bound_val_vis);
    return get_bound_val_vis.GetBoundValues();
  }

  NumArray::Pair getBoundValues(NodeId nodeId,
                                const Graph & graph,
                                const Types<Monitor*>::Pair & monitors,
                                Size particleIndex)
  {
    GraphTypes::ParentIterator it_param, it_param_end;
    boost::tie(it_param, it_param_end) = graph.GetParents(nodeId);

    NumArray::Pair bound_values;

    IsBoundedVisitor is_bounded_vis;
    graph.VisitNode(nodeId, is_bounded_vis);

    if (is_bounded_vis.IsUpperBounded())
    {
      if (!monitors.second)
        throw LogicError("getBoundValues: null monitor pointer.");

      --it_param_end;
      bound_values.second = getNodeValue(*it_param_end,
                                         graph,
                                         *monitors.second,
                                         particleIndex);
    }
    if (is_bounded_vis.IsLowerBounded())
    {
      if (!monitors.first)
        throw LogicError("getBoundValues: null monitor pointer.");

      --it_param_end;
      bound_values.first = getNodeValue(*it_param_end,
                                        graph,
                                        *monitors.first,
                                        particleIndex);
    }

    return bound_values;
  }

//  // -----------------------------------------------------------------
//  // Unbounded Support
//  // -----------------------------------------------------------------
//
//  class fixedUnboundedSupportVisitor: public ConstNodeVisitor
//  {
//  protected:
//    NumArray::Array paramValues_;
//    ValArray & lower_;
//    ValArray & upper_;
//
//    virtual void visit(const StochasticNode & node);
//
//  public:
//    fixedUnboundedSupportVisitor(const NumArray::Array & paramValues,
//                            ValArray & lower,
//                            ValArray & upper) :
//      paramValues_(paramValues), lower_(lower), upper_(upper)
//    {
//    }
//  };
//
//  void fixedUnboundedSupportVisitor::visit(const StochasticNode & node)
//  {
//    node.fixedUnboundedSupport(lower_, upper_, paramValues_);
//  }

  // -----------------------------------------------------------------
  // Get Support Values
  // -----------------------------------------------------------------

  class GetFixedSupportValuesVisitor: public ConstNodeVisitor
  {
  protected:
    const Graph & graph_;
    ValArray & lower_;
    ValArray & upper_;

    virtual void visit(const StochasticNode & node);

  public:
    explicit GetFixedSupportValuesVisitor(const Graph & graph,
                                     ValArray & lower,
                                     ValArray & upper) :
      graph_(graph), lower_(lower), upper_(upper)
    {
    }
  };

  void GetFixedSupportValuesVisitor::visit(const StochasticNode & node)
  {
    // get observed parents values
    Types<NodeId>::Array par = node.Parents();
    NumArray::Array par_values(par.size());
    for (Size i=0; i<par.size(); ++i)
    {
      NodeId id = par[i];
      DimArray * pdim = graph_.GetNode(id).DimPtr().get();
      ValArray * pval = NULL;
      // unobserved parents have null value
      if (graph_.GetObserved()[id])
        pval = graph_.GetValues()[id].get();
      par_values[i].SetPtr(pdim, pval);
    }

    // get bounds values
    NumArray::Pair bound_values;
    if (node.IsUpperBounded())
    {
      bound_values.second = par_values.back();
      par_values.pop_back();

      // check bound is observed
      if (!bound_values.second.ValuesPtr())
        throw LogicError("GetFixedSupportValuesVisitor::visit: node has unobserved upper bound.");
    }
    if (node.IsLowerBounded())
    {
      bound_values.first = par_values.back();
      par_values.pop_back();

      // check bound is observed
      if (!bound_values.first.ValuesPtr())
        throw LogicError("GetFixedSupportValuesVisitor::visit: node has unobserved upper bound.");
    }

    // get unbounded support limits
    node.FixedUnboundedSupport(lower_, upper_, par_values);

    // check there is a lower bound
    if (!bound_values.first.IsNULL())
    {
      // compare support with bounds and replace if necessary
      for (Size i = 0; i < lower_.size(); ++i)
      {
        if (lower_[i] < bound_values.first.Values()[i])
          lower_[i] = bound_values.first.Values()[i];
      }
    }
    // check there is an upper bound
    if (!bound_values.second.IsNULL())
    {
      // compare support with bounds and replace if necessary
      for (Size i = 0; i < upper_.size(); ++i)
      {
        if (upper_[i] > bound_values.second.Values()[i])
          upper_[i] = bound_values.second.Values()[i];
      }
    }
  }

  void getFixedSupportValues(ValArray & lower,
                        ValArray & upper,
                        NodeId nodeId,
                        const Graph & graph)
  {
    GetFixedSupportValuesVisitor get_support_val_vis(graph,
                                                lower,
                                                upper);
    graph.VisitNode(nodeId, get_support_val_vis);
  }

//  void getSupportValues(ValArray & lower,
//                        ValArray & upper,
//                        NodeId nodeId,
//                        const Graph & graph,
//                        const Types<Monitor*>::Array & paramMonitors,
//                        const Types<Monitor*>::Pair & boundMonitors,
//                        Size particleIndex)
//  {
//    NumArray::Array param_values = getParamValues(nodeId,
//                                                  graph,
//                                                  paramMonitors,
//                                                  particleIndex);
//    fixedUnboundedSupportVisitor unbound_support_vis(param_values, lower, upper);
//    graph.VisitNode(nodeId, unbound_support_vis);
//
//    IsBoundedVisitor is_bounded_vis;
//    graph.VisitNode(nodeId, is_bounded_vis);
//
//    if (is_bounded_vis.IsBounded())
//    {
//      NumArray::Pair bound_values = getBoundValues(nodeId,
//                                                   graph,
//                                                   boundMonitors,
//                                                   particleIndex);
//
//      if (!bound_values.first.IsNULL())
//      {
//        for (Size i = 0; i < lower.size(); ++i)
//        {
//          if (lower[i] < bound_values.first.Values()[i])
//            lower[i] = bound_values.first.Values()[i];
//        }
//      }
//      if (!bound_values.second.IsNULL())
//      {
//        for (Size i = 0; i < upper.size(); ++i)
//        {
//          if (upper[i] > bound_values.second.Values()[i])
//            upper[i] = bound_values.second.Values()[i];
//        }
//      }
//    }
//  }

  // -----------------------------------------------------------------
  // Is Support Fixed
  // -----------------------------------------------------------------

  class IsSupportFixedVisitor: public ConstNodeVisitor
  {
  protected:
    const Graph & graph_;
    Bool fixed_;

    virtual void visit(const StochasticNode & node);

  public:

    Bool IsSupportFixed() const
    {
      return fixed_;
    }

    explicit IsSupportFixedVisitor(const Graph & graph) :
      graph_(graph), fixed_(false)
    {
    }
  };

  void IsSupportFixedVisitor::visit(const StochasticNode & node)
  {
    GraphTypes::ParentIterator it_parent, it_parent_end;
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
    for (Size i = 0; it_parent != it_parent_end; ++it_parent, ++i)
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
