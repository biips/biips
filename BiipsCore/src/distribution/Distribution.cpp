//                                               -*- C++ -*-
/*! \file Distribution.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "distribution/Distribution.hpp"

namespace Biips
{

  Bool Distribution::CheckParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    if (!CheckNParam(paramDims.size()))
      throw LogicError(String("Incorrect number of parameters for distribution ") + name_);

    return checkParamDims(paramDims);
  };


  DimArray Distribution::Dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    if (!CheckParamDims(paramDims))
      throw LogicError(String("Non-conforming parameters dimensions for distribution ") + name_);

    return dim(paramDims).Drop();
  }
}
