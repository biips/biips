#include "functions/Sort.hpp"
#include <algorithm>

namespace Biips
{

  void Sort::eval(ValArray & values, const NumArray::Array & paramValues) const
  {
    values = paramValues[0].Values();
    std::sort(values.begin(), values.end());
  }

} /* namespace Biips */
