//                                               -*- C++ -*-
/*! \file Subtract.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_SUBTRACT_HPP_
#define BIIPS_SUBTRACT_HPP_

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  class Subtract : public BinaryScalarFunction<std::minus<Scalar> >
  {
  public:
    typedef Subtract SelfType;
    typedef BinaryScalarFunction<std::minus<Scalar> > BaseType;

  protected:
    Subtract() : BaseType("-") {};

  public:
    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_SUBTRACT_HPP_ */
