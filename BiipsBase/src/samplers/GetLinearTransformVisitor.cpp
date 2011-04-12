//                                               -*- C++ -*-
/*! \file GetLinearTransformVisitor.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "samplers/GetLinearTransformVisitor.hpp"

#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "sampler/NodesRelationVisitor.hpp"
#include "samplers/IsLinearVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"

namespace Biips
{

    void
    GetLinearTransformVisitor::Visit(const StochasticNode & node)
    {
      if (nodeIdDefined_) // TODO manage else case: throw exception
      {
        if (nodeId_ == myId_)
        {
          a_ = 1.0;
          b_ = 0.0;
        }
      }
    }

    void
    GetLinearTransformVisitor::Visit(const LogicalNode & node)
    {
      if (nodeIdDefined_) // TODO manage else case: throw exception
      {
        switch (IsLinearVisitor::LinearFuncMap().find(node.FuncName())->second)
        {
          case IsLinearVisitor::MULTIPLY:
          {
            a_ = 1.0;
            b_ = 1.0;

            for (Size i=0; i<node.Parents().size(); ++i)
            {
              NodeId operand_id = node.Parents()[i];
              switch(nodesRelation(operand_id, myId_, pGraph_))
              {
                case KNOWN:
                {
                  Scalar op_val = getNodeValue(operand_id, pGraph_,
                      pNodeSampler_).ScalarView();
                  a_ *= op_val;
                  b_ *= op_val;
                  break;
                }
                case DEPENDING:
                {
                  SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_);
                  pGraph_->VisitNode(operand_id, get_lin_trans_vis);
                  a_ *= get_lin_trans_vis.GetA();
                  b_ *= get_lin_trans_vis.GetB();
                  break;
                }
                default:
                  break;
              }
            }
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
                  Scalar op_val = getNodeValue(operand_id, pGraph_,
                      pNodeSampler_).ScalarView();
                  b_ += op_val;
                  break;
                }
                case DEPENDING:
                {
                  SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_);
                  pGraph_->VisitNode(operand_id, get_lin_trans_vis);
                  a_ += get_lin_trans_vis.GetA();
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

            SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_);
            pGraph_->VisitNode(left_operand_id, get_lin_trans_vis);
            Scalar right_op_val = getNodeValue(right_operand_id, pGraph_,
                pNodeSampler_).ScalarView();

            a_ = get_lin_trans_vis.GetA() / right_op_val;
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
                Scalar op_val = getNodeValue(left_operand_id, pGraph_,
                    pNodeSampler_).ScalarView();
                a_ = 0.0;
                b_ = op_val;
                break;
              }
              case DEPENDING:
              {
                SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_);
                pGraph_->VisitNode(left_operand_id, get_lin_trans_vis);
                a_ = get_lin_trans_vis.GetA();
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
                Scalar op_val = getNodeValue(right_operand_id, pGraph_,
                    pNodeSampler_).ScalarView();
                b_ -= op_val;
                break;
              }
              case DEPENDING:
              {
                SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_);
                pGraph_->VisitNode(right_operand_id, get_lin_trans_vis);
                a_ -= get_lin_trans_vis.GetA();
                b_ -= get_lin_trans_vis.GetB();
                break;
              }
              default:
                break;
            }
            break;
          }

          case IsLinearVisitor::NEG:
          {
            NodeId operand_id = node.Parents()[0];
            SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_);
            pGraph_->VisitNode(operand_id, get_lin_trans_vis);
            a_ = -get_lin_trans_vis.GetA();
            b_ = -get_lin_trans_vis.GetB();
            break;
          }

          default:
            break;
        }
      }
    }


    GetLinearTransformVisitor::GetLinearTransformVisitor(const Graph * pGraph, NodeId myId,
        NodeSampler * pNodeSampler) :
      pGraph_(pGraph), myId_(myId), pNodeSampler_(pNodeSampler), a_(0.0), b_(
          0.0)
    {
    }
    ;

}
