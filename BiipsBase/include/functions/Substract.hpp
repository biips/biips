//                                               -*- C++ -*-
/*! \file Substract.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_SUBSTRACT_HPP_
#define BIIPS_SUBSTRACT_HPP_

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  class Substract : public BinaryScalarFunction<std::minus<Scalar> >
  {
  public:
    typedef Substract SelfType;
    typedef BinaryScalarFunction<std::minus<Scalar> > BaseType;

  protected:
    Substract() : BaseType("-") {};

  public:
    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_SUBSTRACT_HPP_ */
