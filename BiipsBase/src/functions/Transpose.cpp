//                                               -*- C++ -*-
/*! \file Transpose.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Transpose.hpp"


namespace Biips
{
    DataType Transpose::Eval(const DataType::Array & paramValues) const
    {
      // TODO check paramValues
      const DataType & val = paramValues[0];

      DataType ans(ublas::trans(Matrix(val)));
      return ans;
    }
}
