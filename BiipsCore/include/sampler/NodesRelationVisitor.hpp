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

#include "graph/NodeVisitor.hpp"

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
   * @param nodeB is expected to be a StochasticNode from the nodes sequence of the SMCSampler object.
   * @param pGraph the Graph pointer to which we refer.
   * @return KNOWN, DEPENDING, or UNKNOWN
   * \see NodesRelationType
   */
  NodesRelationType nodesRelation(NodeId nodeA, NodeId nodeB, const Graph * pGraph);

}

#endif /* BIIPS_NODESRELATIONVISITOR_HPP_ */
