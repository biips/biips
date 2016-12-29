#include "functions/Logit.hpp"

namespace Biips
{

  Bool Logit::CheckParamValues(const NumArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();

    for (Size i = 0; i < x.size(); ++i)
    {
      if (x[i] < 0.0 || x[i] > 1)
        return false;
    }

    return true;
  }

} /* namespace Biips */
