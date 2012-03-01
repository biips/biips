//                                               -*- C++ -*-
/*! \file DeparseVisitor.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DEPARSEVISITOR_HPP_
#define BIIPS_DEPARSEVISITOR_HPP_

#include "common/Types.hpp"

namespace Biips
{
  class Graph;

  String deparse(NodeId nodeId, const Graph & graph, const Types<String>::Array & parentNames);
}

#endif /* DEPARSEVISITOR_HPP_ */
