//                                               -*- C++ -*-
/*! \file MatMult.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_MATMULT_HPP_
#define BIIPS_MATMULT_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class MatMult : public Function
  {
  protected:
    typedef MatMult SelfType;
    typedef Function BaseType;

    MatMult() : BaseType("%*%", 2) {}

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual Bool checkParamValues(const MultiArray::Array & paramValues) const { return true; }
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual MultiArray eval(const MultiArray::Array & paramValues) const;

  public:
    virtual Bool IsInfix() const { return true; }
    virtual Bool IsScale(const Flags & scaleMask, const Flags & knownMask) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; }
  };

}

#endif /* BIIPS_MATMULT_HPP_ */
