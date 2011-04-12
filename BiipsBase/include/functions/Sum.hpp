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

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;

    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const { return *P_SCALAR_DIM; };

  public:
    virtual MultiArray Eval(const MultiArray::Array & paramValues) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_SUM_HPP_ */
