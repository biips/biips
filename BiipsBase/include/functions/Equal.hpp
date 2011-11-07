//                                               -*- C++ -*-
/*! \file Equal.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_EQUAL_HPP_
#define BIIPS_EQUAL_HPP_

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  class Equal : public BinaryScalarFunction<std::equal_to<Scalar> >
  {
  public:
    typedef Equal SelfType;
    typedef BinaryScalarFunction<std::equal_to<Scalar> > BaseType; // FIXME use delta error comparison

  protected:
    Equal() : BaseType("==") {};

    virtual Bool checkParamValues(const MultiArray::Array & paramValues) const { return true; };

  public:
    virtual Bool IsInfix() const { return true; }
    virtual Bool IsDiscreteValued(const Flags & mask) const { return true; }

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_EQUAL_HPP_ */
