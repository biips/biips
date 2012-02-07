//                                               -*- C++ -*-
/*! \file Model.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "model/Model.hpp"
#include "model/Monitor.hpp"
#include "sampler/Accumulator.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "graph/StochasticNode.hpp"

namespace Biips
{

  void Model::SetDefaultFilterMonitors()
  {
    for (NodeId id = 0; id < pGraph_->GetSize(); ++id)
    {
      if (pGraph_->GetNode(id).GetType() != STOCHASTIC)
        continue;

      // FIXME: Never add observed nodes, they won't be monitored !!
      if (pGraph_->GetObserved()[id])
        continue;

      // Monitor all unobserved stochastic nodes
      filterMonitorsMap_[id];

      // and its unobserved direct parents
      GraphTypes::ParentIterator it_parents, it_parents_end;
      boost::tie(it_parents, it_parents_end) = pGraph_->GetParents(id);
      for (; it_parents != it_parents_end; ++it_parents)
      {
        // FIXME: Never add observed nodes, they won't be monitored !!
        if (pGraph_->GetObserved()[*it_parents])
          continue;

        filterMonitorsMap_[*it_parents];
      }
    }

    defaultMonitorsSet_ = true;
  }

  Bool Model::SetFilterMonitor(NodeId nodeId)
  {
    // FIXME: it is no use monitoring observed nodes
    if (pGraph_->GetObserved()[nodeId])
      return false;

    filterMonitorsMap_[nodeId];
    return true;
  }

  Bool Model::SetSmoothMonitor(NodeId nodeId)
  {
    SetFilterMonitor(nodeId);

    // FIXME: it is no use monitoring observed nodes
    if (pGraph_->GetObserved()[nodeId])
      return false;

    smoothMonitorsMap_[nodeId];
    return true;
  }

  Bool Model::SetSmoothTreeMonitor(NodeId nodeId)
  {
    // FIXME: it is no use monitoring observed nodes
    if (pGraph_->GetObserved()[nodeId])
      return false;

    smoothTreeMonitoredNodeIds_.insert(nodeId);
    return true;
  }

  void Model::ReleaseFilterMonitors()
  {
    filterMonitors_.clear();
    for (std::map<NodeId, Monitor::Ptr>::iterator it_monitors =
        filterMonitorsMap_.begin(); it_monitors != filterMonitorsMap_.end(); ++it_monitors)
    {
      it_monitors->second.reset();
    }
  }

  void Model::ReleaseSmoothTreeMonitors()
  {
    pSmoothTreeMonitor_.reset();
  }

  void Model::ReleaseSmoothMonitors()
  {
    smoothMonitors_.clear();
    for (std::map<NodeId, Monitor::Ptr>::iterator it_monitors =
        smoothMonitorsMap_.begin(); it_monitors != smoothMonitorsMap_.end(); ++it_monitors)
    {
      it_monitors->second.reset();
    }
  }

  void Model::ClearFilterMonitors()
  {
    filterMonitors_.clear();
    filterMonitorsMap_.clear();
  }

  void Model::ClearSmoothTreeMonitors()
  {
    pSmoothTreeMonitor_.reset();
    smoothTreeMonitoredNodeIds_.clear();
  }

  void Model::ClearSmoothMonitors()
  {
    smoothMonitors_.clear();
    smoothMonitorsMap_.clear();
  }

  const ForwardSampler & Model::Sampler() const
  {
    if (!pSampler_)
      throw LogicError("Can not acces a Null ForwardSampler.");

    return *pSampler_;
  }

  const BackwardSmoother & Model::Smoother() const
  {
    if (!pSmoother_)
      throw LogicError("Can not acces a Null BackwardSmoother.");

    return *pSmoother_;
  }

  void Model::BuildSampler()
  {
    pSampler_ = ForwardSampler::Ptr(new ForwardSampler(*pGraph_));

    pSampler_->Build();
  }

  void Model::InitSampler(Size nParticles,
                          const Rng::Ptr & pRng,
                          const String & rsType,
                          Scalar threshold)
  {
    // release monitors
    ReleaseFilterMonitors();
    ReleaseSmoothTreeMonitors();
    ReleaseSmoothMonitors();

    pSampler_->Initialize(nParticles, pRng, rsType, threshold);

    // lock SmoothTree monitored nodes
    for (std::set<NodeId>::const_iterator it_nodes =
        smoothTreeMonitoredNodeIds_.begin(); it_nodes
        != smoothTreeMonitoredNodeIds_.end(); ++it_nodes)
      pSampler_->LockNode(*it_nodes);

    Size t = pSampler_->Iteration();
    const Types<NodeId>::Array & sampled_nodes = pSampler_->SampledNodes();

    // Filter Monitors
    NodeId node_id = NULL_NODEID;
    FilterMonitor::Ptr p_monitor(new FilterMonitor(t, sampled_nodes)); // FIXME Do we create monitor object even if no nodes are monitored ?
    pSampler_->InitMonitor(*p_monitor);
    for (Size i = 0; i < sampled_nodes.size(); ++i)
    {
      node_id = sampled_nodes[i];
      if (filterMonitorsMap_.count(node_id))
      {
        pSampler_->MonitorNode(node_id, *p_monitor);
        filterMonitorsMap_[node_id] = p_monitor;
      }
    }

    filterMonitors_.push_back(p_monitor);

    if (!pSampler_->AtEnd())
    {
      // release memory
      pSampler_->ReleaseNodes();
      return;
    }

    // Smooth tree Monitors
    p_monitor = FilterMonitor::Ptr(new FilterMonitor(t, sampled_nodes)); // FIXME Do we create monitor object even if no nodes are monitored ?
    pSampler_->InitMonitor(*p_monitor);
    for (std::set<NodeId>::const_iterator it_ids =
        smoothTreeMonitoredNodeIds_.begin(); it_ids
        != smoothTreeMonitoredNodeIds_.end(); ++it_ids)
    {
      pSampler_->MonitorNode(*it_ids, *p_monitor);
      pSampler_->UnlockNode(*it_ids);
    }
    pSmoothTreeMonitor_ = p_monitor;

    // release memory
    pSampler_->UnlockAllNodes();
    pSampler_->ReleaseNodes();
  }

  void Model::IterateSampler()
  {
    if (!pSampler_)
      throw LogicError("Can not iterate a Null ForwardSampler.");

    pSampler_->Iterate();

    Size t = pSampler_->Iteration();
    const Types<NodeId>::Array & sampled_nodes = pSampler_->SampledNodes();

    // Filter Monitors
    NodeId node_id = NULL_NODEID;
    FilterMonitor::Ptr p_monitor(new FilterMonitor(t, sampled_nodes)); // FIXME Do we create monitor object even if no nodes are monitored ?
    pSampler_->InitMonitor(*p_monitor);
    for (Size i = 0; i < sampled_nodes.size(); ++i)
    {
      node_id = sampled_nodes[i];
      if (filterMonitorsMap_.count(node_id))
      {
        pSampler_->MonitorNode(node_id, *p_monitor);
        filterMonitorsMap_[node_id] = p_monitor;
      }
    }

    filterMonitors_.push_back(p_monitor);

    if (!pSampler_->AtEnd())
    {
      // release memory
      pSampler_->ReleaseNodes();
      return;
    }

    // Smooth tree Monitors
    p_monitor = FilterMonitor::Ptr(new FilterMonitor(t, sampled_nodes)); // FIXME Do we create monitor object even if no nodes are monitored ?
    pSampler_->InitMonitor(*p_monitor);
    for (std::set<NodeId>::const_iterator it_ids =
        smoothTreeMonitoredNodeIds_.begin(); it_ids
        != smoothTreeMonitoredNodeIds_.end(); ++it_ids)
    {
      pSampler_->MonitorNode(*it_ids, *p_monitor);
      pSampler_->UnlockNode(*it_ids);
    }
    pSmoothTreeMonitor_ = p_monitor;

    // release memory
    pSampler_->UnlockAllNodes();
    pSampler_->ReleaseNodes();
  }

  void Model::InitBackwardSmoother()
  {
    // release monitors
    ReleaseSmoothMonitors();

    if (!defaultMonitorsSet_)
      throw LogicError("Can not initiate backward smoother: default monitors not set.");

    if (!pSampler_)
      throw LogicError("Can not initiate backward smoother: no ForwardSampler.");

    pSmoother_
        = BackwardSmoother::Ptr(new BackwardSmoother(*pGraph_,
                                                     filterMonitors_,
                                                     pSampler_->GetNodeSamplingIterations()));

    pSmoother_->Initialize();

    Types<NodeId>::Array updated_nodes = pSmoother_->UpdatedNodes();
    Size t = pSmoother_->Iteration();

    // Smooth Monitors
    NodeId node_id = NULL_NODEID;
    SmoothMonitor::Ptr p_monitor(new SmoothMonitor(t, updated_nodes)); // FIXME Do we create monitor object even if no nodes are monitored ?
    pSmoother_->InitMonitor(*p_monitor);
    for (Size i = 0; i < updated_nodes.size(); ++i)
    {
      node_id = updated_nodes[i];
      if (smoothMonitorsMap_.count(node_id))
      {
        pSmoother_->MonitorNode(node_id, *p_monitor);
        smoothMonitorsMap_[node_id] = p_monitor;
      }
    }

    smoothMonitors_.push_back(p_monitor);
  }

  void Model::IterateBackwardSmoother()
  {
    if (!pSmoother_)
      throw LogicError("Can not iterate backward smoother: not initialized.");

    pSmoother_->IterateBack();

    Types<NodeId>::Array updated_nodes = pSmoother_->UpdatedNodes();
    Size t = pSmoother_->Iteration();

    // Smooth Monitors
    NodeId node_id = NULL_NODEID;
    SmoothMonitor::Ptr p_monitor(new SmoothMonitor(t, updated_nodes)); // FIXME Do we create monitor object even if no nodes are monitored ?
    pSmoother_->InitMonitor(*p_monitor);
    for (Size i = 0; i < updated_nodes.size(); ++i)
    {
      node_id = updated_nodes[i];
      if (smoothMonitorsMap_.count(node_id))
      {
        pSmoother_->MonitorNode(node_id, *p_monitor);
        smoothMonitorsMap_[node_id] = p_monitor;
      }
    }

    smoothMonitors_.push_back(p_monitor);
  }

  MultiArray Model::extractMonitorStat(NodeId nodeId,
                                       StatsTag statFeature,
                                       const std::map<NodeId, Monitor::Ptr> & monitorsMap) const
  {
    if (monitorsMap.find(nodeId) == monitorsMap.end())
      throw LogicError("Node is not yet monitored.");

    ElementAccumulator elem_acc;
    elem_acc.AddFeature(statFeature);

    switch (statFeature)
    {
      case MIN:
      case MAX:
      case PDF:
      case QUANTILES:
      case CDF:
      case MODE:
        throw LogicError("Can not extract statistic in Model::extractMonitorStat.");
        break;
      default:
        monitorsMap.at(nodeId)->Accumulate(nodeId,
                                           elem_acc,
                                           pGraph_->GetNode(nodeId).DimPtr());
        break;
    }

    MultiArray stat_marray;

    switch (statFeature)
    {
      case COUNT:
      {
        ValArray::Ptr p_val(new ValArray(1, elem_acc.Count()));
        stat_marray.SetPtr(P_SCALAR_DIM, p_val);
        break;
      }
      case SUM_OF_WEIGHTS:
      {
        ValArray::Ptr p_val(new ValArray(1, elem_acc.SumOfWeights()));
        stat_marray.SetPtr(P_SCALAR_DIM, p_val);
        break;
      }
      case SUM:
        stat_marray = elem_acc.Sum();
        break;
      case MEAN:
        stat_marray = elem_acc.Mean();
        break;
      case VARIANCE:
        stat_marray = elem_acc.Variance();
        break;
      case MOMENT2:
        stat_marray = elem_acc.Moment<2> ();
        break;
      case MOMENT3:
        stat_marray = elem_acc.Moment<3> ();
        break;
      case MOMENT4:
        stat_marray = elem_acc.Moment<4> ();
        break;
      case MOMENT5:
        stat_marray = elem_acc.Moment<5> ();
        break;
      case SKEWNESS:
        stat_marray = elem_acc.Skewness();
        break;
      case KURTOSIS:
        stat_marray = elem_acc.Kurtosis();
        break;
      default:
        break;
    }

    return stat_marray;
  }

  MultiArray Model::ExtractFilterStat(NodeId nodeId, StatsTag statFeature) const
  {
    if (!pSampler_)
      throw LogicError("Can not extract filter statistic: no ForwardSampler.");

    return extractMonitorStat(nodeId, statFeature, filterMonitorsMap_);
  }

  MultiArray Model::ExtractSmoothStat(NodeId nodeId, StatsTag statFeature) const
  {
    if (!pSampler_)
      throw LogicError("Can not extract backward smoother statistic: no ForwardSampler.");

    return extractMonitorStat(nodeId, statFeature, smoothMonitorsMap_);
  }

  // TODO manage discrete variable cases
  ScalarHistogram Model::extractMonitorPdf(NodeId nodeId,
                                           Size numBins,
                                           Scalar cacheFraction,
                                           const std::map<NodeId, Monitor::Ptr> & monitorsMap) const
  {
    if (!pSampler_)
      throw LogicError("Can not extract filter pdf: no ForwardSampler.");

    if (!pGraph_->GetNode(nodeId).Dim().IsScalar())
      throw LogicError("Can not extract filter pdf: node is not scalar.");

    if (monitorsMap.find(nodeId) == monitorsMap.end())
      throw LogicError("Node is not yet monitored.");

    ScalarAccumulator scalar_acc;
    scalar_acc.AddFeature(PDF);
    scalar_acc.SetPdfParam(roundSize(pSampler_->NParticles() * cacheFraction),
                           numBins);

    monitorsMap.at(nodeId)->Accumulate(nodeId, scalar_acc);

    return scalar_acc.Pdf();
  }

  // TODO manage discrete variable cases
  ScalarHistogram Model::ExtractFilterPdf(NodeId nodeId,
                                          Size numBins,
                                          Scalar cacheFraction) const
  {
    if (!pSampler_)
      throw LogicError("Can not extract filter pdf: no ForwardSampler.");

    return extractMonitorPdf(nodeId, numBins, cacheFraction, filterMonitorsMap_);
  }

  // TODO manage discrete variable cases
  ScalarHistogram Model::ExtractSmoothPdf(NodeId nodeId,
                                          Size numBins,
                                          Scalar cacheFraction) const
  {
    if (!pSampler_)
      throw LogicError("Can not extract backward smooth pdf: no ForwardSampler.");

    return extractMonitorPdf(nodeId, numBins, cacheFraction, smoothMonitorsMap_);
  }

  // FIXME Still valid after optimization ?
  MultiArray Model::ExtractSmoothTreeStat(NodeId nodeId, StatsTag statFeature) const
  {
    if (!pSampler_)
      throw LogicError("Can not extract smooth tree statistic: no ForwardSampler.");

    ElementAccumulator elem_acc;
    elem_acc.AddFeature(statFeature);

    switch (statFeature)
    {
      case MIN:
      case MAX:
      case PDF:
      case QUANTILES:
      case CDF:
      case MODE:
        throw LogicError("Can not extract statistic in Model::ExtractSmoothTreeStat.");
        break;
      default:
        pSampler_->Accumulate(nodeId, elem_acc);
        break;
    }

    MultiArray stat_marray;

    switch (statFeature)
    {
      case COUNT:
      {
        ValArray::Ptr p_val(new ValArray(1, elem_acc.Count()));
        stat_marray.SetPtr(P_SCALAR_DIM, p_val);
        break;
      }
      case SUM_OF_WEIGHTS:
      {
        ValArray::Ptr p_val(new ValArray(1, elem_acc.SumOfWeights()));
        stat_marray.SetPtr(P_SCALAR_DIM, p_val);
        break;
      }
      case SUM:
        stat_marray = elem_acc.Sum();
        break;
      case MEAN:
        stat_marray = elem_acc.Mean();
        break;
      case VARIANCE:
        stat_marray = elem_acc.Variance();
        break;
      case MOMENT2:
        stat_marray = elem_acc.Moment<2> ();
        break;
      case MOMENT3:
        stat_marray = elem_acc.Moment<3> ();
        break;
      case MOMENT4:
        stat_marray = elem_acc.Moment<4> ();
        break;
      case MOMENT5:
        stat_marray = elem_acc.Moment<5> ();
        break;
      case SKEWNESS:
        stat_marray = elem_acc.Skewness();
        break;
      case KURTOSIS:
        stat_marray = elem_acc.Kurtosis();
        break;
      default:
        break;
    }

    return stat_marray;
  }

  // TODO manage dicrete variable cases
  // FIXME Still valid after optimization ?
  ScalarHistogram Model::ExtractSmoothTreePdf(NodeId nodeId,
                                              Size numBins,
                                              Scalar cacheFraction) const
  {
    if (!pSampler_)
      throw LogicError("Can not extract smooth tree pdf: no ForwardSampler.");

    if (!pGraph_->GetNode(nodeId).Dim().IsScalar())
      throw LogicError("Can not extract smooth tree pdf: node is not scalar.");

    ScalarAccumulator scalar_acc;
    scalar_acc.AddFeature(PDF);
    scalar_acc.SetPdfParam(roundSize(pSampler_->NParticles() * cacheFraction),
                           numBins);

    pSampler_->Accumulate(nodeId, scalar_acc);

    return scalar_acc.Pdf();
  }

  class LogPriorDensityVisitor: public ConstNodeVisitor
  {
  protected:
    const Graph & graph_;
    Scalar prior_;
  public:
    virtual void visit(const StochasticNode & node)
    {
      NumArray x(node.DimPtr().get(), graph_.GetValues()[nodeId_].get());
      NumArray::Array parents(node.Parents().size());
      for (Size i = 0; i < node.Parents().size(); ++i)
      {
        NodeId par_id = node.Parents()[i];
        parents[i].SetPtr(graph_.GetNode(par_id).DimPtr().get(),
                          graph_.GetValues()[par_id].get());
      }
      NumArray::Pair bounds;
      if (node.PriorPtr()->CanBound())
      {
        if (node.IsLowerBounded())
          bounds.first.SetPtr(graph_.GetNode(node.Lower()).DimPtr().get(),
                              graph_.GetValues()[node.Lower()].get());

        if (node.IsUpperBounded())
          bounds.second.SetPtr(graph_.GetNode(node.Upper()).DimPtr().get(),
                             graph_.GetValues()[node.Upper()].get());
      }

      prior_ = node.LogPriorDensity(x, parents, bounds);
    }

    Scalar GetPrior() const
    {
      return prior_;
    }

    explicit LogPriorDensityVisitor(const Graph & graph) :
      graph_(graph)
    {
    }
  };

  Scalar Model::GetLogPriorDensity(NodeId nodeId) const
  {
    if (pGraph_->GetNode(nodeId).GetType() != STOCHASTIC
        && !pGraph_->GetObserved()[nodeId])
      throw RuntimeError("Can not get prior density: node is not observed stochastic.");

    GraphTypes::ParentIterator it_parent, it_parent_end;
    boost::tie(it_parent, it_parent_end) = pGraph_->GetParents(nodeId);
    for (; it_parent != it_parent_end; ++it_parent)
    {
      if (!pGraph_->GetObserved()[*it_parent])
        throw RuntimeError("Can not get prior density: node has unobserved parents.");
    }

    LogPriorDensityVisitor log_prior_vis(*pGraph_);
    pGraph_->VisitNode(nodeId, log_prior_vis);

    return log_prior_vis.GetPrior();
  }
}
