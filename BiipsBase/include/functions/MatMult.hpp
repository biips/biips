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

  class MatMult : public Biips::Function
  {
  protected:
    typedef MatMult SelfType;

    MatMult() : Function("%*%", 2) {};

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;

    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;

  public:
    virtual MultiArray Eval(const MultiArray::Array & paramValues) const;

    static Function::Ptr Instance() { static Function::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_MATMULT_HPP_ */
