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

  class MNormalVarLinearLikeFormVisitor : public ConstNodeVisitor
  {
  protected:
    typedef MNormalVarLinearLikeFormVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    const Graph * pGraph_;
    NodeId myId_;
    NodeSampler * pSampleNodeVis_;
    Size dimNode_;
    Matrix A_;
    Vector b_;
    Matrix cov_;
    Vector obs_;

  public:
    virtual void Visit(const ConstantNode & node) {};

    virtual void Visit(const StochasticNode & node) // TODO optimize (using effective uBlas functions)
    {
      if ( nodeIdDefined_ ) // TODO manage else case : throw exception
      {
        if ( pGraph_->GetObserved()[nodeId_] )
        {
          DataType cov_i_dat(getNodeValue(node.Parents()[1], pGraph_, pSampleNodeVis_));
          MatrixRef cov_i(cov_i_dat);
          Size dim_obs = cov_i.size1();
          Size cov_old_dim = cov_.size1();
          cov_.resize(cov_old_dim + dim_obs, cov_old_dim + dim_obs);
          ublas::project(cov_, ublas::range(cov_old_dim, cov_.size1()), ublas::range(cov_old_dim, cov_.size2())) = cov_i;
          cov_i.Release();

          GetMLinearTransformVisitor get_lin_trans_vis(pGraph_, myId_, pSampleNodeVis_, dimNode_, dim_obs);
          pGraph_->VisitNode(node.Parents()[0], get_lin_trans_vis);

          const Matrix & A_i = get_lin_trans_vis.GetA();
          Size a_old_size1 = A_.size1();
          A_.resize(a_old_size1 + A_i.size1(), A_.size2());
          ublas::project(A_, ublas::range(a_old_size1, A_.size1()), ublas::range(0, A_.size2())) = A_i;  // FIXME

          const Vector & b_i = get_lin_trans_vis.GetB();
          Size b_old_size = b_.size();
          b_.resize(b_old_size + b_i.size());
          ublas::project(b_, ublas::range(b_old_size, b_.size())) = b_i;

          DataType obs_i_dat(node.DimPtr(), pGraph_->GetValues()[nodeId_]);

          VectorRef obs_i(obs_i_dat);
          Size obs_old_size = obs_.size();
          obs_.resize(obs_old_size + obs_i.size());
          ublas::project(obs_, ublas::range(obs_old_size, obs_.size())) = obs_i;
        }
      }
    };

    virtual void Visit(const LogicalNode & node) {};

    const Matrix & GetA() { return A_; };
    const Vector & GetB() { return b_; };
    const Matrix & GetCov() { return cov_; };
    const Vector & GetObs() { return obs_; };

    MNormalVarLinearLikeFormVisitor(const Graph * pGraph, NodeId myId, NodeSampler * pSampleNodeVis, Size dimNode) // TODO manage dimension
    : pGraph_(pGraph), myId_(myId), pSampleNodeVis_(pSampleNodeVis), dimNode_(dimNode), A_(0, dimNode), b_(0), cov_(0,0), obs_ (0) {};
  };


  void ConjugateMNormalVarLinear::Visit(const StochasticNode & node) // TODO optimize (using effective uBlas functions)
  {
    if ( nodeIdDefined_ && attributesDefined_ ) // TODO manage else case : throw exception
    {
      NodeId prior_mean_id = node.Parents()[0];
      NodeId prior_var_id = node.Parents()[1];
      Size dim_node = pGraph_->GetNode(prior_mean_id).Dim()[0];

      StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
      boost::tie(it_offspring, it_offspring_end) = pGraph_->GetStochasticChildren(nodeId_);

      MNormalVarLinearLikeFormVisitor like_form_vis(pGraph_, nodeId_, this, dim_node);
      while ( it_offspring != it_offspring_end )
      {
        pGraph_->VisitNode(*it_offspring, like_form_vis);
        ++it_offspring;
      }

      const Matrix & like_A = like_form_vis.GetA();
      const Vector & like_b = like_form_vis.GetB();
      const Matrix & like_cov = like_form_vis.GetCov();
      const Vector & obs = like_form_vis.GetObs();

      DataType prior_var_dat(getNodeValue(prior_var_id, pGraph_, this));
      MatrixRef prior_var(prior_var_dat);

      Matrix kalman_gain = ublas::prod(prior_var, ublas::trans(like_A));
      Matrix inn_cov = ublas::prod(like_A, kalman_gain) + like_cov;
      Matrix inn_cov_inv = inn_cov;
      ublas::cholesky_factorize(inn_cov_inv);
      ublas::cholesky_invert(inn_cov_inv);
      kalman_gain = ublas::prod(kalman_gain, inn_cov_inv);

      DataType prior_mean_dat(getNodeValue(prior_mean_id, pGraph_, this));
      VectorRef prior_mean(prior_mean_dat);

      Vector obs_pred = ublas::prod(like_A, prior_mean) + like_b;
      Vector post_mean = prior_mean + ublas::prod(kalman_gain, (obs - obs_pred));
      prior_mean.Release();

      Matrix post_var = ublas::prod(Matrix(ublas::identity_matrix<Scalar>(dim_node, dim_node) - Matrix(ublas::prod(kalman_gain, like_A))), prior_var);
      prior_var.Release();

      DataType::Array post_param_values(2);
      post_param_values[0] = DataType(post_mean);
      post_param_values[1] = DataType(post_var);
      nodeValuesMap_[nodeId_] = DMNormVar::Instance()->Sample(post_param_values, pRng_).ValuesPtr(); // TODO GenerateValue( Numerical::Array, Rng ) to avoid use of pointer function

      DataType::Array norm_const_param_values(2);
      norm_const_param_values[0] = DataType(obs_pred);
      norm_const_param_values[1] = DataType(inn_cov);
      logWeight_ = DMNormVar::Instance()->LogUnnormPdf(DataType(obs), norm_const_param_values); // TODO LogPdf( Numerical::Array, Numerical ) to avoid use of pointer function
      // TODO optimize computation removing constant terms

      sampledFlagsMap_[nodeId_] = true;
    }
  }


  class IsConjugateMNormalVarLinearVisitor : public ConstNodeVisitor
  {
  protected:
    const Graph * pGraph_;
    const NodeId myId_;
    Bool conjugate_;

  public:
    void Visit(const ConstantNode & node) {}; // TODO throw exception
    void Visit(const LogicalNode & node) {}; // TODO throw exception

    void Visit(const StochasticNode & node)
    {
      if ( nodeIdDefined_ )
      {
        conjugate_ = false;
        if ( node.PriorName() == "dmnorm.var" )
        {
          NodeId mean_id = node.Parents()[0];
          NodeId var_id = node.Parents()[1];
          conjugate_ = ( (nodesRelation(var_id, myId_, pGraph_) == KNOWN )
              && isLinear(mean_id, myId_, pGraph_) ) ? true : false;
        }
      }
    };

    Bool IsConjugate() const { return conjugate_; }

    IsConjugateMNormalVarLinearVisitor(const Graph * pGraph, NodeId myId) : pGraph_(pGraph), myId_(myId), conjugate_(false) {}
  };



  class CanSampleMNormalVarLinearVisitor : public ConstNodeVisitor
  {
  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;

    const Graph * pGraph_;
    Bool canSample_;

  public:
    void Visit(const ConstantNode & node) {}; // TODO throw exception
    void Visit(const LogicalNode & node) {}; // TODO throw exception

    void Visit(const StochasticNode & node)
    {
      if ( nodeIdDefined_ )
      {
        canSample_ = false;
        if ( !pGraph_->GetObserved()[nodeId_] ) // TODO throw exception
        {
          if ( node.PriorName() == "dmnorm.var" )
          {
            StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
            boost::tie(it_offspring, it_offspring_end) = pGraph_->GetStochasticChildren(nodeId_);

            IsConjugateMNormalVarLinearVisitor child_vis(pGraph_, nodeId_);

            while ( it_offspring != it_offspring_end )
            {
              if ( pGraph_->GetObserved()[*it_offspring] )
              {
                pGraph_->VisitNode(*it_offspring, child_vis);
                if ( child_vis.IsConjugate() )
                {
                  canSample_ = true;
                }
                else
                {
                  canSample_ = false;
                  break;
                }
              }
              ++it_offspring;
            }
          }
        }
      }
    };

    Bool CanSample() const { return canSample_; }

    CanSampleMNormalVarLinearVisitor(const Graph * pGraph) : pGraph_(pGraph), canSample_(false) {}
  };



  Bool ConjugateMNormalVarLinearFactory::Create(const Graph * pGraph, NodeId nodeId, BaseType::CreatedPtr & pNodeSamplerInstance) const
  {
    CanSampleMNormalVarLinearVisitor can_sample_vis(pGraph);

    pGraph->VisitNode(nodeId, can_sample_vis);

    Bool flag_created = can_sample_vis.CanSample();

    if ( flag_created )
    {
      pNodeSamplerInstance = NodeSamplerFactory::CreatedPtr(new CreatedType(pGraph));
    }

    return flag_created;
  }


  ConjugateMNormalVarLinearFactory::Ptr ConjugateMNormalVarLinearFactory::pConjugateMNormalVarLinearFactoryInstance_(new ConjugateMNormalVarLinearFactory());


}
