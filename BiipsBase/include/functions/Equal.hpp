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
    typedef BinaryScalarFunction<std::equal_to<Scalar> > BaseType;

  protected:
    Equal() : BaseType("==") {};

  public:
    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_EQUAL_HPP_ */
