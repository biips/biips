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

/*! \file GetMLinearTransformVisitor.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
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

  void GetMLinearTransformVisitor::visit(const StochasticNode & node)
  {
    if (nodeId_ == myId_)
    {
      A_ = ublas::identity_matrix<Scalar>(dimObs_, dimNode_);
      b_ = ublas::zero_vector<Scalar>(dimObs_);
    }
  }

  void GetMLinearTransformVisitor::visit(const LogicalNode & node) // TODO optimize (using effective uBlas functions)
  {
    switch (linearFuncMap().find(node.FuncName())->second)
    {
      case MULTIPLY:
      {
        Scalar mult_const = 1.0;

        for (Size i = 0; i < node.Parents().size(); ++i)
        {
          NodeId operand_id = node.Parents()[i];
          switch (nodesRelation(operand_id, myId_, graph_))
          {
            case KNOWN:
            {
              Scalar op_val =
                  getNodeValue(operand_id, graph_, nodeSampler_).ScalarView();
              mult_const *= op_val;
              break;
            }
            case DEPENDING:
            {
              SelfType get_lin_trans_vis(graph_,
                                         myId_,
                                         nodeSampler_,
                                         dimNode_,
                                         dimObs_);
              graph_.VisitNode(operand_id, get_lin_trans_vis);
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

      case ADD:
      {
        for (Size i = 0; i < node.Parents().size(); ++i)
        {
          NodeId operand_id = node.Parents()[i];
          switch (nodesRelation(operand_id, myId_, graph_))
          {
            case KNOWN:
            {
              if (graph_.GetNode(operand_id).Dim().IsScalar())
              {
                Scalar op_val =
                    getNodeValue(operand_id, graph_, nodeSampler_).ScalarView();
                b_ += ublas::scalar_vector<Scalar>(b_.size(), op_val);
              }
              else
              {
                NumArray op_val =
                    getNodeValue(operand_id, graph_, nodeSampler_);
                b_ += VectorRef(op_val);
              }
              break;
            }
            case DEPENDING:
            {
              SelfType get_lin_trans_vis(graph_,
                                         myId_,
                                         nodeSampler_,
                                         dimNode_,
                                         dimObs_);
              graph_.VisitNode(operand_id, get_lin_trans_vis);
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

      case DIVIDE:
      {
        NodeId left_operand_id = node.Parents()[0];
        NodeId right_operand_id = node.Parents()[1];

        SelfType get_lin_trans_vis(graph_,
                                   myId_,
                                   nodeSampler_,
                                   dimNode_,
                                   dimObs_);
        graph_.VisitNode(left_operand_id, get_lin_trans_vis);
        Scalar right_op_val = getNodeValue(right_operand_id,
                                           graph_,
                                           nodeSampler_).ScalarView();

        A_ = get_lin_trans_vis.GetA() / right_op_val;
        b_ = get_lin_trans_vis.GetB() / right_op_val;
        break;
      }

      case SUBSTRACT:
      {
        NodeId left_operand_id = node.Parents()[0];
        switch (nodesRelation(left_operand_id, myId_, graph_))
        {
          case KNOWN:
          {
            if (graph_.GetNode(left_operand_id).Dim().IsScalar())
            {
              Scalar l_op_val = getNodeValue(left_operand_id,
                                             graph_,
                                             nodeSampler_).ScalarView();
              b_ = ublas::scalar_vector<Scalar>(b_.size(), l_op_val);
            }
            else
            {
              NumArray l_op_val = getNodeValue(left_operand_id,
                                               graph_,
                                               nodeSampler_);
              b_ = VectorRef(l_op_val);
            }
            break;
          }
          case DEPENDING:
          {
            SelfType get_lin_trans_vis(graph_,
                                       myId_,
                                       nodeSampler_,
                                       dimNode_,
                                       dimObs_);
            graph_.VisitNode(left_operand_id, get_lin_trans_vis);
            A_ = get_lin_trans_vis.GetA();
            b_ = get_lin_trans_vis.GetB();
            break;
          }
          default:
            break;
        }

        NodeId right_operand_id = node.Parents()[1];
        switch (nodesRelation(right_operand_id, myId_, graph_))
        {
          case KNOWN:
          {
            if (graph_.GetNode(right_operand_id).Dim().IsScalar())
            {
              Scalar r_op_val = getNodeValue(right_operand_id,
                                             graph_,
                                             nodeSampler_).ScalarView();
              b_ -= ublas::scalar_vector<Scalar>(b_.size(), r_op_val);
            }
            else
            {
              NumArray r_op_val = getNodeValue(left_operand_id,
                                               graph_,
                                               nodeSampler_);
              b_ -= VectorRef(r_op_val);
            }
            break;
          }
          case DEPENDING:
          {
            SelfType get_lin_trans_vis(graph_,
                                       myId_,
                                       nodeSampler_,
                                       dimNode_,
                                       dimObs_);
            graph_.VisitNode(right_operand_id, get_lin_trans_vis);
            A_ -= get_lin_trans_vis.GetA();
            b_ -= get_lin_trans_vis.GetB();
            break;
          }
          default:
            break;
        }
        break;
      }

      case MAT_MULT:
      {
        NodeId left_operand_id = node.Parents()[0];
        NumArray l_op_val_marray = getNodeValue(left_operand_id,
                                                graph_,
                                                nodeSampler_);
        MatrixRef l_op_val(l_op_val_marray);

        NodeId right_operand_id = node.Parents()[1];
        SelfType get_lin_trans_vis(graph_,
                                   myId_,
                                   nodeSampler_,
                                   dimNode_,
                                   l_op_val.size2());
        graph_.VisitNode(right_operand_id, get_lin_trans_vis);

        A_ = ublas::prod(l_op_val, get_lin_trans_vis.GetA());
        b_ = ublas::prod(l_op_val, get_lin_trans_vis.GetB());

        break;
      }

      case NEG:
      {
        NodeId operand_id = node.Parents()[0];
        SelfType get_lin_trans_vis(graph_,
                                   myId_,
                                   nodeSampler_,
                                   dimNode_,
                                   dimObs_);
        graph_.VisitNode(operand_id, get_lin_trans_vis);
        A_ = -get_lin_trans_vis.GetA();
        b_ = -get_lin_trans_vis.GetB();
        break;
      }

      default:
        break;
    }
  }
  ;

  GetMLinearTransformVisitor::GetMLinearTransformVisitor(const Graph & graph,
                                                         NodeId myId,
                                                         NodeSampler & nodeSampler,
                                                         Size dimNode,
                                                         Size dimObs) // TODO manage dimension
  :
    graph_(graph), myId_(myId), nodeSampler_(nodeSampler), dimNode_(dimNode),
        dimObs_(dimObs), A_(ublas::zero_matrix<Scalar>(dimObs, dimNode)),
        b_(ublas::zero_vector<Scalar>(dimObs))
  {

  }

}
