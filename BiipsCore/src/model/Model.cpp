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

namespace Biips
{

  void Model::SetResampleParam(ResampleType rtMode, Scalar threshold)
  {
    if (!pSampler_)
      throw LogicError("Can not set resampling parameters: no SMCSampler.");

    pSampler_->SetResampleParams(rtMode, threshold);
  }


  void Model::SetDefaultFilterMonitors()
  {
    for (NodeId id=0; id<pGraph_->GetSize(); ++id)
    {
      if (pGraph_->GetNode(id).GetType() == STOCHASTIC)
      {
        if (!pGraph_->GetObserved()[id])
        {
          // Monitor all unobserved stochastic nodes
          filterMonitorsMap_[id];

          // and its direct parents
          GraphTypes::DirectParentNodeIdIterator it_parents, it_parents_end;
          boost::tie(it_parents, it_parents_end) = pGraph_->GetParents(id);
          for (; it_parents != it_parents_end; ++it_parents)
            filterMonitorsMap_[*it_parents];
        }
      }
    }

    defaultMonitorsSet_ = true;
  }


  void Model::SetFilterMonitor(const Types<NodeId>::Array & nodeIds)
  {
    for (Size i=0; i<nodeIds.size(); ++i)
      filterMonitorsMap_[nodeIds[i]];
  }


  const SMCSampler & Model::Sampler() const
  {
    if (!pSampler_)
      throw LogicError("Can not acces a Null SMCSampler.");

    return *pSampler_;
  }


  void Model::InitSampler(Size nParticles, Rng::Ptr pRng)
  {
    pRng_ = pRng;

    pSampler_ = SMCSampler::Ptr(new SMCSampler(nParticles, pGraph_.get(), pRng_.get()));

    pSampler_->Initialize();

    // Clear Filter Monitors
    filterMonitors_.clear();
    for (std::map<NodeId, Monitor::Ptr>::iterator  it = filterMonitorsMap_.begin();
        it != filterMonitorsMap_.end(); ++it)
    {
      it->second = Monitor::Ptr();
    }
  }


  void Model::IterateSampler()
  {
    if (!pSampler_)
      throw LogicError("Can not iterate a Null SMCSampler.");

    Size t = pSampler_->Time();
    const Types<NodeId>::Array & sampled_nodes = pSampler_->NextSampledNodes();

    pSampler_->Iterate();

    // Filter Monitors
    NodeId node_id = NULL_NODEID;
    Monitor::Ptr p_monitor(new Monitor(t, sampled_nodes.front())); // FIXME Do we create monitor object even if no nodes are monitored ?
    pSampler_->SetMonitorWeights(*p_monitor);
    for (Size i=0; i<sampled_nodes.size(); ++i)
    {
      node_id = sampled_nodes[i];
      if(filterMonitorsMap_.count(node_id))
      {
        pSampler_->SetMonitorNodeValues(node_id, *p_monitor);
        filterMonitorsMap_[node_id] = p_monitor;
      }
    }

    filterMonitors_.push_back(p_monitor);
  }


  void Model::InitBackwardSmoother()
  {
    if (!defaultMonitorsSet_)
      throw LogicError("Can not initiate backward smoother: default monitors not set.");

    pSmoother_ = BackwardSmoother::Ptr(new BackwardSmoother(pGraph_, filterMonitors_));

    pSmoother_->Initialize();
  }


  void Model::IterateBackwardSmoother()
  {
    if (!pSmoother_)
      throw LogicError("Can not iterate backward smoother: not initialized.");

    pSmoother_->IterateBack();
  }


  MultiArray Model::ExtractFilterStat(NodeId nodeId, StatsTag statFeature) const
  {
    if (!pSampler_)
      throw LogicError("Can not extract filter statistic: no SMCSampler.");

//    // FIXME Search the first filter Monitor object responsible of the nodeId
//    Types<Monitor::Ptr>::ConstIterator it = filterMonitors_.begin();
//    for (; it != filterMonitors_.end(); ++it)
//    {
//      if ((*it)->Contains(nodeId))
//        break;
//    }
//
//    if (it == filterMonitors_.end())
//      throw LogicError("Node is not yet monitored.");

    if (filterMonitorsMap_.find(nodeId) == filterMonitorsMap_.end())
      throw LogicError("Node is not yet monitored.");

    ElementAccumulator elem_acc;
    elem_acc.AddFeature(statFeature);

    switch (statFeature)
    {
      case MIN: case MAX: case PDF: case QUANTILES: case CDF: case MAX_PDF:
        throw LogicError("Can not extract statistic in Model::ExtractFilterStat.");
        break;
      default:
        filterMonitorsMap_.at(nodeId)->Accumulate(nodeId, elem_acc, pGraph_->GetNode(nodeId).DimPtr());
        break;
    }

    MultiArray stat_marray;

    switch (statFeature)
    {
      case COUNT:
        stat_marray.Alloc(Scalar(elem_acc.Count()));
        break;
      case SUM_OF_WEIGHTS:
        stat_marray.Alloc(elem_acc.SumOfWeights());
        break;
      case SUM:
        stat_marray.SetPtr(elem_acc.Sum());
        break;
      case MEAN:
        stat_marray.SetPtr(elem_acc.Mean());
        break;
      case VARIANCE:
        stat_marray.SetPtr(elem_acc.Variance());
        break;
      case MOMENT2:
        stat_marray.SetPtr(elem_acc.Moment<2>());
        break;
      case MOMENT3:
        stat_marray.SetPtr(elem_acc.Moment<3>());
        break;
      case MOMENT4:
        stat_marray.SetPtr(elem_acc.Moment<4>());
        break;
      case MOMENT5:
        stat_marray.SetPtr(elem_acc.Moment<5>());
        break;
      case SKEWNESS:
        stat_marray.SetPtr(elem_acc.Skewness());
        break;
      case KURTOSIS:
        stat_marray.SetPtr(elem_acc.Kurtosis());
        break;
      default:
        break;
    }

    return stat_marray;
  }


