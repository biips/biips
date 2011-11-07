//                                               -*- C++ -*-
/*! \file ConstantNode.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_CONSTANTNODE_HPP_
#define BIIPS_CONSTANTNODE_HPP_

#include "Node.hpp"

namespace Biips
{
  class ConstantNode : public Node
  {
  public:
    typedef ConstantNode SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef Types<SelfType>::IteratorPair IteratorPair;

    virtual NodeType GetType() const { return CONSTANT; };
    virtual void AcceptVisitor(NodeVisitor & vis);
    virtual void AcceptVisitor(ConstNodeVisitor & vis) const;

    explicit ConstantNode(const DimArray::Ptr & pDim) : Node(pDim) {}
  };

}


#endif /* BIIPS_CONSTANTNODE_HPP_ */
