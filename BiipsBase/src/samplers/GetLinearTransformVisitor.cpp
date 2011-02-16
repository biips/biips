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
      if (nodeIdDefined_) // TODO manage else case : throw exception
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
      if (nodeIdDefined_) // TODO manage else case : throw exception
      {
        switch (IsLinearVisitor::LinearFuncMap().find(node.FuncName())->second)
        {
          case IsLinearVisitor::MULTIPLY:
          {
            NodeId left_operand_id = node.Parents()[0];
            NodeId right_operand_id = node.Parents()[1];
            NodesRelationType left_op_relation = nodesRelation(left_operand_id,
                myId_, pGraph_);
            NodesRelationType right_op_relation = nodesRelation(
                right_operand_id, myId_, pGraph_);

            if ((left_op_relation == DEPENDING) && (right_op_relation == KNOWN))
            {
              SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_);
              pGraph_->VisitNode(left_operand_id, get_lin_trans_vis);

              // TODO assert known_op_id is scalar
              Scalar op_val = getNodeValue(right_operand_id, pGraph_,
                  pNodeSampler_).ScalarView();
              a_ = op_val * get_lin_trans_vis.GetA();
              b_ = op_val * get_lin_trans_vis.GetB();
            }
            else if ((left_op_relation == KNOWN) && (right_op_relation
                == DEPENDING))
            {
              SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_);
              pGraph_->VisitNode(right_operand_id, get_lin_trans_vis);

              // TODO assert known_op_id is scalar
              Scalar op_val = getNodeValue(left_operand_id, pGraph_,
                  pNodeSampler_).ScalarView();
              a_ = op_val * get_lin_trans_vis.GetA();
              b_ = op_val * get_lin_trans_vis.GetB();
            }
            // TODO manage else case

            break;
          }
          case IsLinearVisitor::ADD:
          {
            NodeId left_operand_id = node.Parents()[0];
            NodeId right_operand_id = node.Parents()[1];

            NodesRelationType left_op_relation = nodesRelation(left_operand_id,
                myId_, pGraph_);
            NodesRelationType right_op_relation = nodesRelation(
                right_operand_id, myId_, pGraph_);

            if ((left_op_relation == DEPENDING) && (right_op_relation == KNOWN))
            {
              SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_);
              pGraph_->VisitNode(left_operand_id, get_lin_trans_vis);

              a_ = get_lin_trans_vis.GetA();
              Scalar r_op_val = getNodeValue(right_operand_id, pGraph_,
                  pNodeSampler_).ScalarView();
              b_ = get_lin_trans_vis.GetB() + r_op_val;
            }
            else if ((left_op_relation == KNOWN) && (right_op_relation
                == DEPENDING))
            {
              SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_);
              pGraph_->VisitNode(right_operand_id, get_lin_trans_vis);

              a_ = get_lin_trans_vis.GetA();
              Scalar l_op_val = getNodeValue(right_operand_id, pGraph_,
                  pNodeSampler_).ScalarView();
              b_ = l_op_val + get_lin_trans_vis.GetB();
            }
            else if ((left_op_relation == DEPENDING) && (right_op_relation
                == DEPENDING))
            {
              SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_);

              pGraph_->VisitNode(left_operand_id, get_lin_trans_vis);
              a_ = get_lin_trans_vis.GetA();
              b_ = get_lin_trans_vis.GetB();

              pGraph_->VisitNode(right_operand_id, get_lin_trans_vis);
              a_ += get_lin_trans_vis.GetA();
              b_ += get_lin_trans_vis.GetB();
            }
            // TODO manage else case : we assume at least one operand is depending only on myId and the other is not unknown since linearity was checked before

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
