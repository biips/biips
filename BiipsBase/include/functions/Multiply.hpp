//                                               -*- C++ -*-
/*! \file Multiply.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_MULTIPLY_HPP_
#define BIIPS_MULTIPLY_HPP_

#include "function/Function.hpp"

namespace Biips
{
  
  class Multiply : public Biips::Function
  {
  protected:
    typedef Multiply SelfType;

    Multiply() : Function("*", 2) {};

  public:
    virtual DataType Eval(const DataType::Array & paramValues) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_MULTIPLY_HPP_ */
