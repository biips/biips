#ifndef BIIPS_CLOGLOG_HPP_
#define BIIPS_CLOGLOG_HPP_

#include <cmath>

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  struct CLogLogScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::log(-std::log(1 - val));
    }
  };

  struct ICLogLogScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return 1 - std::exp(-std::exp(val));
    }
  };

  class CLogLog: public UnaryScalarFunction<CLogLogScalar>
  {
    public:
    typedef CLogLog SelfType;
    typedef UnaryScalarFunction<CLogLogScalar> BaseType;

  protected:
    CLogLog() :
      BaseType("cloglog")
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

  class ICLogLog: public UnaryScalarFunction<ICLogLogScalar>
  {
    public:
    typedef ICLogLog SelfType;
    typedef UnaryScalarFunction<ICLogLogScalar> BaseType;

  protected:
    ICLogLog() :
      BaseType("icloglog")
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

#endif /* BIIPS_CLOGLOG_HPP_ */
