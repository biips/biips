//                                               -*- C++ -*-
/*! \file LogLikeVisitor.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_LOGLIKEVISITOR_HPP_
#define BIIPS_LOGLIKEVISITOR_HPP_

#include "common/Types.hpp"

namespace Biips
{
  class Graph;
  class NodeSampler;

  Scalar getLogLikelihood(const Graph & graph, NodeId myId, NodeSampler & nodeSampler);
}

#endif /* BIIPS_LOGLIKEVISITOR_HPP_ */
