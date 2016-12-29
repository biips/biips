#include "functions/Equals.hpp"
#include <cfloat>

namespace Biips
{

  Bool Equals::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    for (Size i = 0; i < paramDims.size(); ++i)
    {
      if (!paramDims[0]->IsScalar())
        return false;
    }
    return true;
  }

  void Equals::eval(ValArray & values, const NumArray::Array & paramValues) const
  {
    values[0] = std::fabs(paramValues[0].ScalarView()
        - paramValues[1].ScalarView()) < 16 * DBL_EPSILON;
  }
}
