//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
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

/*! \file ConjugateNormalLinear.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "samplers/ConjugateNormalLinear.hpp"
#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "sampler/NodesRelationVisitor.hpp"
#include "samplers/GetLinearTransformVisitor.hpp"
#include "samplers/IsLinearVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"
#include "distributions/DNorm.hpp"

namespace Biips
{

  const String ConjugateNormalLinear::NAME_ =
      "Conjugate Normal (with known precision and linear mean function)";

  class NormalLinearLikeFormVisitor: public ConstNodeVisitor
  {
  protected:
    typedef NormalLinearLikeFormVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    const Graph & graph_;
    NodeId myId_;
    ConjugateNormalLinear & nodeSampler_;
    Scalar mean_;
    Scalar prec_;

    virtual void visit(const StochasticNode & node)
    {
      mean_ = 0.0;
      prec_ = 0.0;

      NodeId mean_id = node.Parents()[0];
      NodeId prec_id = node.Parents()[1];

      Scalar prec = getNodeValue(prec_id, graph_, nodeSampler_).ScalarView();
      GetLinearTransformVisitor get_lin_trans_vis(graph_, myId_, nodeSampler_);
      graph_.VisitNode(mean_id, get_lin_trans_vis);

      prec_ = get_lin_trans_vis.GetA() * prec;
      mean_ = (graph_.GetValues()[nodeId_]->ScalarView()
          - get_lin_trans_vis.GetB()) * prec_;
      prec_ *= get_lin_trans_vis.GetA();
    }

  public:
    Scalar GetMean() const
    {
      return mean_;
    }

    Scalar GetPrec() const
    {
      return prec_;
    }

    NormalLinearLikeFormVisitor(const Graph & graph,
                                NodeId myId,
                                ConjugateNormalLinear & nodeSampler) :
      graph_(graph), myId_(myId), nodeSampler_(nodeSampler), mean_(0.0),
          prec_(0.0)
    {
    }
  };

  void ConjugateNormalLinear::sample(const StochasticNode & node)
  {
    NodeId prior_mean_id = node.Parents()[0];
    NodeId prior_prec_id = node.Parents()[1];

    static ValArray prior_mean;
    prior_mean = getNodeValue(prior_mean_id, graph_, *this).Values();
    Scalar prior_prec = getNodeValue(prior_prec_id, graph_, *this).ScalarView();

    Scalar like_prec = 0.0;
    static ValArray like_mean(1);
    like_mean[0] = 0.0;

    GraphTypes::LikelihoodChildIterator it_offspring, it_offspring_end;
    boost::tie(it_offspring, it_offspring_end)
        = graph_.GetLikelihoodChildren(nodeId_);
    NormalLinearLikeFormVisitor like_form_vis(graph_, nodeId_, *this);
    while (it_offspring != it_offspring_end)
    {
      graph_.VisitNode(*it_offspring, like_form_vis);
      like_prec += like_form_vis.GetPrec();
      like_mean[0] += like_form_vis.GetMean();
      ++it_offspring;
    }

    static ValArray post_prec(1);
    post_prec[0] = prior_prec + like_prec;
    static ValArray post_mean(1);
    post_mean[0] = (prior_mean[0] * prior_prec + like_mean[0]) / post_prec[0];
    like_mean[0] /= like_prec;

    static NumArray::Array post_param_values(2);
    post_param_values[0].SetPtr(P_SCALAR_DIM.get(), &post_mean);
    post_param_values[1].SetPtr(P_SCALAR_DIM.get(), &post_prec);

    //allocate memory
    nodeValuesMap()[nodeId_].reset(new ValArray(1));
    //sample
    DNorm::Instance()->Sample(*nodeValuesMap()[nodeId_],
                              post_param_values,
                              NULL_NUMARRAYPAIR,
                              *pRng_);

    static NumArray::Array norm_const_param_values(2);
    norm_const_param_values[0].SetPtr(P_SCALAR_DIM.get(), &like_mean);
    static ValArray norm_const_prec(1);
    norm_const_prec[0] = 1.0 / (1.0 / prior_prec + 1.0 / like_prec);
    norm_const_param_values[1].SetPtr(P_SCALAR_DIM.get(), &norm_const_prec);
    logIncrementalWeight_
        = DNorm::Instance()->LogDensity(NumArray(P_SCALAR_DIM.get(),
                                                 &prior_mean),
                                        norm_const_param_values,
                                        NULL_NUMARRAYPAIR); // FIXME Boundaries
    if (isNan(logIncrementalWeight_))
      throw RuntimeError("Failure to calculate log incremental weight.");

    // TODO optimize computation removing constant terms

    sampledFlagsMap()[nodeId_] = true;
  }

  class IsConjugateNormalLinearVisitor: public ConstNodeVisitor
  {
  protected:
    const Graph & graph_;
    const NodeId myId_;
    Bool conjugate_;

    void visit(const StochasticNode & node)
    {
      conjugate_ = false;
      if (node.PriorName() != "dnorm")
        return;

      // FIXME
      if (node.IsBounded())
        return;

      NodeId mean_id = node.Parents()[0];
      NodeId var_id = node.Parents()[1];
      conjugate_ = ((nodesRelation(var_id, myId_, graph_) == KNOWN)
          && isLinear(mean_id, myId_, graph_)) ? true : false;
    }

  public:
    Bool IsConjugate() const
    {
      return conjugate_;
    }

    IsConjugateNormalLinearVisitor(const Graph & graph, NodeId myId) :
      graph_(graph), myId_(myId), conjugate_(false)
    {
    }
  };

  class CanSampleNormalLinearVisitor: public ConstNodeVisitor
  {
  protected:
    const Graph & graph_;
    Bool canSample_;

    void visit(const StochasticNode & node)
    {
      canSample_ = false;

      if (graph_.GetObserved()[nodeId_])
        throw LogicError("CanSampleNormalLinearVisitor can not visit observed node: node id sequence of the forward sampler may be bad.");

      if (node.PriorName() != "dnorm")
        return;

      // FIXME
      if (node.IsBounded())
        return;

      GraphTypes::LikelihoodChildIterator it_offspring, it_offspring_end;
      boost::tie(it_offspring, it_offspring_end)
          = graph_.GetLikelihoodChildren(nodeId_);

      IsConjugateNormalLinearVisitor child_vis(graph_, nodeId_);

      for (; it_offspring != it_offspring_end; ++it_offspring)
      {
        graph_.VisitNode(*it_offspring, child_vis);

        canSample_ = child_vis.IsConjugate();

        if (!canSample_)
          break;
      }
    }

  public:
    Bool CanSample() const
    {
      return canSample_;
    }

    CanSampleNormalLinearVisitor(const Graph & graph) :
      graph_(graph), canSample_(false)
    {
    }
  };

  Bool ConjugateNormalLinearFactory::Create(const Graph & graph,
                                            NodeId nodeId,
                                            BaseType::CreatedPtr & pNodeSamplerInstance) const
  {
    CanSampleNormalLinearVisitor can_sample_vis(graph);

    graph.VisitNode(nodeId, can_sample_vis);

    Bool flag_created = can_sample_vis.CanSample();

    if (flag_created)
    {
      pNodeSamplerInstance
          = NodeSamplerFactory::CreatedPtr(new CreatedType(graph));
    }

    return flag_created;
  }

  ConjugateNormalLinearFactory::Ptr
      ConjugateNormalLinearFactory::pConjugateNormalLinearFactoryInstance_(new ConjugateNormalLinearFactory());

}

