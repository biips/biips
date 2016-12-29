#ifndef BIIPS_PROBIT_HPP_
#define BIIPS_PROBIT_HPP_

#include <cmath>

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  struct ProbitScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const;
  };

  struct PhiScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const;
  };

  class Probit: public UnaryScalarFunction<ProbitScalar>
  {
    public:
    typedef Probit SelfType;
    typedef UnaryScalarFunction<ProbitScalar> BaseType;

  protected:
    Probit() :
      BaseType("probit")
    {
    }

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;

    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

  class Phi: public UnaryScalarFunction<PhiScalar>
  {
    public:
    typedef Phi SelfType;
    typedef UnaryScalarFunction<PhiScalar> BaseType;

  protected:
    Phi() :
      BaseType("phi")
    {
    }

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

} /* namespace Biips */

#endif /* BIIPS_PROBIT_HPP_ */
