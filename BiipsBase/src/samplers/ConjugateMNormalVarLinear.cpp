//                                               -*- C++ -*-
/*! \file ConjugateMNormalVarLinear.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
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

  const String ConjugateMNormalVarLinear::NAME_ = "Conjugate Multivariate Normal (with known covariance matrix and linear mean function)";


  class MNormalVarLinearLikeFormVisitor : public ConstStochasticNodeVisitor
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
      if ( !graph_.GetObserved()[nodeId_] )
        return;

      MultiArray cov_i_dat(getNodeValue(node.Parents()[1], graph_, nodeSampler_));
      MatrixRef cov_i(cov_i_dat);
      Size dim_obs = cov_i.size1();
      Size cov_old_dim = cov_.size1();
      cov_.resize(cov_old_dim + dim_obs, cov_old_dim + dim_obs);
      ublas::project(cov_, ublas::range(cov_old_dim, cov_.size1()), ublas::range(cov_old_dim, cov_.size2())) = cov_i;
      cov_i.Release();

      GetMLinearTransformVisitor get_lin_trans_vis(graph_, myId_, nodeSampler_, dimNode_, dim_obs);
      graph_.VisitNode(node.Parents()[0], get_lin_trans_vis);

      const Matrix & A_i = get_lin_trans_vis.GetA();
      Size a_old_size1 = A_.size1();
      A_.resize(a_old_size1 + A_i.size1(), A_.size2());
      ublas::project(A_, ublas::range(a_old_size1, A_.size1()), ublas::range(0, A_.size2())) = A_i;  // FIXME

      const Vector & b_i = get_lin_trans_vis.GetB();
      Size b_old_size = b_.size();
      b_.resize(b_old_size + b_i.size());
      ublas::project(b_, ublas::range(b_old_size, b_.size())) = b_i;

      MultiArray obs_i_dat(node.DimPtr(), graph_.GetValues()[nodeId_]);

      VectorRef obs_i(obs_i_dat);
      Size obs_old_size = obs_.size();
      obs_.resize(obs_old_size + obs_i.size());
      ublas::project(obs_, ublas::range(obs_old_size, obs_.size())) = obs_i;
    }

  public:
    const Matrix & GetA() { return A_; };
    const Vector & GetB() { return b_; };
    const Matrix & GetCov() { return cov_; };
    const Vector & GetObs() { return obs_; };

    MNormalVarLinearLikeFormVisitor(const Graph & graph, NodeId myId, NodeSampler & nodeSampler, Size dimNode) // TODO manage dimension
    : graph_(graph), myId_(myId), nodeSampler_(nodeSampler), dimNode_(dimNode), A_(0, dimNode), b_(0), cov_(0,0), obs_ (0) {};
  };


  void ConjugateMNormalVarLinear::sample(const StochasticNode & node) // TODO optimize (using effective uBlas functions)
  {
    NodeId prior_mean_id = node.Parents()[0];
    NodeId prior_var_id = node.Parents()[1];
    Size dim_node = graph_.GetNode(prior_mean_id).Dim()[0];

    StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
    boost::tie(it_offspring, it_offspring_end) = graph_.GetStochasticChildren(nodeId_);

    MNormalVarLinearLikeFormVisitor like_form_vis(graph_, nodeId_, *this, dim_node);
    while ( it_offspring != it_offspring_end )
    {
      graph_.VisitNode(*it_offspring, like_form_vis);
      ++it_offspring;
    }

    const Matrix & like_A = like_form_vis.GetA();
    const Vector & like_b = like_form_vis.GetB();
    const Matrix & like_cov = like_form_vis.GetCov();
    const Vector & obs = like_form_vis.GetObs();

    MultiArray prior_var_dat(getNodeValue(prior_var_id, graph_, *this));
    MatrixRef prior_var(prior_var_dat);

    Matrix kalman_gain = ublas::prod(prior_var, ublas::trans(like_A));
    Matrix inn_cov = ublas::prod(like_A, kalman_gain) + like_cov;
    Matrix inn_cov_inv = inn_cov;
    ublas::cholesky_factorize(inn_cov_inv);
    ublas::cholesky_invert(inn_cov_inv);
    kalman_gain = ublas::prod(kalman_gain, inn_cov_inv);

    MultiArray prior_mean_dat(getNodeValue(prior_mean_id, graph_, *this));
    VectorRef prior_mean(prior_mean_dat);

    Vector obs_pred = ublas::prod(like_A, prior_mean) + like_b;
    Vector post_mean = prior_mean + ublas::prod(kalman_gain, (obs - obs_pred));
    prior_mean.Release();

    Matrix post_var = ublas::prod(Matrix(ublas::identity_matrix<Scalar>(dim_node, dim_node) - Matrix(ublas::prod(kalman_gain, like_A))), prior_var);
    prior_var.Release();

    MultiArray::Array post_param_values(2);
    post_param_values[0] = MultiArray(post_mean);
    post_param_values[1] = MultiArray(post_var);
    nodeValuesMap_[nodeId_] = DMNormVar::Instance()->Sample(post_param_values, NULL_MULTIARRAYPAIR, *pRng_).ValuesPtr();

    MultiArray::Array norm_const_param_values(2);
    norm_const_param_values[0] = MultiArray(obs_pred);
    norm_const_param_values[1] = MultiArray(inn_cov);
    logIncrementalWeight_ = DMNormVar::Instance()->LogDensity(MultiArray(obs), norm_const_param_values, NULL_MULTIARRAYPAIR); // FIXME Boundaries
    if (isNan(logIncrementalWeight_))
      throw RuntimeError("Failure to calculate log incremental weight.");
    // TODO optimize computation removing constant terms

    sampledFlagsMap_[nodeId_] = true;
  }


  class IsConjugateMNormalVarLinearVisitor : public ConstStochasticNodeVisitor
  {
  protected:
    const Graph & graph_;
    const NodeId myId_;
    Bool conjugate_;

    void visit(const StochasticNode & node)
    {
      conjugate_ = false;

      if ( node.PriorName() == "dmnormvar" )
        return;

      // FIXME
      if (node.IsBounded())
        return;

      NodeId mean_id = node.Parents()[0];
      NodeId var_id = node.Parents()[1];
      conjugate_ = ( (nodesRelation(var_id, myId_, graph_) == KNOWN )
          && isLinear(mean_id, myId_, graph_) ) ? true : false;
    }

  public:
    Bool IsConjugate() const { return conjugate_; }

    IsConjugateMNormalVarLinearVisitor(const Graph & graph, NodeId myId) : graph_(graph), myId_(myId), conjugate_(false) {}
  };



  class CanSampleMNormalVarLinearVisitor : public ConstStochasticNodeVisitor
  {
  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;

    const Graph & graph_;
    Bool canSample_;

    void visit(const StochasticNode & node)
    {
      canSample_ = false;

      if ( graph_.GetObserved()[nodeId_] )
        throw LogicError("CanSampleMNormalVarLinearVisitor can not visit observed node: node id sequence of the forward sampler may be bad.");

      if ( node.PriorName() != "dmnormvar" )
        return;

      // FIXME
      if (node.IsBounded())
        return;

      StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
      boost::tie(it_offspring, it_offspring_end) = graph_.GetStochasticChildren(nodeId_);

      IsConjugateMNormalVarLinearVisitor child_vis(graph_, nodeId_);

      while ( it_offspring != it_offspring_end )
      {
        if ( graph_.GetObserved()[*it_offspring] )
        {
          graph_.VisitNode(*it_offspring, child_vis);

          canSample_ = child_vis.IsConjugate();

          if (!canSample_)
            break;
        }
        ++it_offspring;
      }
    }

  public:
    Bool CanSample() const { return canSample_; }

    CanSampleMNormalVarLinearVisitor(const Graph & graph) : graph_(graph), canSample_(false) {}
  };



  Bool ConjugateMNormalVarLinearFactory::Create(const Graph & graph, NodeId nodeId, BaseType::CreatedPtr & pNodeSamplerInstance) const
  {
    CanSampleMNormalVarLinearVisitor can_sample_vis(graph);

    graph.VisitNode(nodeId, can_sample_vis);

    Bool flag_created = can_sample_vis.CanSample();

    if ( flag_created )
    {
      pNodeSamplerInstance = NodeSamplerFactory::CreatedPtr(new CreatedType(graph));
    }

    return flag_created;
  }


  ConjugateMNormalVarLinearFactory::Ptr ConjugateMNormalVarLinearFactory::pConjugateMNormalVarLinearFactoryInstance_(new ConjugateMNormalVarLinearFactory());


}
