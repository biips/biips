//                                               -*- C++ -*-
/*! \file BinaryScalarFunction.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_SCALARFUNCTION_HPP_
#define BIIPS_SCALARFUNCTION_HPP_

#include "function/Function.hpp"

namespace Biips
{

  template<typename UnaryOperator>
  class UnaryScalarFunction : public Function
  {
  public:
    typedef Function BaseType;
    typedef UnaryScalarFunction SelfType;
    typedef typename Types<SelfType>::Ptr Ptr;

  protected:
    UnaryScalarFunction(const String & name) : BaseType(name, 1) {};

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const { return true; }

    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;

  public:virtual MultiArray Eval(const MultiArray::Array & paramValues) const;

    virtual ~UnaryScalarFunction() {};
  };


  template<typename BinaryOperator>
  class BinaryScalarFunction : public Function
  {
  public:
    typedef Function BaseType;
    typedef BinaryScalarFunction SelfType;
    typedef typename Types<SelfType>::Ptr Ptr;

  protected:
    BinaryScalarFunction(const String & name) : BaseType(name, 2) {};

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;

    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;

  public:
    virtual MultiArray Eval(const MultiArray::Array & paramValues) const;

    virtual ~BinaryScalarFunction() {};
  };


  template<typename BinaryOperator>
  class VariableScalarFunction : public Function
  {
  public:
    typedef Function BaseType;
    typedef VariableScalarFunction SelfType;
    typedef typename Types<SelfType>::Ptr Ptr;

  protected:
    VariableScalarFunction(const String & name) : BaseType(name, 0) {};

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;

    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;

  public:
    virtual MultiArray Eval(const MultiArray::Array & paramValues) const;

    virtual ~VariableScalarFunction() {};
  };




  template<typename UnaryOperator>
  MultiArray UnaryScalarFunction<UnaryOperator>::Eval(const MultiArray::Array & paramValues) const
  {
    const MultiArray & val = paramValues[0];

    static UnaryOperator op;

    MultiArray ans = MultiArray(val.DimPtr(), val.Values().Apply(op));
    return ans;
  }


  template<typename UnaryOperator>
  DimArray UnaryScalarFunction<UnaryOperator>::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *paramDims[0];
  }

  template<typename BinaryOperator>
  Bool BinaryScalarFunction<BinaryOperator>::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    if(paramDims[0]->IsScalar())
      return true;
    else if(paramDims[1]->IsScalar())
      return true;
    else
      return *paramDims[0] == *paramDims[1];
  }


  template<typename BinaryOperator>
  DimArray BinaryScalarFunction<BinaryOperator>::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    if ( paramDims[0]->IsScalar() )
      return *paramDims[1];
    else
      return *paramDims[0];
  }


  template<typename BinaryOperator>
  MultiArray BinaryScalarFunction<BinaryOperator>::Eval(const MultiArray::Array & paramValues) const
  {
    // TODO check paramValues
    const MultiArray & left = paramValues[0];
    const MultiArray & right = paramValues[1];

    static BinaryOperator op;

    MultiArray ans;
    if ( left.IsScalar() )
      ans = MultiArray(right.DimPtr(), right.Values().Apply(std::bind1st(op, left.ScalarView())));
    else if ( right.IsScalar() )
      ans = MultiArray(left.DimPtr(), left.Values().Apply(std::bind2nd(op, right.ScalarView())));
    else
      ans = MultiArray(left.DimPtr(), left.Values().Apply(op, right.Values()));
    return ans;
  }


  template<typename BinaryOperator>
  Bool VariableScalarFunction<BinaryOperator>::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    Bool ans = true;
    DimArray::Ptr p_ref_dim;
    for (Size i=0; i<paramDims.size() && ans; ++i)
    {
      if(paramDims[i]->IsScalar())
        continue;
      else if (!p_ref_dim)
        p_ref_dim = paramDims[i];
      else
        ans = *paramDims[i] == *p_ref_dim;
    }
    return ans;
  }


  template<typename BinaryOperator>
  DimArray VariableScalarFunction<BinaryOperator>::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    DimArray::Ptr p_ref_dim = paramDims[0];
    if (p_ref_dim->IsScalar())
    {
      for (Size i=1; i<paramDims.size(); ++i)
      {
        if(paramDims[i]->IsScalar())
          continue;
        else
        {
          p_ref_dim = paramDims[i];
          break;
        }
      }
    }
    return *p_ref_dim;
  }


  template<typename BinaryOperator>
  MultiArray VariableScalarFunction<BinaryOperator>::Eval(const MultiArray::Array & paramValues) const
  {
    // TODO check paramValues
    static BinaryOperator op;

    MultiArray ans;
    ans.Alloc(paramValues[0]);
    for (Size i=1; i<paramValues.size(); ++i)
    {
      const MultiArray & right = paramValues[i];
      if ( ans.IsScalar() )
      {
        Scalar s = ans.ScalarView();
        ans.Alloc(right);
        ans.Values().SelfApply(std::bind1st(op, s));
      }
      else if ( right.IsScalar() )
        ans.Values().SelfApply(std::bind2nd(op, right.ScalarView()));
      else
        ans.Values().SelfApply(op, right.Values());
    }
    return ans;
  }
}

#endif /* BIIPS_SCALARFUNCTION_HPP_ */
