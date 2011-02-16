//                                               -*- C++ -*-
/*! \file Cos.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_COS_HPP_
#define BIIPS_COS_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class Cos : public Biips::Function
  {
  protected:
    typedef Cos SelfType;

    Cos() : Function("cos", 1) {};

  public:
    virtual DataType Eval(const DataType::Array & paramValues) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_COS_HPP_ */
