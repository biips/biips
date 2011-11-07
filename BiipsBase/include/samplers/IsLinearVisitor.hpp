//                                               -*- C++ -*-
/*! \file IsLinearVisitor.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_ISLINEARVISITOR_HPP_
#define BIIPS_ISLINEARVISITOR_HPP_

#include "common/Types.hpp"
#include <map>

namespace Biips
{
  class Graph;

  enum LinearFuncType
  {
    ADD,
    MULTIPLY,
    MAT_MULT,
    SUBSTRACT,
    DIVIDE,
    NEG
  };


  const std::map<String, LinearFuncType> & linearFuncMap();


  // nodeB is expected to be a StochasticNode from the nodeSequence
  Bool isLinear(NodeId nodeA, NodeId nodeB, const Graph & graph);

  // nodeB is expected to be a StochasticNode from the nodeSequence
  Bool isScale(NodeId nodeA, NodeId nodeB, const Graph & graph);

}

#endif /* BIIPS_CHECKLINEARITYVISITOR_HPP_ */
