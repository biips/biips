#include "functions/TrigonometricFunctions.hpp"

namespace Biips
{

  Bool ArcCos::CheckParamValues(const NumArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();
    for (Size i = 0; i < x.size(); ++i)
    {
      if (x[i] < -1.0 || x[i] > 1.0)
        return false;
    }
    return true;
  }

  Bool ArcSin::CheckParamValues(const NumArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();
    for (Size i = 0; i < x.size(); ++i)
    {
      if (x[i] < -1.0 || x[i] > 1.0)
        return false;
    }
    return true;
  }

  Bool ArcCosh::CheckParamValues(const NumArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();
    for (Size i = 0; i < x.size(); ++i)
    {
      if (x[i] < 1.0)
        return false;
    }
    return true;
  }

  Bool ArcTanh::CheckParamValues(const NumArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();
    for (Size i = 0; i < x.size(); ++i)
    {
      if (x[i] <= -1.0 || x[i] >= 1.0)
        return false;
    }
    return true;
  }
}
