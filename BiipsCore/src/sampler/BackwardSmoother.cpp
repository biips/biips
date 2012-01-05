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
#include "sampler/GetNodeValueVisitor.hpp"

namespace Biips
{

  BackwardSmoother::BackwardSmoother(const Graph & graph, const Types<Monitor::Ptr>::Array & filterMonitors,
      const Types<Size>::Array & nodeIterations)
    : graph_(graph), filterMonitors_(filterMonitors), initialized_(false),
      nodeIterations_(nodeIterations)
  {
  }


  void BackwardSmoother::sumOfWeightsAndEss()
  {
    long double sum = 0.0;
    for(Size i = 0; i < weights_.size(); ++i)
      sum += weights_[i];

    sumOfWeights_ = sum;

    long double sum_sq = 0.0;
    for(Size i = 0; i < weights_.size(); i++)
      sum_sq += powl(weights_[i], 2);

    ess_ = expl(-logl(sum_sq) + 2*logl(sum));
  }
  
  
  void BackwardSmoother::Initialize()
  {
    Monitor & last_monitor = *filterMonitors_.back();
    iter_ = last_monitor.GetIteration();
    weights_.assign(last_monitor.GetUnnormWeights().begin(), last_monitor.GetUnnormWeights().end());

    sumOfWeights_ = last_monitor.GetSumOfWeights();
    ess_ = last_monitor.GetESS();

    initialized_ = true;
  }


  void BackwardSmoother::IterateBack()
  {
    if (!initialized_)
      throw LogicError("Can not iterate BackwardSmoother: not initialized.");

    Monitor::Ptr p_last_monitor = filterMonitors_.back();
    filterMonitors_.pop_back();

    if (iter_ == 0)
      throw LogicError("Can not iterate BackwardSmoother: the sequence has reached the end.");

    --iter_;

    if (filterMonitors_.empty())
      throw LogicError("Can not iterate BackwardSmoother: there is no remaining iteration of filtering Monitor object.");

    Monitor & new_monitor = *(filterMonitors_.back());

    if (new_monitor.GetIteration() != iter_)
    {
      throw LogicError(String("Can not iterate BackwardSmoother: missing iteration ")
          + print(iter_) + (" of filtering Monitor object."));
    }

    NodeId last_node_id = p_last_monitor->GetSampledNodes().front();
    if (graph_.GetNode(last_node_id).GetType() != STOCHASTIC)
      throw LogicError("Can not iterate BackwardSmoother: last sampled node is not stochastic.");

    const StochasticNode & last_node = dynamic_cast<const StochasticNode &>(graph_.GetNode(last_node_id));

    Types<NodeId>::Array last_parents_ids = last_node.Parents();
    for (Size p=0; p<last_parents_ids.size(); ++p)
    {
      NodeId param_id = last_parents_ids[p];
      if (!graph_.GetObserved()[param_id])
      {
        if (!new_monitor.Contains(param_id))
          throw LogicError("Can not iterate BackwardSmoother: not all the parents of the last sampled node are monitored.");
      }
    }

    Size n_particles = weights_.size();
    ublas::matrix<Scalar> P_mat(n_particles, n_particles);

    NumArray::Array param_values_i;
    NumArray last_particle_value_j;
    NumArray::Pair bound_values_i;

    // Computing matrix P
    for (Size i=0; i<n_particles; ++i)
    {
      for (Size j=0; j<n_particles; ++j)
      {
        param_values_i = getParamValues(last_node_id, graph_, new_monitor, i);
        bound_values_i = getBoundValues(last_node_id, graph_, new_monitor, i);

        last_particle_value_j = getNodeValue(last_node_id, graph_, *p_last_monitor, j);

        P_mat(i,j) = std::exp(last_node.LogPriorDensity(last_particle_value_j, param_values_i, bound_values_i));
        if (isNan(P_mat(i,j)))
          throw NodeError(last_node_id, "Failure to calculate log prior density.");
      }
    }

    // Updating weights
    Vector weights_filter_vec(n_particles, Vector::array_type());
    new_monitor.SwapWeights(weights_filter_vec.data());

    Vector weights_vec = ublas::prod(weights_filter_vec, P_mat);
    for (Size i=0; i<n_particles; ++i)
    {
      if (weights_vec[i] == 0.0)
        throw NumericalError(String("Failure to calculate log weight: can not divide by 0."));

      weights_[i] /= weights_vec[i];
    }

    weights_vec.data().clear();
    weights_vec.data().swap(weights_);

    weights_vec = ublas::prod(P_mat, weights_vec);
    weights_vec.data().swap(weights_);

    for (Size i=0; i<n_particles; ++i)
    {
      weights_[i] *= weights_filter_vec[i];

      if (isNan(weights_[i]))
        throw NumericalError(String("Failure to calculate log weight."));
    }

    new_monitor.SwapWeights(weights_filter_vec.data());

    sumOfWeightsAndEss();
  }


