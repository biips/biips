//                                               -*- C++ -*-
/*! \file Sum.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_SUM_HPP_
#define BIIPS_SUM_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class Sum : public Function
  {
  protected:
    typedef Sum SelfType;

    Sum() : Function("sum", 1) {};

  public:
    virtual DataType Eval(const DataType::Array & paramValues) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_SUM_HPP_ */
