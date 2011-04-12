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

  public:
    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_NEG_HPP_ */
