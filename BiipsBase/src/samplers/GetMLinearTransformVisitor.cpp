//                                               -*- C++ -*-
/*! \file GetMLinearTransformVisitor.cpp
* \brief
*
* $LastChangedBy$
* $LastChangedDate$
* $LastChangedRevision$
* $Id$
*/

#include "samplers/GetMLinearTransformVisitor.hpp"

#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "sampler/NodesRelationVisitor.hpp"
#include "samplers/IsLinearVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"

namespace Biips
{

  void GetMLinearTransformVisitor::Visit(const StochasticNode & node)
  {
    if ( nodeIdDefined_ ) // TODO manage else case : throw exception
    {
      if (nodeId_ == myId_)
      {
        A_ = ublas::identity_matrix<Scalar>(dimObs_, dimNode_);
        b_ = ublas::zero_vector<Scalar>(dimObs_);
      }
    }
  };

  void GetMLinearTransformVisitor::Visit(const LogicalNode & node) // TODO optimize (using effective uBlas functions)
  {
    if ( nodeIdDefined_ ) // TODO manage else case : throw exception
    {
      switch ( IsLinearVisitor::LinearFuncMap().find(node.FuncName())->second )
      {
        case IsLinearVisitor::MULTIPLY:
        {
          Scalar mult_const = 1.0;

          for (Size i=0; i<node.Parents().size(); ++i)
          {
            NodeId operand_id = node.Parents()[i];
            switch(nodesRelation(operand_id, myId_, pGraph_))
            {
              case KNOWN:
              {
                Scalar op_val = getNodeValue(operand_id, pGraph_,
                    pNodeSampler_).ScalarView();
                mult_const *= op_val;
                break;
              }
              case DEPENDING:
              {
                SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_, dimNode_, dimObs_);
                pGraph_->VisitNode(operand_id, get_lin_trans_vis);
                A_ = get_lin_trans_vis.GetA();
                b_ = get_lin_trans_vis.GetB();
                break;
              }
              default:
                break;
            }
          }

          A_ *= mult_const;
          b_ *= mult_const;

          break;
        }

        case IsLinearVisitor::ADD:
        {
          for (Size i=0; i<node.Parents().size(); ++i)
          {
            NodeId operand_id = node.Parents()[i];
            switch(nodesRelation(operand_id, myId_, pGraph_))
            {
              case KNOWN:
              {
                if (pGraph_->GetNode(operand_id).Dim().IsScalar())
                {
                  Scalar op_val = getNodeValue(operand_id, pGraph_, pNodeSampler_).ScalarView();
                  b_ += ublas::scalar_vector<Scalar>(b_.size(), op_val);
                }
                else
                  b_ += getNodeValue(operand_id, pGraph_, pNodeSampler_).VectorView();
                break;
              }
              case DEPENDING:
              {
                SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_, dimNode_, dimObs_);
                pGraph_->VisitNode(operand_id, get_lin_trans_vis);
                A_ += get_lin_trans_vis.GetA();
                b_ += get_lin_trans_vis.GetB();
                break;
              }
              default:
                break;
            }
          }
          break;
        }

        case IsLinearVisitor::DIVIDE:
        {
          NodeId left_operand_id = node.Parents()[0];
          NodeId right_operand_id = node.Parents()[1];

          SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_, dimNode_, dimObs_);
          pGraph_->VisitNode(left_operand_id, get_lin_trans_vis);
          Scalar right_op_val = getNodeValue(right_operand_id, pGraph_,
              pNodeSampler_).ScalarView();

          A_ = get_lin_trans_vis.GetA() / right_op_val;
          b_ = get_lin_trans_vis.GetB() / right_op_val;
          break;
        }

        case IsLinearVisitor::SUBSTRACT:
        {
          NodeId left_operand_id = node.Parents()[0];
          switch(nodesRelation(left_operand_id, myId_, pGraph_))
          {
            case KNOWN:
            {
              if (pGraph_->GetNode(left_operand_id).Dim().IsScalar())
              {
                Scalar l_op_val = getNodeValue(left_operand_id, pGraph_,
                    pNodeSampler_).ScalarView();
                b_ = ublas::scalar_vector<Scalar>(b_.size(), l_op_val);
              }
              else
                b_ = getNodeValue(left_operand_id, pGraph_,
                    pNodeSampler_).VectorView();
              break;
            }
            case DEPENDING:
            {
              SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_, dimNode_, dimObs_);
              pGraph_->VisitNode(left_operand_id, get_lin_trans_vis);
              A_ = get_lin_trans_vis.GetA();
              b_ = get_lin_trans_vis.GetB();
              break;
            }
            default:
              break;
          }

          NodeId right_operand_id = node.Parents()[1];
          switch(nodesRelation(right_operand_id, myId_, pGraph_))
          {
            case KNOWN:
            {
              if (pGraph_->GetNode(right_operand_id).Dim().IsScalar())
              {
                Scalar r_op_val = getNodeValue(right_operand_id, pGraph_,
                    pNodeSampler_).ScalarView();
                b_ -= ublas::scalar_vector<Scalar>(b_.size(), r_op_val);
              }
              else
                b_ -= getNodeValue(left_operand_id, pGraph_,
                    pNodeSampler_).VectorView();
              break;
            }
            case DEPENDING:
            {
              SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_, dimNode_, dimObs_);
              pGraph_->VisitNode(right_operand_id, get_lin_trans_vis);
              A_ -= get_lin_trans_vis.GetA();
              b_ -= get_lin_trans_vis.GetB();
              break;
            }
            default:
              break;
          }
          break;
        }

        case IsLinearVisitor::MAT_MULT:
        {
          NodeId left_operand_id = node.Parents()[0];
          MultiArray l_op_val_marray = getNodeValue(left_operand_id, pGraph_, pNodeSampler_);
          MatrixRef l_op_val(l_op_val_marray);

          NodeId right_operand_id = node.Parents()[1];
          SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_, dimNode_, l_op_val.size2());
          pGraph_->VisitNode(right_operand_id, get_lin_trans_vis);

          A_ = ublas::prod(l_op_val, get_lin_trans_vis.GetA());
          b_ = ublas::prod(l_op_val, get_lin_trans_vis.GetB());

          break;
        }

        case IsLinearVisitor::NEG:
        {
          NodeId operand_id = node.Parents()[0];
          SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_, dimNode_, dimObs_);
          pGraph_->VisitNode(operand_id, get_lin_trans_vis);
          A_ = -get_lin_trans_vis.GetA();
          b_ = -get_lin_trans_vis.GetB();
          break;
        }

        default:
          break;
      }
    }
  };

  GetMLinearTransformVisitor::GetMLinearTransformVisitor(const Graph * pGraph, NodeId myId, NodeSampler * pSampleNodeVis, Size dimNode, Size dimObs) // TODO manage dimension
  : pGraph_(pGraph), myId_(myId), pNodeSampler_(pSampleNodeVis),
    dimNode_(dimNode), dimObs_(dimObs),
    A_(ublas::zero_matrix<Scalar>(dimObs, dimNode)),
    b_(ublas::zero_vector<Scalar>(dimObs)) {};



}