  Scalar BackwardSmoother::GetNodeESS(NodeId nodeId) const
  {
    if (!Initialized())
      throw LogicError("Can not GetNodeESS: smoother not initialized.");

    if (GetNodeSamplingIteration(nodeId) > iter_)
      throw LogicError("Can not get node ess: node was sampled at greater iteration.");

    Monitor & monitor = *(filterMonitors_.back());

    if (!monitor.Contains(nodeId))
      throw LogicError("Can not GetNodeESS: Node is not monitored.");

    if (!monitor.WeightsSet())
      throw LogicError("Can not GetNodeESS: weights not set.");

    if (std::find(UpdatedNodes().begin(), UpdatedNodes().end(), nodeId) != UpdatedNodes().end())
      return ess_;

    std::map<ValArray*, Types<Size>::Array> indices_table;
    for (Size i=0; i<monitor.GetNodeValues(nodeId).size(); ++i)
      indices_table[monitor.GetNodeValues(nodeId)[i].get()].push_back(i);

    typedef long double LongScalar;
    LongScalar sum_sq = 0.0;
    LongScalar sum_temp;
    for (std::map<ValArray*, Types<Size>::Array>::const_iterator it=indices_table.begin();
        it!=indices_table.end(); ++it)
    {
      sum_temp = 0.0;
      for (Size i=0; i<it->second.size(); ++i)
        sum_temp += weights_[it->second[i]];

      sum_sq += std::pow(sum_temp, 2);
    }
    if (sum_sq == 0.0)
      throw NumericalError("Failure to calculate node ESS: sum of squared weights is null.");

    return std::exp(-std::log(sum_sq) + 2*std::log(sumOfWeights_));
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

    featuresAcc.Init(graph_.GetNode(nodeId).DimPtr());
    for(Size i=0; i < last_monitor.NParticles(); i++)
      featuresAcc.Push(*(last_monitor.GetNodeValues(nodeId)[i]), weights_[i]);
  }


  Size BackwardSmoother::GetNodeSamplingIteration(NodeId nodeId) const
  {
    if (graph_.GetObserved()[nodeId])
      throw LogicError("Observed nodes can't have sampling iteration!");

    return nodeIterations_[nodeId];
  }
  
  
  void BackwardSmoother::InitMonitor(SmoothMonitor & monitor) const
  {
    monitor.Init(weights_, ess_, sumOfWeights_);
  }


  void BackwardSmoother::MonitorNode(NodeId nodeId, SmoothMonitor & monitor) const
  {
    Monitor & last_monitor = *filterMonitors_.back();
    monitor.AddNode(nodeId, last_monitor);

    Size iter = monitor.GetNodeSamplingIteration(nodeId);

    if (!monitor.HasIterationESS(iter))
      monitor.SetIterationESS(iter, GetNodeESS(nodeId));
  }

}
