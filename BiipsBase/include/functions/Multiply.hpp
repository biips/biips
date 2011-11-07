//                                               -*- C++ -*-
/*! \file Multiply.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_MULTIPLY_HPP_
#define BIIPS_MULTIPLY_HPP_

#include "functions/ScalarFunction.hpp"

namespace Biips
{
  
  // TODO : check that any zero argument produces zero evaluation, cf. JAGS Multiply class

  class Multiply : public VariableScalarFunction<std::multiplies<Scalar> >
  {
  public:
    typedef Multiply SelfType;
    typedef VariableScalarFunction<std::multiplies<Scalar> > BaseType;

  protected:
    Multiply() : BaseType("*") {}
    virtual Bool IsDiscreteValued(const Flags & mask) const;

    virtual Bool checkParamValues(const MultiArray::Array & paramValues) const { return true; }

  public:
    virtual Bool IsInfix() const { return true; }
    virtual Bool IsScale(const Flags & scaleMask, const Flags & knownMask) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}
#endif /* BIIPS_MULTIPLY_HPP_ */
