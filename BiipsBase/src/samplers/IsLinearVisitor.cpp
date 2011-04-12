//                                               -*- C++ -*-
/*! \file IsLinearVisitor.cpp
* \brief
*
* \author  $LastChangedBy$
* \date    $LastChangedDate$
* \version $LastChangedRevision$
* Id:      $Id$
*/

#include "samplers/IsLinearVisitor.hpp"
#include "graph/Graph.hpp"
#include "sampler/NodesRelationVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"
#include "functions/Multiply.hpp"
#include "functions/Add.hpp"
#include "functions/MatMult.hpp"
#include "functions/Substract.hpp"
#include "functions/Divide.hpp"
#include "functions/Neg.hpp"

namespace Biips
{

  const std::map<String, IsLinearVisitor::LinearFuncType> & IsLinearVisitor::LinearFuncMap()
  {
    static std::map<String, LinearFuncType> linearFuncMap;
    if ( linearFuncMap.empty() )
    {
      linearFuncMap[Multiply::Instance()->Name()] = MULTIPLY;
      linearFuncMap[Add::Instance()->Name()] = ADD;
      linearFuncMap[MatMult::Instance()->Name()] = MAT_MULT;
      linearFuncMap[Substract::Instance()->Name()] = SUBSTRACT;
      linearFuncMap[Divide::Instance()->Name()] = DIVIDE;
      linearFuncMap[Neg::Instance()->Name()] = NEG;
    }
    return linearFuncMap;
  }


  void IsLinearVisitor::Visit(const StochasticNode & node)
  {
    if ( nodeIdDefined_ ) // TODO manage else case : throw exception
    {
      if ( nodeId_ == myId_ )
        ans_ = true;
      else
        ans_ = false;
    }
  }


  void IsLinearVisitor::Visit(const LogicalNode & node)
  {
    ans_ = false;
    std::map<String, IsLinearVisitor::LinearFuncType>::const_iterator it_func_type = LinearFuncMap().find(node.FuncName());
    if ( it_func_type != LinearFuncMap().end() )
    {
      switch ( it_func_type->second )
      {
        case MULTIPLY:
        {
          // Exactly one parent must be DEPENDING and linear.
          // the other parents must be KNOWN and scalar
          ans_ = true;
          Bool depending_is_found = false;
          for (Size i=0; i<node.Parents().size() && ans_; ++i)
          {
            NodeId operand_id = node.Parents()[i];
            switch( nodesRelation(operand_id, myId_, pGraph_) )
            {
              case KNOWN:
              {
                ans_ = pGraph_->GetNode(operand_id).Dim().IsScalar();
                break;
              }
              case DEPENDING:
              {
                if (!depending_is_found)
                {
                  depending_is_found = true;
                  SelfType op_is_linear_vis(pGraph_, myId_);
                  pGraph_->VisitNode(operand_id, op_is_linear_vis);
                  ans_ = op_is_linear_vis.IsLinear();
                }
                else
                  ans_ = false;
                break;
              }
              default:
                ans_ = false;
                break;
            }
          }

          if (!depending_is_found)
            ans_ = false;

          break;
        }

        case ADD:
        {
          // Parents must be KNOWN or DEPENDING and linear.
          // At least one parent must be DEPENDING and linear.
          ans_ = true;
          Bool depending_is_found = false;
          for (Size i=0; i<node.Parents().size() && ans_; ++i)
          {
            NodeId operand_id = node.Parents()[i];
            switch( nodesRelation(operand_id, myId_, pGraph_) )
            {
              case KNOWN:
              {
                break;
              }
              case DEPENDING:
              {
                depending_is_found = true;
                SelfType op_is_linear_vis(pGraph_, myId_);
                pGraph_->VisitNode(operand_id, op_is_linear_vis);
                ans_ = op_is_linear_vis.IsLinear();
                break;
              }
              default:
                ans_ = false;
                break;
            }
          }

          if (!depending_is_found)
            ans_ = false;

          break;
        }

        case SUBSTRACT:
        {
          NodeId left_operand_id = node.Parents()[0];
          NodeId right_operand_id = node.Parents()[1];
          switch( nodesRelation(left_operand_id, myId_, pGraph_) )
          {
            case KNOWN:
            {
              SelfType right_op_is_linear_vis(pGraph_, myId_);
              pGraph_->VisitNode(right_operand_id, right_op_is_linear_vis);
              ans_ = right_op_is_linear_vis.IsLinear();
              break;
            }
            case DEPENDING:
            {
              SelfType op_is_linear_vis(pGraph_, myId_);
              pGraph_->VisitNode(left_operand_id, op_is_linear_vis);
              if ( op_is_linear_vis.IsLinear() )
              {
                if ( nodesRelation(right_operand_id, myId_, pGraph_) == KNOWN  )
                  ans_ = true;
                else
                {
                  pGraph_->VisitNode(right_operand_id, op_is_linear_vis);
                  ans_ = op_is_linear_vis.IsLinear();
                }
              }
              break;
            }
            default:
              ans_ = false;
              break;
          }
          break;
        }

        case MAT_MULT:
        {
          NodeId left_operand_id = node.Parents()[0];
          NodeId right_operand_id = node.Parents()[1];
          if ( nodesRelation(left_operand_id, myId_, pGraph_) == KNOWN )
          {
            SelfType right_op_is_linear_vis(pGraph_, myId_);
            pGraph_->VisitNode(right_operand_id, right_op_is_linear_vis);
            ans_ = right_op_is_linear_vis.IsLinear();
          }
          else
            ans_ = false;
          break;
        }

        case DIVIDE:
        {
          NodeId left_operand_id = node.Parents()[0];
          NodeId right_operand_id = node.Parents()[1];
          if ( nodesRelation(right_operand_id, myId_, pGraph_) == KNOWN )
          {
            if ( pGraph_->GetNode(right_operand_id).Dim().IsScalar() )
            {
              SelfType left_op_is_linear_vis(pGraph_, myId_);
              pGraph_->VisitNode(left_operand_id, left_op_is_linear_vis);
              ans_ = left_op_is_linear_vis.IsLinear();
            }
            else
              ans_ = false;
          }
          else
            ans_ = false;
          break;
        }

        case NEG:
        {
          NodeId operand_id = node.Parents()[0];
          SelfType op_is_linear_vis(pGraph_, myId_);
          pGraph_->VisitNode(operand_id, op_is_linear_vis);
          ans_ = op_is_linear_vis.IsLinear();
          break;
        }

        default:
          ans_ = false;
          break;
      }
    }
  }


  // nodeB is expected to be a StochasticNode from the nodeSequence
  Bool isLinear(NodeId nodeA, NodeId nodeB, const Graph * pGraph)
  {
    IsLinearVisitor vis(pGraph, nodeB);
    pGraph->VisitNode(nodeA, vis);
    return vis.IsLinear();
  }


}
