//                                               -*- C++ -*-
/*
 * BiiPS software is a set of libraries for
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

/*! \file ConjugateMNormalVarLinear.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

#include "samplers/ConjugateMNormalVarLinear.hpp"
#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "sampler/NodesRelationVisitor.hpp"
#include "samplers/ConjugateMNormalVarLinear.hpp"
#include "samplers/GetMLinearTransformVisitor.hpp"
#include "samplers/IsLinearVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"
#include "distributions/DMNormVar.hpp"
#include "common/cholesky.hpp"

namespace Biips
{

  const String
      ConjugateMNormalVarLinear::NAME_ =
          "Conjugate Multivariate Normal (with known covariance matrix and linear mean function)";

  class MNormalVarLinearLikeFormVisitor: public ConstNodeVisitor
  {
  protected:
    typedef MNormalVarLinearLikeFormVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    const Graph & graph_;
    NodeId myId_;
    NodeSampler & nodeSampler_;
    Size dimNode_;
    Matrix A_;
    Vector b_;
    Matrix cov_;
    Vector obs_;

    virtual void visit(const StochasticNode & node) // TODO optimize (using effective uBlas functions)
    {
      NumArray cov_i_dat(getNodeValue(node.Parents()[1], graph_, nodeSampler_));
      MatrixRef cov_i(cov_i_dat);
      Size dim_obs = cov_i.size1();
      Size cov_old_dim = cov_.size1();
      cov_.resize(cov_old_dim + dim_obs, cov_old_dim + dim_obs);
      ublas::project(cov_,
                     ublas::range(cov_old_dim, cov_.size1()),
                     ublas::range(cov_old_dim, cov_.size2())) = cov_i;
      cov_i.Release();

      GetMLinearTransformVisitor get_lin_trans_vis(graph_,
                                                   myId_,
                                                   nodeSampler_,
                                                   dimNode_,
                                                   dim_obs);
      graph_.VisitNode(node.Parents()[0], get_lin_trans_vis);

      const Matrix & A_i = get_lin_trans_vis.GetA();
      Size a_old_size1 = A_.size1();
      A_.resize(a_old_size1 + A_i.size1(), A_.size2());
      ublas::project(A_,
                     ublas::range(a_old_size1, A_.size1()),
                     ublas::range(0, A_.size2())) = A_i; // FIXME

      const Vector & b_i = get_lin_trans_vis.GetB();
      Size b_old_size = b_.size();
      b_.resize(b_old_size + b_i.size());
      ublas::project(b_, ublas::range(b_old_size, b_.size())) = b_i;

      NumArray
          obs_i_dat(node.DimPtr().get(), graph_.GetValues()[nodeId_].get());

      VectorRef obs_i(obs_i_dat);
      Size obs_old_size = obs_.size();
      obs_.resize(obs_old_size + obs_i.size());
      ublas::project(obs_, ublas::range(obs_old_size, obs_.size())) = obs_i;
    }

  public:
    const Matrix & GetA()
    {
      return A_;
    }
    const Vector & GetB()
    {
      return b_;
    }
    const Matrix & GetCov()
    {
      return cov_;
    }
    Vector & GetObs()
    {
      return obs_;
    }

    MNormalVarLinearLikeFormVisitor(const Graph & graph,
                                    NodeId myId,
                                    NodeSampler & nodeSampler,
                                    Size dimNode) // TODO manage dimension
    :
      graph_(graph), myId_(myId), nodeSampler_(nodeSampler), dimNode_(dimNode),
          A_(0, dimNode), b_(0), cov_(0, 0), obs_(0)
    {
    }
  };

  void ConjugateMNormalVarLinear::sample(const StochasticNode & node) // TODO optimize (using effective uBlas functions)
  {
    NodeId prior_mean_id = node.Parents()[0];
    NodeId prior_var_id = node.Parents()[1];
    Size dim_node = graph_.GetNode(prior_mean_id).Dim()[0];

    GraphTypes::LikelihoodChildIterator it_offspring, it_offspring_end;
    boost::tie(it_offspring, it_offspring_end)
        = graph_.GetLikelihoodChildren(nodeId_);

    MNormalVarLinearLikeFormVisitor like_form_vis(graph_,
                                                  nodeId_,
                                                  *this,
                                                  dim_node);
    while (it_offspring != it_offspring_end)
    {
      graph_.VisitNode(*it_offspring, like_form_vis);
      ++it_offspring;
    }

    const Matrix & like_A = like_form_vis.GetA();
    const Vector & like_b = like_form_vis.GetB();
    const Matrix & like_cov = like_form_vis.GetCov();
    Vector & obs = like_form_vis.GetObs();

    NumArray prior_var_dat(getNodeValue(prior_var_id, graph_, *this));
    MatrixRef prior_var(prior_var_dat);

    Matrix kalman_gain = ublas::prod(prior_var, ublas::trans(like_A));
    static Matrix inn_cov;
    inn_cov = ublas::prod(like_A, kalman_gain) + like_cov;
    Matrix inn_cov_inv = inn_cov;
    if (!ublas::cholesky_factorize(inn_cov_inv))
      throw LogicError("ConjugateMNormalVarLinear::sample: matrix inn_cov_inv is not positive-semidefinite.");
    ublas::cholesky_invert(inn_cov_inv);
    kalman_gain = ublas::prod(kalman_gain, inn_cov_inv);

    NumArray prior_mean_dat(getNodeValue(prior_mean_id, graph_, *this));
    VectorRef prior_mean(prior_mean_dat);

    static Vector obs_pred;
    obs_pred = ublas::prod(like_A, prior_mean) + like_b;
    static Vector post_mean;
    post_mean = prior_mean + ublas::prod(kalman_gain, (obs - obs_pred));
    prior_mean.Release();

    static Matrix post_var;
    post_var = ublas::prod(Matrix(ublas::identity_matrix<Scalar>(dim_node,
                                                                 dim_node)
        - Matrix(ublas::prod(kalman_gain, like_A))), prior_var);
    prior_var.Release();

    static NumArray::Array post_param_values(2);
    static DimArray dim_mean(1);
    dim_mean[0] = post_mean.size();
    post_param_values[0] = NumArray(&dim_mean, &post_mean.data());
    static DimArray dim_cov(2);
    dim_cov[0] = post_var.size1();
    dim_cov[1] = post_var.size2();
    post_param_values[1] = NumArray(&dim_cov, &post_var.data());

    //allocate memory
    nodeValuesMap()[nodeId_].reset(new ValArray(node.Dim().Length()));
    //sample
    DMNormVar::Instance()->Sample(*nodeValuesMap()[nodeId_],
                                  post_param_values,
                                  NULL_NUMARRAYPAIR,
                                  *pRng_);

    static NumArray::Array norm_const_param_values(2);
    dim_mean[0] = obs_pred.size();
    norm_const_param_values[0] = NumArray(&dim_mean, &obs_pred.data());
    dim_cov[0] = inn_cov.size1();
    dim_cov[1] = inn_cov.size2();
    norm_const_param_values[1] = NumArray(&dim_cov, &inn_cov.data());
    logIncrementalWeight_
        = DMNormVar::Instance()->LogDensity(NumArray(&dim_mean, &obs.data()),
                                            norm_const_param_values,
                                            NULL_NUMARRAYPAIR); // FIXME Boundaries
    if (isNan(logIncrementalWeight_))
      throw RuntimeError("Failure to calculate log incremental weight.");
    // TODO optimize computation removing constant terms

    sampledFlagsMap()[nodeId_] = true;
  }

  class IsConjugateMNormalVarLinearVisitor: public ConstNodeVisitor
  {
  protected:
    const Graph & graph_;
    const NodeId myId_;
    Bool conjugate_;

    void visit(const StochasticNode & node)
    {
      conjugate_ = false;

      if (node.PriorName() != "dmnormvar")
        return;

      // FIXME
      if (node.IsBounded())
        return;

      NodeId mean_id = node.Parents()[0];
      NodeId var_id = node.Parents()[1];
      conjugate_ = ((nodesRelation(var_id, myId_, graph_) == KNOWN)
          && isLinear(mean_id, myId_, graph_));
    }

  public:
    Bool IsConjugate() const
    {
      return conjugate_;
    }

    IsConjugateMNormalVarLinearVisitor(const Graph & graph, NodeId myId) :
      graph_(graph), myId_(myId), conjugate_(false)
    {
    }
  };

  class CanSampleMNormalVarLinearVisitor: public ConstNodeVisitor
  {
  protected:
    const Graph & graph_;
    Bool canSample_;

    void visit(const StochasticNode & node)
    {
      canSample_ = false;

      if (graph_.GetObserved()[nodeId_])
        throw LogicError("CanSampleMNormalVarLinearVisitor can not visit observed node: node id sequence of the forward sampler may be bad.");

      if (node.PriorName() != "dmnormvar")
        return;

      // FIXME
      if (node.IsBounded())
        return;

      IsConjugateMNormalVarLinearVisitor child_vis(graph_, nodeId_);

      GraphTypes::LikelihoodChildIterator it_offspring, it_offspring_end;
      boost::tie(it_offspring, it_offspring_end)
          = graph_.GetLikelihoodChildren(nodeId_);
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

    CanSampleMNormalVarLinearVisitor(const Graph & graph) :
      graph_(graph), canSample_(false)
    {
    }
  };

  Bool ConjugateMNormalVarLinearFactory::Create(const Graph & graph,
                                                NodeId nodeId,
                                                BaseType::CreatedPtr & pNodeSamplerInstance) const
  {
    CanSampleMNormalVarLinearVisitor can_sample_vis(graph);

    graph.VisitNode(nodeId, can_sample_vis);

    Bool flag_created = can_sample_vis.CanSample();

    if (flag_created)
    {
      pNodeSamplerInstance
          = NodeSamplerFactory::CreatedPtr(new CreatedType(graph));
    }

    return flag_created;
  }

  ConjugateMNormalVarLinearFactory::Ptr
      ConjugateMNormalVarLinearFactory::pConjugateMNormalVarLinearFactoryInstance_(new ConjugateMNormalVarLinearFactory());

}
