#include "functions/UsualFunctions.hpp"

namespace Biips
{

  Bool Sqrt::CheckParamValues(const NumArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();

    for (Size i = 0; i < x.size(); ++i)
    {
      if (x[i] < 0.0)
        return false;
    }

    return true;
  }

  Bool Log::CheckParamValues(const NumArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();

    for (Size i = 0; i < x.size(); ++i)
    {
      if (x[i] <= 0.0)
        return false;
    }

    return true;
  }
}
