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
  
  class Multiply : public VariableScalarFunction<std::multiplies<Scalar> >
  {
  public:
    typedef Multiply SelfType;
    typedef VariableScalarFunction<std::multiplies<Scalar> > BaseType;

  protected:
    Multiply() : BaseType("*") {};

  public:
    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_MULTIPLY_HPP_ */
