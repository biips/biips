//                                               -*- C++ -*-
/*! \file Cos.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_COS_HPP_
#define BIIPS_COS_HPP_

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  struct CosScalar : public std::unary_function<Scalar, Scalar>
  {
    Scalar operator() (Scalar val) const
    {
      return std::cos(val);
    }
  };


  class Cos : public UnaryScalarFunction<CosScalar>
  {
  public:
    typedef Cos SelfType;
    typedef UnaryScalarFunction<CosScalar> BaseType;

  protected:
    Cos() : BaseType("cos") {};

  public:
    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_COS_HPP_ */
