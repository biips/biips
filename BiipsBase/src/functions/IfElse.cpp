//                                               -*- C++ -*-
/*! \file IfElse.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/IfElse.hpp"

namespace Biips
{

  Bool IfElse::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    DimArray ref_dim;

    for (Size i=0; i<paramDims.size(); ++i)
    {
      if (!paramDims[i]->IsScalar())
      {
        ref_dim = *paramDims[i];
        break;
      }
    }

    for (Size i=0; i<paramDims.size(); ++i)
    {
      if (paramDims[i]->IsScalar())
        continue;
      if (*paramDims[i] != ref_dim)
        return false;
    }
    return true;
  }


  DimArray IfElse::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    for (Size i=0; i<paramDims.size(); ++i)
    {
      if ( !paramDims[i]->IsScalar() )
        return *paramDims[i];
    }

    return *paramDims[0];
  }


  MultiArray IfElse::eval(const MultiArray::Array & paramValues) const
  {
    DimArray::Ptr p_dim = P_SCALAR_DIM;
    for (Size i=0; i<paramValues.size(); ++i)
    {
      if (!paramValues[i].IsScalar())
      {
        p_dim = paramValues[i].DimPtr();
        break;
      }
    }
    MultiArray ans(p_dim);

    const MultiArray & x = paramValues[0];
    const MultiArray & a = paramValues[1];
    const MultiArray & b = paramValues[2];

    for (Size i=0, j=0, k=0, l=0; i<ans.Length(); ++i)
    {
      ans.Values()[i] = x.Values()[j] ? a.Values()[k] : b.Values()[l];

      if (!x.IsScalar())
        ++j;
      if (!a.IsScalar())
        ++k;
      if (!b.IsScalar())
        ++l;
    }

    return ans;
  }


  Bool IfElse::IsDiscreteValued(const Flags & mask) const
  {
    return mask[1] && mask[2];
  }
}
