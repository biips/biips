//                                               -*- C++ -*-
/*! \file Pow.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_POW_HPP_
#define BIIPS_POW_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class Pow : public Biips::Function
  {
  protected:
    typedef Pow SelfType;

    Pow() : Function("^", 2) {};

  public:
    virtual DataType Eval(const DataType::Array & paramValues) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_POW_HPP_ */
