//                                               -*- C++ -*-
/*! \file Step.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_STEP_HPP_
#define BIIPS_STEP_HPP_

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  struct StepScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return val >= 0.0 ? 1.0 : 0.0;
    }
  };

  class Step: public UnaryScalarFunction<StepScalar>
  {
  public:
    typedef Step SelfType;
    typedef UnaryScalarFunction<StepScalar> BaseType;

  protected:
    Step() :
      BaseType("step")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    virtual Bool IsDiscreteValued(const Flags & mask) const
    {
      return true;
    }
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

}

#endif /* BIIPS_STEP_HPP_ */
