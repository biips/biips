//                                               -*- C++ -*-
/*! \file Sum.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Sum.hpp"


namespace Biips
{
    DataType Sum::Eval(const DataType::Array & paramValues) const
    {
      // TODO check paramValues
      const DataType & val = paramValues[0];

      DataType ans(DimArray(1,1));
      ans.ScalarView() = val.Values().Sum();

      return ans;
    }
}
