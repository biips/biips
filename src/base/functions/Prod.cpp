#include "functions/Prod.hpp"

namespace Biips
{

  void Prod::eval(ValArray & values, const NumArray::Array & paramValues) const
  {
    const NumArray & val = paramValues[0];

    values[0] = std::accumulate(val.Values().begin(),
                                val.Values().end(),
                                1.0,
                                std::multiplies<Scalar>());
  }

}
