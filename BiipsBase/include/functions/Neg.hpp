//                                               -*- C++ -*-
/*! \file Neg.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_NEG_HPP_
#define BIIPS_NEG_HPP_

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  class Neg : public UnaryScalarFunction<std::negate<Scalar> >
  {
  public:
    typedef Neg SelfType;
    typedef UnaryScalarFunction<std::negate<Scalar> > BaseType;

  protected:
    Neg() : BaseType("NEG") {};

    virtual Bool checkParamValues(const MultiArray::Array & paramValues) const { return true; }

  public:
    virtual Bool IsPrefix() const { return true; }
    virtual Bool IsDiscreteValued(const Flags & mask) const;
    virtual Bool IsScale(const Flags & scaleMask, const Flags & knownMask) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_NEG_HPP_ */
