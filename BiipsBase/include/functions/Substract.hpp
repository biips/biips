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

#include "function/Function.hpp"

namespace Biips
{

  class Substract : public Biips::Function
  {
  protected:
    typedef Substract SelfType;

    Substract() : Function("-", 2) {};

  public:
    virtual DataType Eval(const DataType::Array & paramValues) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_SUBSTRACT_HPP_ */
