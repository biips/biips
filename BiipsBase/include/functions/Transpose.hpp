//                                               -*- C++ -*-
/*! \file Transpose.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef TRANSPOSE_HPP_
#define TRANSPOSE_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class Transpose : public Biips::Function
  {
  protected:
    typedef Transpose SelfType;

    Transpose() : Function("t", 1) {};

  public:
    virtual DataType Eval(const DataType::Array & paramValues) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* TRANSPOSE_HPP_ */
