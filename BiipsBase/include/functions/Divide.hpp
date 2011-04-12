//                                               -*- C++ -*-
/*! \file Divide.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DIVIDE_HPP_
#define BIIPS_DIVIDE_HPP_

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  class Divide : public BinaryScalarFunction<std::divides<Scalar> >
  {
  public:
    typedef Divide SelfType;
    typedef BinaryScalarFunction<std::divides<Scalar> > BaseType;

  protected:
    Divide() : BaseType("/") {};

  public:
    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_DIVIDE_HPP_ */
