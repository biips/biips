//                                               -*- C++ -*-
/*! \file Sqrt.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_SQRT_HPP_
#define BIIPS_SQRT_HPP_

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  struct SqrtScalar : public std::unary_function<Scalar, Scalar>
  {
    Scalar operator() (Scalar val) const
    {
      return std::sqrt(val);
    }
  };

  class Sqrt : public UnaryScalarFunction<SqrtScalar>
  {
  public:
    typedef Sqrt SelfType;
    typedef UnaryScalarFunction<SqrtScalar> BaseType;

  protected:
    Sqrt() : BaseType("sqrt") {};

  public:
    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_SQRT_HPP_ */
