#include "common/MultiArray.hpp"
#include "common/Vector.hpp"
#include "common/Matrix.hpp"
#include "common/Utility.hpp"

namespace Biips
{
  Bool allMissing(const MultiArray & marray)
  {
    Size N = marray.Length();
    const ValArray & v = marray.Values();
    for (Size i = 0; i < N; ++i)
    {
      if (!isNA(v[i]))
        return false;
    }
    return true;
  }

  Bool anyMissing(const MultiArray & marray)
  {
    Size N = marray.Length();
    const ValArray & v = marray.Values();
    for (Size i = 0; i < N; ++i)
    {
      if (isNA(v[i]))
        return true;
    }
    return false;
  }

  std::ostream & operator << (std::ostream & os, const MultiArray & dat)
  {
    const IndexRange range(dat.Dim());
    Types<IndexRange::IndexType>::Array indices = range.Lower();
    printRecursion(os, dat.Values(), range, range.NDim()-1, indices);
    return os;
  }
}
