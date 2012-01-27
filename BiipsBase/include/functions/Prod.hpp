//                                               -*- C++ -*-
/*! \file Prod.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_PROD_HPP_
#define BIIPS_PROD_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class Prod: public Function
  {
  protected:
    typedef Prod SelfType;

    Prod() :
      Function("prod", 1)
    {
    }

    virtual Bool
    checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return *P_SCALAR_DIM;
    }
    virtual void
        eval(ValArray & values, const NumArray::Array & paramValues) const;

  public:
    virtual Bool IsDiscreteValued(const Flags & mask) const;
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

}

#endif /* BIIPS_PROD_HPP_ */
