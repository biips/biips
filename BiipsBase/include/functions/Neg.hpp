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

#include "function/Function.hpp"

namespace Biips
{

  class Neg : public Biips::Function
  {
  protected:
    typedef Neg SelfType;

    Neg() : Function("NEG", 1) {};

  public:
    virtual DataType Eval(const DataType::Array & paramValues) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_NEG_HPP_ */
