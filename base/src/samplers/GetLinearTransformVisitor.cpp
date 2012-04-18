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

/*! \file GetLinearTransformVisitor.cpp
* \brief
*
* \author  $LastChangedBy$
* \date    $LastChangedDate$
* \version $LastChangedRevision$
* Id:      $Id$
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

  void GetLinearTransformVisitor::visit(const StochasticNode & node)
  {
    if (nodeId_ == myId_)
    {
      a_ = 1.0;
      b_ = 0.0;
    }
  }


  void GetLinearTransformVisitor::visit(const LogicalNode & node)
  {
    switch (linearFuncMap().find(node.FuncName())->second)
    {
      case MULTIPLY:
      {
        a_ = 1.0;
        b_ = 1.0;

        for (Size i=0; i<node.Parents().size(); ++i)
        {
          NodeId operand_id = node.Parents()[i];
          switch(nodesRelation(operand_id, myId_, graph_))
          {
            case KNOWN:
            {
              Scalar op_val = getNodeValue(operand_id, graph_,
                  nodeSampler_).ScalarView();
              a_ *= op_val;
              b_ *= op_val;
              break;
            }
            case DEPENDING:
            {
              SelfType get_lin_trans_vis(graph_, myId_, nodeSampler_);
              graph_.VisitNode(operand_id, get_lin_trans_vis);
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

      case ADD:
      {
        for (Size i=0; i<node.Parents().size(); ++i)
        {
          NodeId operand_id = node.Parents()[i];
          switch(nodesRelation(operand_id, myId_, graph_))
          {
            case KNOWN:
            {
              Scalar op_val = getNodeValue(operand_id, graph_,
                  nodeSampler_).ScalarView();
              b_ += op_val;
              break;
            }
            case DEPENDING:
            {
              SelfType get_lin_trans_vis(graph_, myId_, nodeSampler_);
              graph_.VisitNode(operand_id, get_lin_trans_vis);
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

      case DIVIDE:
      {
        NodeId left_operand_id = node.Parents()[0];
        NodeId right_operand_id = node.Parents()[1];

        SelfType get_lin_trans_vis(graph_, myId_, nodeSampler_);
        graph_.VisitNode(left_operand_id, get_lin_trans_vis);
        Scalar right_op_val = getNodeValue(right_operand_id, graph_,
            nodeSampler_).ScalarView();

        a_ = get_lin_trans_vis.GetA() / right_op_val;
        b_ = get_lin_trans_vis.GetB() / right_op_val;
        break;
      }

      case SUBSTRACT:
      {
        NodeId left_operand_id = node.Parents()[0];
        switch(nodesRelation(left_operand_id, myId_, graph_))
        {
          case KNOWN:
          {
            Scalar op_val = getNodeValue(left_operand_id, graph_,
                nodeSampler_).ScalarView();
            a_ = 0.0;
            b_ = op_val;
            break;
          }
          case DEPENDING:
          {
            SelfType get_lin_trans_vis(graph_, myId_, nodeSampler_);
            graph_.VisitNode(left_operand_id, get_lin_trans_vis);
            a_ = get_lin_trans_vis.GetA();
            b_ = get_lin_trans_vis.GetB();
            break;
          }
          default:
            break;
        }

        NodeId right_operand_id = node.Parents()[1];
        switch(nodesRelation(right_operand_id, myId_, graph_))
        {
          case KNOWN:
          {
            Scalar op_val = getNodeValue(right_operand_id, graph_,
                nodeSampler_).ScalarView();
            b_ -= op_val;
            break;
          }
          case DEPENDING:
          {
            SelfType get_lin_trans_vis(graph_, myId_, nodeSampler_);
            graph_.VisitNode(right_operand_id, get_lin_trans_vis);
            a_ -= get_lin_trans_vis.GetA();
            b_ -= get_lin_trans_vis.GetB();
            break;
          }
          default:
            break;
        }
        break;
      }

      case NEG:
      {
        NodeId operand_id = node.Parents()[0];
        SelfType get_lin_trans_vis(graph_, myId_, nodeSampler_);
        graph_.VisitNode(operand_id, get_lin_trans_vis);
        a_ = -get_lin_trans_vis.GetA();
        b_ = -get_lin_trans_vis.GetB();
        break;
      }

      default:
        break;
    }
  }


  GetLinearTransformVisitor::GetLinearTransformVisitor(const Graph & graph, NodeId myId, NodeSampler & nodeSampler)
    : graph_(graph), myId_(myId), nodeSampler_(nodeSampler), a_(0.0), b_(0.0)
  {

  }

}
