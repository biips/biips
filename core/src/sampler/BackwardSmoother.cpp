//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
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

/*! \file BackwardSmoother.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "sampler/BackwardSmoother.hpp"
#include "graph/StochasticNode.hpp"
#include "common/Utility.hpp"
#include "common/Accumulator.hpp"
#include "common/ArrayAccumulator.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "boost/numeric/ublas/matrix_proxy.hpp"

namespace Biips
{

  BackwardSmoother::BackwardSmoother(const Graph & graph,
                                     const Types<Monitor::Ptr>::Array & filterMonitors,
                                     const Types<Size>::Array & nodeIterations) :
    graph_(graph), filterMonitors_(filterMonitors), initialized_(false),
        nodeIterations_(nodeIterations)
  {
  }

  void BackwardSmoother::sumOfWeightsAndEss()
  {
    long double sum = 0.0;
    for (Size i = 0; i < weights_.size(); ++i)
      sum += weights_[i];

    sumOfWeights_ = sum;

    long double sum_sq = 0.0;
    for (Size i = 0; i < weights_.size(); i++)
      sum_sq += powl(weights_[i], 2);

    ess_ = expl(-logl(sum_sq) + 2 * logl(sum));
  }

  void BackwardSmoother::Initialize()
  {
    Monitor & last_monitor = *filterMonitors_.back();
    iter_ = last_monitor.GetIteration();
    //    logWeights_.assign(last_monitor.GetUnnormLogWeights().begin(),
    //                       last_monitor.GetUnnormLogWeights().end());
    weights_.assign(last_monitor.GetUnnormWeights().begin(),
                    last_monitor.GetUnnormWeights().end());

    sumOfWeights_ = last_monitor.GetSumOfWeights();
    ess_ = last_monitor.GetESS();

    initialized_ = true;
  }

  //  struct ExpFunction: public std::unary_function<Scalar, Scalar>
  //  {
  //    Scalar operator()(Scalar val) const
  //    {
  //      return std::exp(val);
  //    }
  //  };


  const Monitor::Ptr & BackwardSmoother::getParentFilterMonitor(NodeId id)
  {
    if (graph_.GetObserved()[id])
    {
      return filterMonitors_.back();
      // any monitor is ok because observed node values
      // are not searched in monitors
    }
    else
    {
      Types<Monitor::Ptr>::Array::const_reverse_iterator rit =
          filterMonitors_.rbegin();
      while (!(*rit)->Contains(id) && (rit != filterMonitors_.rend()))
      {
        ++rit;
      }
      if (rit == filterMonitors_.rend())
        throw LogicError("getParentFilterMonitor: there is one parent unmonitored parent.");

      return *rit;
    }
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

    const StochasticNode & last_node =
        dynamic_cast<const StochasticNode &> (graph_.GetNode(last_node_id));

    // fill parameters monitors vector
    Types<Monitor::Ptr>::Array param_monitors;
    for (Size p = 0; p < last_node.Parents().size(); ++p)
      param_monitors.push_back(getParentFilterMonitor(last_node.Parents()[p]));

    // fill bounds monitors pair
    Types<Monitor::Ptr>::Pair bound_monitors;
    if (last_node.IsLowerBounded())
      bound_monitors.second = getParentFilterMonitor(last_node.Lower());
    if (last_node.IsUpperBounded())
      bound_monitors.second = getParentFilterMonitor(last_node.Upper());

    Size n_particles = weights_.size();
    ublas::matrix<Scalar> P_mat(n_particles, n_particles);

    NumArray::Array param_values_i;
    NumArray last_particle_value_j;
    NumArray::Pair bound_values_i;

    // Computing matrix P
    for (Size i = 0; i < n_particles; ++i)
    {
      for (Size j = 0; j < n_particles; ++j)
      {
        param_values_i
            = getParamValues(last_node_id, graph_, param_monitors, i);
        bound_values_i
            = getBoundValues(last_node_id, graph_, bound_monitors, i);

        last_particle_value_j = getNodeValue(last_node_id,
                                             graph_,
                                             *p_last_monitor,
                                             j);

        Scalar d = std::exp(last_node.LogPriorDensity(last_particle_value_j,
                                                      param_values_i,
                                                      bound_values_i));

        //        Scalar d = last_node.LogPriorDensity(last_particle_value_j,
        //                                             param_values_i,
        //                                             bound_values_i);
        if (isNan(d))
          throw NodeError(last_node_id,
                          "Failure to calculate log prior density.");
        P_mat(i, j) = d;
      }
    }

    //    // Updating weights
    //    Vector log_weights_filter_vec(n_particles, Vector::array_type());
    //    new_monitor.SwapLogWeights(log_weights_filter_vec.data());
    //
    //    Vector log_weights_temp_vec(n_particles);
    //    for (Size j = 0; j < n_particles; ++j)
    //    {
    //      Vector v = log_weights_temp_vec + ublas::column(P_mat, j);
    //
    //      long double sum = 0.0;
    //      for (Size l = 0; l < n_particles; ++l)
    //        sum += expl(v(l));
    //      log_weights_temp_vec(j) = std::log(sum);
    //    }
    //
    //    Vector log_weights_smooth_vec(n_particles, Vector::array_type());
    //    logWeights_.swap(log_weights_smooth_vec.data());
    //
    //    Vector log_weights_temp2_vec(n_particles);
    //    for (Size i = 0; i < n_particles; ++i)
    //    {
    //      Vector v = log_weights_smooth_vec + ublas::row(P_mat, i)
    //          + log_weights_temp_vec;
    //      long double sum = 0.0;
    //      for (Size j = 0; j < n_particles; ++j)
    //        sum += expl(v(j));
    //      log_weights_temp2_vec(i) = std::log(sum);
    //    }
    //
    //    log_weights_smooth_vec = log_weights_filter_vec + log_weights_temp2_vec;
    //
    //    logWeights_.swap(log_weights_smooth_vec.data());
    //    std::transform(logWeights_.begin(),
    //                   logWeights_.end(),
    //                   weights_.begin(),
    //                   ExpFunction());

    // Updating weights
    Vector weights_filter_vec(n_particles, Vector::array_type());
    new_monitor.SwapWeights(weights_filter_vec.data());
    Vector weights_vec = ublas::prod(weights_filter_vec, P_mat);
    for (Size i = 0; i < n_particles; ++i)
    {
      if (weights_vec[i] == 0.0)
        throw NumericalError(String("Failure to calculate log weight: can not divide by 0."));

      weights_[i] /= weights_vec[i];
    }

    weights_vec.data().clear();
    weights_vec.data().swap(weights_);

    weights_vec = ublas::prod(P_mat, weights_vec);
    weights_vec.data().swap(weights_);

    for (Size i = 0; i < n_particles; ++i)
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

    if (std::find(UpdatedNodes().begin(), UpdatedNodes().end(), nodeId)
        != UpdatedNodes().end())
      return ess_;

    std::map<ValArray*, Types<Size>::Array> indices_table;
    for (Size i = 0; i < monitor.GetNodeValues(nodeId).size(); ++i)
      indices_table[monitor.GetNodeValues(nodeId)[i].get()].push_back(i);

    typedef long double LongScalar;
    LongScalar sum_sq = 0.0;
    LongScalar sum_temp;
    for (std::map<ValArray*, Types<Size>::Array>::const_iterator it =
        indices_table.begin(); it != indices_table.end(); ++it)
    {
      sum_temp = 0.0;
      for (Size i = 0; i < it->second.size(); ++i)
        sum_temp += weights_[it->second[i]];

      sum_sq += std::pow(sum_temp, 2);
    }
    if (sum_sq == 0.0)
      throw NumericalError("Failure to calculate node ESS: sum of squared weights is null.");

    return std::exp(-std::log(sum_sq) + 2 * std::log(sumOfWeights_));
  }

  void BackwardSmoother::Accumulate(NodeId nodeId,
                                    Accumulator & featuresAcc,
                                    Size n) const
  {
    const Monitor & last_monitor = *filterMonitors_.back();
    if (!last_monitor.Contains(nodeId))
      throw LogicError(String("Can not accumulate BackwardSmoother: node "
          + print(nodeId) + " is not monitored at the current iteration."));

    featuresAcc.Init();
    for (Size i = 0; i < last_monitor.NParticles(); i++)
      featuresAcc.Push((*(last_monitor.GetNodeValues(nodeId)[i]))[n],
                       weights_[i]);
  }

  void BackwardSmoother::Accumulate(NodeId nodeId,
                                    DiscreteAccumulator & featuresAcc,
                                    Size n) const
  {
    const Monitor & last_monitor = *filterMonitors_.back();
    if (!last_monitor.Contains(nodeId))
      throw LogicError(String("Can not accumulate BackwardSmoother: node "
          + print(nodeId) + " is not monitored at the current iteration."));

    featuresAcc.Init();
    for (Size i = 0; i < last_monitor.NParticles(); i++)
      featuresAcc.Push((*(last_monitor.GetNodeValues(nodeId)[i]))[n],
                       weights_[i]);
  }

  void BackwardSmoother::Accumulate(NodeId nodeId,
                                    ArrayAccumulator & featuresAcc) const
  {
    const Monitor & last_monitor = *filterMonitors_.back();
    if (!last_monitor.Contains(nodeId))
      throw LogicError(String("Can not accumulate BackwardSmoother: node "
          + print(nodeId) + " is not monitored at the current iteration."));

    featuresAcc.Init(graph_.GetNode(nodeId).DimPtr());
    for (Size i = 0; i < last_monitor.NParticles(); i++)
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