  // FIXME Still valid after optimization ?
  MultiArray Model::ExtractSmoothTreeStat(NodeId nodeId, StatsTag statFeature) const
  {
    if (!pSampler_)
      throw LogicError("Can not extract smooth tree statistic: no SMCSampler.");

    ElementAccumulator elem_acc;
    elem_acc.AddFeature(statFeature);

    switch (statFeature)
    {
      case MIN: case MAX: case PDF: case QUANTILES: case CDF: case MAX_PDF:
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
        stat_marray.Alloc(Scalar(elem_acc.Count()));
        break;
      case SUM_OF_WEIGHTS:
        stat_marray.Alloc(elem_acc.SumOfWeights());
        break;
      case SUM:
        stat_marray.SetPtr(elem_acc.Sum());
        break;
      case MEAN:
        stat_marray.SetPtr(elem_acc.Mean());
        break;
      case VARIANCE:
        stat_marray.SetPtr(elem_acc.Variance());
        break;
      case MOMENT2:
        stat_marray.SetPtr(elem_acc.Moment<2>());
        break;
      case MOMENT3:
        stat_marray.SetPtr(elem_acc.Moment<3>());
        break;
      case MOMENT4:
        stat_marray.SetPtr(elem_acc.Moment<4>());
        break;
      case MOMENT5:
        stat_marray.SetPtr(elem_acc.Moment<5>());
        break;
      case SKEWNESS:
        stat_marray.SetPtr(elem_acc.Skewness());
        break;
      case KURTOSIS:
        stat_marray.SetPtr(elem_acc.Kurtosis());
        break;
      default:
        break;
    }

    return stat_marray;
  }


  // TODO manage dicrete variable cases
  ScalarHistogram Model::ExtractFilterPdf(NodeId nodeId, Size numBins, Scalar cacheFraction) const
  {
    if (!pSampler_)
      throw LogicError("Can not extract filter pdf: no SMCSampler.");

    if (!pGraph_->GetNode(nodeId).Dim().IsScalar())
      throw LogicError("Can not extract filter pdf: node is not scalar.");

//    // FIXME Search the first filter Monitor object responsible of the nodeId
//    Types<Monitor::Ptr>::ConstIterator it = filterMonitors_.begin();
//    for (; it != filterMonitors_.end(); ++it)
//    {
//      if ((*it)->Contains(nodeId))
//        break;
//    }
//
//    if (it == filterMonitors_.end())
//      throw LogicError("Node is not yet monitored.");

    if (filterMonitorsMap_.find(nodeId) == filterMonitorsMap_.end())
      throw LogicError("Node is not yet monitored.");

    ScalarAccumulator scalar_acc;
    scalar_acc.AddFeature(PDF);
    scalar_acc.SetPdfParam(roundSize(pSampler_->NParticles()*cacheFraction), numBins);

    filterMonitorsMap_.at(nodeId)->Accumulate(nodeId, scalar_acc);

    return scalar_acc.Pdf();
  }


  // TODO manage dicrete variable cases
  // FIXME Still valid after optimization ?
  ScalarHistogram Model::ExtractSmoothTreePdf(NodeId nodeId, Size numBins, Scalar cacheFraction) const
  {
    if (!pSampler_)
      throw LogicError("Can not extract smooth tree pdf: no SMCSampler.");

    if (!pGraph_->GetNode(nodeId).Dim().IsScalar())
      throw LogicError("Can not extract smooth tree pdf: node is not scalar.");


    ScalarAccumulator scalar_acc;
    scalar_acc.AddFeature(PDF);
    scalar_acc.SetPdfParam(roundSize(pSampler_->NParticles()*cacheFraction), numBins);

    pSampler_->Accumulate(nodeId, scalar_acc);

    return scalar_acc.Pdf();
  }
}
