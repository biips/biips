//                                               -*- C++ -*-
/*! \file Add.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_ADD_HPP_
#define BIIPS_ADD_HPP_

#include "functions/ScalarFunction.hpp"

namespace Biips
{
  
  class Add : public VariableScalarFunction<std::plus<Scalar> >
  {
  public:
    typedef Add SelfType;
    typedef VariableScalarFunction<std::plus<Scalar> > BaseType;

  protected:
    Add() : BaseType("+") {};

  public:
    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_ADD_HPP_ */
