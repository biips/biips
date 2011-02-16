//                                               -*- C++ -*-
/*! \file Divide.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DIVIDE_HPP_
#define BIIPS_DIVIDE_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class Divide : public Biips::Function
  {
  protected:
    typedef Divide SelfType;

    Divide() : Function("/", 2) {};

  public:
    virtual DataType Eval(const DataType::Array & paramValues) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_DIVIDE_HPP_ */
