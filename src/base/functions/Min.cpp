#include "functions/Min.hpp"

namespace Biips
{

  void Min::eval(ValArray & values, const NumArray::Array & paramValues) const
  {
    Scalar ans = *std::min_element(paramValues[0].Values().begin(),
                                  paramValues[0].Values().end());
    for (Size i = 1; i < paramValues.size(); ++i)
    {
      Scalar max_i = *std::min_element(paramValues[i].Values().begin(),
                                      paramValues[i].Values().end());
      ans = std::min(ans, max_i);
    }
    values[0] = ans;
  }

  Bool Min::IsDiscreteValued(const Flags & mask) const
  {
    return allTrue(mask);
  }
}

