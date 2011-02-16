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
            NodeId left_operand_id = node.Parents()[0];
            NodeId right_operand_id = node.Parents()[1];
            NodesRelationType left_op_relation = nodesRelation(left_operand_id, myId_, pGraph_);
            NodesRelationType right_op_relation = nodesRelation(right_operand_id, myId_, pGraph_);

            if ( (left_op_relation == DEPENDING) && (right_op_relation == KNOWN) )
            {
              SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_, dimNode_, dimObs_);
              pGraph_->VisitNode(left_operand_id, get_lin_trans_vis);

              // TODO assert known_op_id is scalar
              Scalar op_val = getNodeValue(right_operand_id, pGraph_, pNodeSampler_).ScalarView();
              A_ = op_val * get_lin_trans_vis.GetA();
              b_ = op_val * get_lin_trans_vis.GetB();
            }
            else if ( (left_op_relation == KNOWN) && (right_op_relation == DEPENDING) )
            {
              SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_, dimNode_, dimObs_);
              pGraph_->VisitNode(right_operand_id, get_lin_trans_vis);

              // TODO assert known_op_id is scalar
              Scalar op_val = getNodeValue(left_operand_id, pGraph_, pNodeSampler_).ScalarView();
              A_ = op_val * get_lin_trans_vis.GetA();
              b_ = op_val * get_lin_trans_vis.GetB();
            }
            // TODO manage else case

            break;
          }

          case IsLinearVisitor::ADD:
          {
            NodeId left_operand_id = node.Parents()[0];
            NodeId right_operand_id = node.Parents()[1];

            NodesRelationType left_op_relation = nodesRelation(left_operand_id, myId_, pGraph_);
            NodesRelationType right_op_relation = nodesRelation(right_operand_id, myId_, pGraph_);

            if ( (left_op_relation == DEPENDING) && (right_op_relation == KNOWN) )
            {
              SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_, dimNode_, dimObs_);
              pGraph_->VisitNode(left_operand_id, get_lin_trans_vis);

              A_ = get_lin_trans_vis.GetA();
              DataType r_op_val_dat(getNodeValue(right_operand_id, pGraph_, pNodeSampler_));
              VectorRef r_op_val(r_op_val_dat);
              b_ = get_lin_trans_vis.GetB() + r_op_val;
            }
            else if ( (left_op_relation == KNOWN) && (right_op_relation == DEPENDING) )
            {
              SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_, dimNode_, dimObs_);
              pGraph_->VisitNode(right_operand_id, get_lin_trans_vis);

              A_ = get_lin_trans_vis.GetA();
              DataType l_op_val_dat(getNodeValue(right_operand_id, pGraph_, pNodeSampler_));
              VectorRef l_op_val(l_op_val_dat);
              b_ = l_op_val + get_lin_trans_vis.GetB();
            }
            else if ( (left_op_relation == DEPENDING) && (right_op_relation == DEPENDING) )
            {
              SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_, dimNode_, dimObs_);

              pGraph_->VisitNode(left_operand_id, get_lin_trans_vis);
              A_ = get_lin_trans_vis.GetA();
              b_ = get_lin_trans_vis.GetB();

              pGraph_->VisitNode(right_operand_id, get_lin_trans_vis);
              A_ += get_lin_trans_vis.GetA();
              b_ += get_lin_trans_vis.GetB();
            }
            // TODO manage else case : we assume at least one operand is depending only on myId and the other is not unknown since linearity was checked before

            break;
          }

          case IsLinearVisitor::MAT_MULT:
          {
            NodeId left_operand_id = node.Parents()[0];
            NodeId right_operand_id = node.Parents()[1];
            NodesRelationType left_op_relation = nodesRelation(left_operand_id, myId_, pGraph_);
            NodesRelationType right_op_relation = nodesRelation(right_operand_id, myId_, pGraph_);

            if ( (left_op_relation == KNOWN) && (right_op_relation == DEPENDING) )
            {
              DataType l_op_val_dat(getNodeValue(left_operand_id, pGraph_, pNodeSampler_));
              MatrixRef l_op_val(l_op_val_dat);

              SelfType get_lin_trans_vis(pGraph_, myId_, pNodeSampler_, dimNode_, l_op_val.size2());
              pGraph_->VisitNode(right_operand_id, get_lin_trans_vis);

              A_ = ublas::prod(l_op_val, get_lin_trans_vis.GetA());
              b_ = ublas::prod(l_op_val, get_lin_trans_vis.GetB());
            }
            // TODO manage else case

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
