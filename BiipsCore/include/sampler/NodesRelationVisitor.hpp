//                                               -*- C++ -*-
/*! \file NodesRelationVisitor.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_NODESRELATIONVISITOR_HPP_
#define BIIPS_NODESRELATIONVISITOR_HPP_

#include "graph/GraphTypes.hpp"

namespace Biips
{
  class Graph;

  //! Nodes relation function
  /*!
   * Answers the question: what is the relation between A and B?
   * For instance, we want to know, when computing nodeB,
   * which node values are known, i.e. have been computed before.
   * This is particularly useful when establishing conjugacy relations
   * between stochastic nodes.
   * @param nodeA we look at nodeA from nodeB point of view.
   * @param nodeB is expected to be a StochasticNode from the nodes sequence of the ForwardSampler object.
   * @param graph the Graph pointer to which we refer.
   * @return KNOWN, DEPENDING, or UNKNOWN
   * \see NodesRelationType
   */
  NodesRelationType nodesRelation(NodeId nodeA, NodeId nodeB, const Graph & graph);

//  Types<NodesRelationType>::Array getParentnodesRelations(NodeId nodeId, const Graph & graph);
}

#endif /* BIIPS_NODESRELATIONVISITOR_HPP_ */
