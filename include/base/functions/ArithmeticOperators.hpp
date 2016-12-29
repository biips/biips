#ifndef BIIPS_ARITHMETICOPERATORS_HPP_
#define BIIPS_ARITHMETICOPERATORS_HPP_

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  class Add: public VariableScalarFunction<std::plus<Scalar> >
  {
  public:
    typedef Add SelfType;
    typedef VariableScalarFunction<std::plus<Scalar> > BaseType;

  protected:
    Add() :
      BaseType("+")
    {
    }

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

    virtual Bool IsInfix() const
    {
      return true;
    }
    virtual Bool IsDiscreteValued(const Flags & mask) const;
    virtual Bool
    IsLinear(const Flags & linearMask, const Flags & knownMask) const;

    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

  class Subtract: public BinaryScalarFunction<std::minus<Scalar> >
  {
  public:
    typedef Subtract SelfType;
    typedef BinaryScalarFunction<std::minus<Scalar> > BaseType;

  protected:
    Subtract() :
      BaseType("-")
    {
    }

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

    virtual Bool IsInfix() const
    {
      return true;
    }
    virtual Bool IsDiscreteValued(const Flags & mask) const;
    virtual Bool
    IsLinear(const Flags & linearMask, const Flags & knownMask) const;

    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

  // TODO : check that any zero argument produces zero evaluation, cf. JAGS Multiply class
  class Multiply: public VariableScalarFunction<std::multiplies<Scalar> >
  {
  public:
    typedef Multiply SelfType;
    typedef VariableScalarFunction<std::multiplies<Scalar> > BaseType;

  protected:
    Multiply() :
      BaseType("*")
    {
    }

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }
    virtual Bool IsDiscreteValued(const Flags & mask) const;

    virtual Bool IsInfix() const
    {
      return true;
    }
    virtual Bool IsScale(const Flags & scaleMask, const Flags & knownMask) const;

    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

  class Divide: public BinaryScalarFunction<std::divides<Scalar> >
  {
  public:
    typedef Divide SelfType;
    typedef BinaryScalarFunction<std::divides<Scalar> > BaseType;

  protected:
    Divide() :
      BaseType("/")
    {
    }


  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;
    virtual Bool IsInfix() const
    {
      return true;
    }
    Bool IsScale(const Flags & scaleMask, const Flags & knownMask) const;

    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

  class Neg: public UnaryScalarFunction<std::negate<Scalar> >
  {
  public:
    typedef Neg SelfType;
    typedef UnaryScalarFunction<std::negate<Scalar> > BaseType;

  protected:
    Neg() :
      BaseType("NEG")
    {
    }


  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }
    virtual Bool IsPrefix() const
    {
      return true;
    }
    virtual Bool IsDiscreteValued(const Flags & mask) const;
    virtual Bool IsScale(const Flags & scaleMask, const Flags & knownMask) const;

    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };
}

#endif /* BIIPS_ARITHMETICOPERATORS_HPP_ */
