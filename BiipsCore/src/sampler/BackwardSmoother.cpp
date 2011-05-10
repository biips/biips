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

    Monitor & last_monitor = *filterMonitors_.back();
    if (t_ == 0)
      throw LogicError("Can not iterate BackwardSmoother: the sequence has reached the end.");

    --t_;

    if (filterMonitors_.size() < 2)
      throw LogicError("Can not iterate BackwardSmoother: there is no remaining iteration of filtering Monitor object.");

    Monitor & new_monitor = **(filterMonitors_.end()-2);

    if (new_monitor.GetTime() != t_)
    {
      throw LogicError(String("Can not iterate BackwardSmoother: missing iteration ")
          + print(t_) + (" of filtering Monitor object."));
    }

    NodeId last_node_id = last_monitor.GetSampledNode();
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
    MultiArray::Array param_values_i(last_parents_ids.size());

    Size n_particles = weights_.size();
    ublas::matrix<Scalar> P_mat(n_particles, n_particles);

    for (Size i=0; i<n_particles; ++i)
    {
      for (Size j=0; j<n_particles; ++j)
      {
        for (Size p=0; p<last_parents_ids.size(); ++p)
        {
          NodeId param_id = last_parents_ids[p];
          const DimArray::Ptr & param_dim = pGraph_->GetNode(param_id).DimPtr();
          // TODO: improve access to node values
          if (pGraph_->GetObserved()[param_id])
            param_values_i[p].SetPtr(param_dim, pGraph_->GetValues()[param_id]);
          else
            param_values_i[p].SetPtr(param_dim, new_monitor.GetNodeValues(param_id)[i]);
        }

        MultiArray last_particle_value_j(last_node.DimPtr(), last_monitor.GetNodeValues(last_node_id)[j]);

        P_mat(i,j) = exp(last_node.LogLike(last_particle_value_j, param_values_i));
      }
    }

    // TODO optimize using matrix operations
    ValArray new_weights = new_monitor.GetWeights();
    for (Size i=0; i<n_particles; ++i)
    {
      long double sum_j = 0.0;
      for (Size j=0; j<n_particles; ++j)
      {
        long double sum_l = 0.0;
        for (Size l=0; l<n_particles; ++l)
          sum_l += (long double)weights_[l] * P_mat(l,j);
        sum_j += (long double)weights_[j] * P_mat(i,j) / sum_l;
      }
      new_weights[i] *= sum_j;
    }

    weights_.swap(new_weights);

    filterMonitors_.pop_back();
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

}
