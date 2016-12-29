#ifndef BIIPS_LOGIT_HPP_
#define BIIPS_LOGIT_HPP_

#include <cmath>

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  struct LogitScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::log(val)-std::log(1-val);
    }
  };

  struct ILogitScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return 1/(1 + std::exp(-val));
    }
  };

  class Logit: public UnaryScalarFunction<LogitScalar>
  {
    public:
    typedef Logit SelfType;
    typedef UnaryScalarFunction<LogitScalar> BaseType;

  protected:
    Logit() :
      BaseType("logit")
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

  class ILogit: public UnaryScalarFunction<ILogitScalar>
  {
    public:
    typedef ILogit SelfType;
    typedef UnaryScalarFunction<ILogitScalar> BaseType;

  protected:
    ILogit() :
      BaseType("ilogit")
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

#endif /* BIIPS_LOGIT_HPP_ */
