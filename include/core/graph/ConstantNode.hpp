#ifndef BIIPS_CONSTANTNODE_HPP_
#define BIIPS_CONSTANTNODE_HPP_

#include "Node.hpp"

namespace Biips
{
  class ConstantNode: public Node
  {
  public:
    typedef ConstantNode SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef Types<SelfType>::IteratorPair IteratorPair;

    explicit ConstantNode(const DimArray::Ptr & pDim) :
      Node(CONSTANT, pDim)
    {
    }
  };

}

#endif /* BIIPS_CONSTANTNODE_HPP_ */
