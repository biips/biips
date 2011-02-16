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

#include "function/Function.hpp"

namespace Biips
{

  class Equal : public Biips::Function
  {
  protected:
    typedef Equal SelfType;

    Equal() : Function("==", 2) {};

  public:
    virtual DataType Eval(const DataType::Array & paramValues) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_EQUAL_HPP_ */
