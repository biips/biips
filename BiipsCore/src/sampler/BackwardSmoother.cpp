//                                               -*- C++ -*-
/*! \file BackwardSmoother.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "sampler/BackwardSmoother.hpp"
#include "graph/StochasticNode.hpp"
#include "common/Utility.hpp"
#include "sampler/Accumulator.hpp"

namespace Biips
{

  BackwardSmoother::BackwardSmoother(const Graph::Ptr & pGraph, const Types<Monitor::Ptr>::Array & filterMonitors)
    : pGraph_(pGraph), filterMonitors_(filterMonitors), initialized_(false) {}


  void BackwardSmoother::Initialize()
  {
    Monitor & last_monitor = *filterMonitors_.back();
    t_ = last_monitor.GetTime();
    weights_.assign(last_monitor.GetWeights().begin(), last_monitor.GetWeights().end());
    sumOfweights_ = last_monitor.GetSumOfWeights();

    initialized_ = true;
  }


  void BackwardSmoother::IterateBack()
  {
    if (!initialized_)
      throw LogicError("Can not iterate BackwardSmoother: not initialized.");

    Monitor::Ptr p_last_monitor = filterMonitors_.back();
    filterMonitors_.pop_back();

    if (t_ == 0)
      throw LogicError("Can not iterate BackwardSmoother: the sequence has reached the end.");

    --t_;

    if (filterMonitors_.empty())
      throw LogicError("Can not iterate BackwardSmoother: there is no remaining iteration of filtering Monitor object.");

    Monitor & new_monitor = *(filterMonitors_.back());

    if (new_monitor.GetTime() != t_)
    {
      throw LogicError(String("Can not iterate BackwardSmoother: missing iteration ")
          + print(t_) + (" of filtering Monitor object."));
    }

    NodeId last_node_id = p_last_monitor->GetSampledNode();
    if (pGraph_->GetNode(last_node_id).GetType() != STOCHASTIC)
      throw LogicError("Can not iterate BackwardSmoother: last sampled node is not stochastic.");

    const StochasticNode & last_node = dynamic_cast<const StochasticNode &>(pGraph_->GetNode(last_node_id));

    Types<NodeId>::Array last_parents_ids = last_node.Parents();
    for (Size p=0; p<last_parents_ids.size(); ++p)
    {
      NodeId param_id = last_parents_ids[p];
      if (!pGraph_->GetObserved()[param_id])
      {
        if (!new_monitor.Contains(param_id))
          throw LogicError("Can not iterate BackwardSmoother: not all the parents of the last sampled node are monitored.");
      }
    }

    Size n_particles = weights_.size();
    ublas::matrix<Scalar> P_mat(n_particles, n_particles);

    MultiArray::Array param_values_i(last_parents_ids.size());
    NodeId param_id = NULL_NODEID;
    MultiArray last_particle_value_j;

    // Computing matrix P
    for (Size i=0; i<n_particles; ++i)
    {
      for (Size j=0; j<n_particles; ++j)
      {
        for (Size p=0; p<last_parents_ids.size(); ++p)
        {
          param_id = last_parents_ids[p];
          // TODO: improve access to node values
          if (pGraph_->GetObserved()[param_id])
            param_values_i[p].SetPtr(pGraph_->GetNode(param_id).DimPtr(), pGraph_->GetValues()[param_id]);
          else
            param_values_i[p].SetPtr(pGraph_->GetNode(param_id).DimPtr(), new_monitor.GetNodeValues(param_id)[i]);
        }

        last_particle_value_j.SetPtr(last_node.DimPtr(), p_last_monitor->GetNodeValues(last_node_id)[j]);

        P_mat(i,j) = exp(last_node.LogPriorPdf(last_particle_value_j, param_values_i));
      }
    }

    // Updating weights
    Vector weights_filter_vec(n_particles, Vector::array_type());
    weights_filter_vec.data().swap(new_monitor.Weights());

    Vector weights_vec = ublas::prod(weights_filter_vec, P_mat);
    for (Size i=0; i<n_particles; ++i)
      weights_[i] /= weights_vec[i];

    weights_vec.data().clear();
    weights_vec.data().swap(weights_);

    weights_vec = ublas::prod(P_mat, weights_vec);
    weights_vec.data().swap(weights_);

    for (Size i=0; i<n_particles; ++i)
      weights_[i] *= weights_filter_vec[i];

    weights_filter_vec.data().swap(new_monitor.Weights());
  }


  void BackwardSmoother::Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n) const
  {
    const Monitor & last_monitor = *filterMonitors_.back();
    if (!last_monitor.Contains(nodeId))
      throw LogicError(String("Can not accumulate BackwardSmoother: node " + print(nodeId) + " is not monitored at the current iteration."));

    featuresAcc.Init();
    for(Size i=0; i < last_monitor.NParticles(); i++)
      featuresAcc.Push((*(last_monitor.GetNodeValues(nodeId)[i]))[n], weights_[i]);
  }


  void BackwardSmoother::Accumulate(NodeId nodeId, DiscreteScalarAccumulator & featuresAcc, Size n) const
  {
    const Monitor & last_monitor = *filterMonitors_.back();
    if (!last_monitor.Contains(nodeId))
      throw LogicError(String("Can not accumulate BackwardSmoother: node " + print(nodeId) + " is not monitored at the current iteration."));

    featuresAcc.Init();
    for(Size i=0; i < last_monitor.NParticles(); i++)
      featuresAcc.Push((*(last_monitor.GetNodeValues(nodeId)[i]))[n], weights_[i]);
  }


  void BackwardSmoother::Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc) const
  {
    const Monitor & last_monitor = *filterMonitors_.back();
    if (!last_monitor.Contains(nodeId))
      throw LogicError(String("Can not accumulate BackwardSmoother: node " + print(nodeId) + " is not monitored at the current iteration."));

    featuresAcc.Init(pGraph_->GetNode(nodeId).DimPtr());
    for(Size i=0; i < last_monitor.NParticles(); i++)
      featuresAcc.Push(last_monitor.GetNodeValues(nodeId)[i], weights_[i]);
  }


  void BackwardSmoother::SetMonitorWeights(Monitor & monitor) const
  {
    monitor.Weights().assign(weights_.begin(), weights_.end());
  }


  void BackwardSmoother::SetMonitorNodeValues(NodeId nodeId, Monitor & monitor) const
  {
    Monitor & last_monitor = *filterMonitors_.back();
    monitor.NodeValues(nodeId).assign(last_monitor.GetNodeValues(nodeId).begin(), last_monitor.GetNodeValues(nodeId).end());
  }

}
