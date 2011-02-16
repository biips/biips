//                                               -*- C++ -*-
/*! \file NodesRelationVisitor.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "sampler/NodesRelationVisitor.hpp"
#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"

namespace Biips
{

  class NodesRelationVisitor : public ConstNodeVisitor
  {
  protected:
    typedef NodesRelationVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef GraphTypes::StochasticParentNodeIdIterator StochasticParentNodeIdIterator;

    const Graph * pGraph_;
    NodeId myId_;
    NodesRelationType ans_;

  public:
    virtual void Visit(const ConstantNode & node)  { ans_ = KNOWN; }; // Every ContantNode is KNOWN.

    virtual void Visit(const StochasticNode & node);

    virtual void Visit(const LogicalNode & node);

    NodesRelationType GetAnswer() const { return ans_; };

    NodesRelationVisitor(const Graph * pGraph, NodeId myId)
    : pGraph_(pGraph), myId_(myId), ans_(UNKNOWN) {};
  };


  void NodesRelationVisitor::Visit(const StochasticNode & node)
  {
    if ( nodeIdDefined_ ) // TODO manage else case : throw exception
    {
      if ( nodeId_ == myId_ )
        ans_ = DEPENDING; // A stochastic node depends on itself
      else if ( pGraph_->GetObserved()[nodeId_] )
        ans_ = KNOWN; // Its value is KNOWN if it is observed
      else
      {
        Types<NodeId>::ConstIterator it_node_id_1, it_node_id_2, it_node_id_end;
        boost::tie(it_node_id_1, it_node_id_end) = pGraph_->GetNodes();
        it_node_id_2 = it_node_id_1;
        if ( std::find(it_node_id_1, it_node_id_end, nodeId_) < std::find(it_node_id_2, it_node_id_end, myId_) )
          ans_ = KNOWN;
        // Its value is KNOWN if it has been computed before, i.e. if nodeID_ is before myId_
        // in the nodes sequence of the SMCSampler object.
        else
          ans_ = UNKNOWN;
        // else its value is UNKNOWN
      }
    }
  };


  void NodesRelationVisitor::Visit(const LogicalNode & node)
  {
    if ( nodeIdDefined_ ) // TODO manage else case : throw exception
    {
      StochasticParentNodeIdIterator it_stoch_parent_id, it_stoch_parent_id_end;
      boost::tie(it_stoch_parent_id, it_stoch_parent_id_end) = pGraph_->GetStochasticParents(nodeId_);
      ans_ = KNOWN; // First consider nodeId_ is KNOWN
      // Then visit its stochastic parents.
      // if one is UNKNOWN, then nodeId_ is UNKNOWN and the loop is stopped
      while ( (ans_ != UNKNOWN) && (it_stoch_parent_id != it_stoch_parent_id_end) )
      {
        SelfType vis(pGraph_, myId_);
        pGraph_->VisitNode(*it_stoch_parent_id, vis);
        switch ( vis.GetAnswer() )
        {
          case DEPENDING:
            ans_ = DEPENDING;
            break;
            // if myId_ depends on one stochastic parent's value, relation is changed to DEPENDING,
            // until other stochastic parent is UNKNOWN
          case UNKNOWN:
            ans_ = UNKNOWN;
            // if one is UNKNOWN, then nodeId_ is UNKNOWN
            break;
          default:
            break;
            // else, i.e. nodeId_ is KNOWN, continue
        }
        ++it_stoch_parent_id;
      }
    }
  };


  // nodeB is expected to be a StochasticNode from the nodeSequence
  NodesRelationType nodesRelation(NodeId nodeA, NodeId nodeB, const Graph * pGraph)
  {
    NodesRelationVisitor vis(pGraph, nodeB);
    pGraph->VisitNode(nodeA, vis);
    return vis.GetAnswer();
  }

}
