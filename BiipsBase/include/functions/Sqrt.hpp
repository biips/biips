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

#include "functions/Pow.hpp"

namespace Biips
{

  class Sqrt : public Biips::Function
  {
  protected:
    typedef Sqrt SelfType;

    Sqrt() : Function("sqrt", 1) {};

  public:
    virtual DataType Eval(const DataType::Array & paramValues) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_SQRT_HPP_ */
