//                                               -*- C++ -*-
/*! \file TrigonometricFunctions.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_TRIGONOMETRICFUNCTIONS_HPP_
#define BIIPS_TRIGONOMETRICFUNCTIONS_HPP_

#include "functions/ScalarFunction.hpp"
#include <boost/math/special_functions/acosh.hpp>
#include <boost/math/special_functions/asinh.hpp>
#include <boost/math/special_functions/atanh.hpp>

namespace Biips
{

  struct CosScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::cos(val);
    }
  };

  struct SinScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::sin(val);
    }
  };

  struct TanScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::tan(val);
    }
  };

  struct ArcCosScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::acos(val);
    }
  };

  struct ArcSinScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::asin(val);
    }
  };

  struct ArcTanScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::atan(val);
    }
  };

  struct CoshScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::cosh(val);
    }
  };

  struct SinhScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::sinh(val);
    }
  };

  struct TanhScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::tanh(val);
    }
  };

  struct ArcCoshScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return boost::math::acosh(val);
    }
  };

  struct ArcSinhScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return boost::math::asinh(val);
    }
  };

  struct ArcTanhScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return boost::math::atanh(val);
    }
  };

  class Cos: public UnaryScalarFunction<CosScalar>
  {
  public:
    typedef Cos SelfType;
    typedef UnaryScalarFunction<CosScalar> BaseType;

  protected:
    Cos() :
      BaseType("cos")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

  class Sin: public UnaryScalarFunction<SinScalar>
  {
  public:
    typedef Sin SelfType;
    typedef UnaryScalarFunction<SinScalar> BaseType;

  protected:
    Sin() :
      BaseType("sin")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

  class Tan: public UnaryScalarFunction<TanScalar>
  {
  public:
    typedef Tan SelfType;
    typedef UnaryScalarFunction<TanScalar> BaseType;

  protected:
    Tan() :
      BaseType("tan")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

  class ArcCos: public UnaryScalarFunction<ArcCosScalar>
  {
  public:
    typedef ArcCos SelfType;
    typedef UnaryScalarFunction<ArcCosScalar> BaseType;

  protected:
    ArcCos() :
      BaseType("arccos")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const;

  public:
    virtual String Alias() const
    {
      return "acos";
    }
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

  class ArcSin: public UnaryScalarFunction<ArcSinScalar>
  {
  public:
    typedef ArcSin SelfType;
    typedef UnaryScalarFunction<ArcSinScalar> BaseType;

  protected:
    ArcSin() :
      BaseType("arcsin")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const;

  public:
    virtual String Alias() const
    {
      return "asin";
    }
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

  class ArcTan: public UnaryScalarFunction<ArcTanScalar>
  {
  public:
    typedef ArcTan SelfType;
    typedef UnaryScalarFunction<ArcTanScalar> BaseType;

  protected:
    ArcTan() :
      BaseType("arctan")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    virtual String Alias() const
    {
      return "atan";
    }
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

  class Cosh: public UnaryScalarFunction<CoshScalar>
  {
  public:
    typedef Cosh SelfType;
    typedef UnaryScalarFunction<CoshScalar> BaseType;

  protected:
    Cosh() :
      BaseType("cosh")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

  class Sinh: public UnaryScalarFunction<SinhScalar>
  {
  public:
    typedef Sinh SelfType;
    typedef UnaryScalarFunction<SinhScalar> BaseType;

  protected:
    Sinh() :
      BaseType("sinh")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

  class Tanh: public UnaryScalarFunction<TanhScalar>
  {
  public:
    typedef Tanh SelfType;
    typedef UnaryScalarFunction<TanhScalar> BaseType;

  protected:
    Tanh() :
      BaseType("tanh")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

  class ArcCosh: public UnaryScalarFunction<ArcCoshScalar>
  {
  public:
    typedef ArcCosh SelfType;
    typedef UnaryScalarFunction<ArcCoshScalar> BaseType;

  protected:
    ArcCosh() :
      BaseType("arccosh")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const;

  public:
    virtual String Alias() const
    {
      return "acosh";
    }
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

  class ArcSinh: public UnaryScalarFunction<ArcSinhScalar>
  {
  public:
    typedef ArcSinh SelfType;
    typedef UnaryScalarFunction<ArcSinhScalar> BaseType;

  protected:
    ArcSinh() :
      BaseType("arcsinh")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    virtual String Alias() const
    {
      return "asinh";
    }
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

  class ArcTanh: public UnaryScalarFunction<ArcTanhScalar>
  {
  public:
    typedef ArcTanh SelfType;
    typedef UnaryScalarFunction<ArcTanhScalar> BaseType;

  protected:
    ArcTanh() :
      BaseType("arctanh")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const;

  public:
    virtual String Alias() const
    {
      return "atanh";
    }
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };
}

#endif /* BIIPS_TRIGONOMETRICFUNCTIONS_HPP_ */
