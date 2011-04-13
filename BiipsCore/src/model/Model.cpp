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
    for (std::map<NodeId, Monitor::Ptr>::iterator  it = filterMonitorsMap_.begin();
        it != filterMonitorsMap_.end(); ++it)
    {
      it->second->Clear();
    }
  }


  void Model::SetResampleParam(ResampleType rtMode, Scalar threshold)
  {
    if (!pSampler_)
      throw LogicError("Can not set resampling parameters: no SMCSampler.");

    pSampler_->SetResampleParams(rtMode, threshold);
  }


  Monitor::Ptr Model::SetFilterMonitor(const Types<NodeId>::Array & nodeIds)
  {
    Monitor::Ptr p_monitor(new Monitor());

    for (Size i=0; i<nodeIds.size(); ++i)
    {
      if (filterMonitorsMap_.count(nodeIds[i]))
        throw LogicError("Node has already a filter monitor.");

      filterMonitorsMap_.insert(std::make_pair(nodeIds[i], p_monitor));
    }
    return p_monitor;
  }

  void Model::IterateSampler()
  {
    if (!pSampler_)
      throw LogicError("Can not iterate a Null SMCSampler.");

    const Types<NodeId>::Array & sampled_nodes = SMCSampler::NextSampledNodes();

    pSampler_->Iterate();

    // Filter Monitors
    NodeId node_id = NULL_NODEID;
    Monitor::Ptr p_monitor;
    for (Size i=0; i<sampled_nodes.size(); ++i)
    {
      node_id = sampled_nodes[i];
      if(filterMonitorsMap_.count(node_id))
      {
        p_monitor = filterMonitorsMap_.at(node_id);
        p_monitor->AddNode(node_id, pSampler_->NParticles(), pSampler_->Ess());
        pSampler_->MonitorNode(node_id, *p_monitor);
      }
    }
  }


  MultiArray Model::ExtractFilterStat(NodeId nodeId, StatsTag statFeature) const
  {
    if (!pSampler_)
      throw LogicError("Can not extract filter statistic: no SMCSampler.");


    // Search the filter Monitor object responsible of the nodeId
    std::map<NodeId, Monitor::Ptr>::const_iterator it = filterMonitorsMap_.begin();
    for (; it != filterMonitorsMap_.end(); ++it)
    {
      if (it->second->Contains(nodeId))
        break;
    }

    if (it == filterMonitorsMap_.end())
      throw LogicError("Node is not yet monitored.");

    ElementAccumulator elem_acc;
    elem_acc.AddFeature(statFeature);

    switch (statFeature)
    {
      case MIN: case MAX: case PDF: case QUANTILES: case CDF: case MAX_PDF:
        throw LogicError("Can not extract statistic in Model::ExtractFilterStat.");
        break;
      default:
        it->second->Accumulate(nodeId, elem_acc, pGraph_->GetNode(nodeId).DimPtr());
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


  // TODO traiter le cas des variables dicretes
  ScalarHistogram Model::ExtractFilterPdf(NodeId nodeId, Size numBins, Scalar cacheFraction) const
  {
    if (!pSampler_)
      throw LogicError("Can not extract filter pdf: no SMCSampler.");

    if (!pGraph_->GetNode(nodeId).Dim().IsScalar())
      throw LogicError("Can not extract filter pdf: node is not scalar.");


    // Search the filter Monitor object responsible of the nodeId
    std::map<NodeId, Monitor::Ptr>::const_iterator it = filterMonitorsMap_.begin();
    for (; it != filterMonitorsMap_.end(); ++it)
    {
      if (it->second->Contains(nodeId))
        break;
    }

    if (it == filterMonitorsMap_.end())
      throw LogicError("Node is not yet monitored.");

    ScalarAccumulator scalar_acc;
    scalar_acc.AddFeature(PDF);
    scalar_acc.SetPdfParam(roundSize(pSampler_->NParticles()*cacheFraction), numBins);

    it->second->Accumulate(nodeId, scalar_acc);

    return scalar_acc.Pdf();
  }
}
