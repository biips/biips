//                                               -*- C++ -*-
/*! \file IfElse.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_IFELSE_HPP_
#define BIIPS_IFELSE_HPP_

#include "function/Function.hpp"

namespace Biips
{
  class IfElse : public Function
  {
  public:
    typedef Function BaseType;
    typedef IfElse SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    IfElse() : BaseType("ifelse", 3) {};

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual Bool checkParamValues(const MultiArray::Array & paramValues) const { return true; }
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual MultiArray eval(const MultiArray::Array & paramValues) const;

  public:
    virtual Bool IsDiscreteValued(const Flags & mask) const;
    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };


}

#endif /* BIIPS_IFELSE_HPP_ */
