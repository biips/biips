//                                            -*- C++ -*-
/*! \file Function.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_FUNCTION_HPP_
#define BIIPS_FUNCTION_HPP_

#include "common/Common.hpp"
#include "common/Table.hpp"

namespace Biips
{

  class Function
  {
  protected:
    const String name_;
    const Size nParam_;

    Function(const String & name, Size nParam) : name_(name), nParam_(nParam) {};

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const = 0;

    virtual Bool checkParamValues(const MultiArray::Array & paramValues) const = 0;

    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const = 0;

    virtual MultiArray eval(const MultiArray::Array & paramValues) const = 0;

  public:
    typedef Function SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    const String & Name() const { return name_; }
    virtual String Alias() const { return ""; }
    Size NParam() const { return nParam_; }

    virtual Bool IsPrefix() const { return false; }
    virtual Bool IsInfix() const { return false; }
    virtual Bool IsDiscreteValued(const Flags & mask) const { return false; }
    virtual Bool IsScale(const Flags & scaleMask, const Flags & knownMask) const { return false; }
    virtual Bool IsLinear(const Flags & linearMask, const Flags & knownMask) const { return IsScale(linearMask, knownMask); }

    Bool CheckNParam(Size n) const { return nParam_ > 0 ? n == nParam_ : n > 0; }

    Bool CheckParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;

    Bool CheckParamValues(const MultiArray::Array & paramValues) const;

    DimArray Dim(const Types<DimArray::Ptr>::Array & paramDims) const;

    MultiArray Eval(const MultiArray::Array & paramValues) const;

    virtual ~Function() {};
  };


  typedef Table<Function> FunctionTable;
}

#endif /* BIIPS_FUNCTION_HPP_ */
