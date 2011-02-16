//                                               -*- C++ -*-
/*! \file Add.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_ADD_HPP_
#define BIIPS_ADD_HPP_

#include "function/Function.hpp"

namespace Biips
{
  
  class Add : public Biips::Function
  {
  protected:
    typedef Add SelfType;

    Add() : Function("+", 2) {};

  public:
    virtual DataType Eval(const DataType::Array & paramValues) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_ADD_HPP_ */
