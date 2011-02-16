//                                               -*- C++ -*-
/*! \file AggNode.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_AGGNODE_HPP_
#define BIIPS_AGGNODE_HPP_

#include "graph/LogicalNode.hpp"

namespace Biips
{

  class AggNode : public LogicalNode
  {
  public:
    typedef AggNode SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    Types<Size>::Array offsets_;

    static const String NAME_;

  public:
    static const String & Name() { return NAME_; }

    virtual const String & FuncName() const { return Name(); };
    virtual DataType Eval(const DataType::Array & paramValues) const;

    AggNode(const DimArray::Ptr pDim, const Types<NodeId>::Array & parameters, const Types<Size>::Array & offsets) : LogicalNode(pDim, parameters), offsets_(offsets) {}
    // TODO check lengths
  };

}

#endif /* BIIPS_AGGNODE_HPP_ */
