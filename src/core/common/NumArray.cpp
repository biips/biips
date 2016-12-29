#include "common/NumArray.hpp"
#include "common/MultiArray.hpp"
#include "common/Vector.hpp"
#include "common/Matrix.hpp"
#include "common/Utility.hpp"

namespace Biips
{

  NumArray::NumArray(const MultiArray & marray) :
      pDim_(marray.DimPtr().get()), pValues_(marray.ValuesPtr().get())
  {

  }

  Bool allMissing(const NumArray & marray)
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

  Bool anyMissing(const NumArray & marray)
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

  std::ostream & operator << (std::ostream & os, const NumArray & dat)
  {
    const IndexRange range(dat.Dim());
    Types<IndexRange::IndexType>::Array indices = range.Lower();
    printRecursion(os, dat.Values(), range, range.NDim()-1, indices);
    return os;
  }

  NumArrayArray::NumArrayArray(const MultiArrayArray & marray) :
      NumArrayArray::BaseType(marray.size())
  {
    for (Size i = 0; i < size(); ++i)
      at(i).SetPtr(marray[i].DimPtr().get(), marray[i].ValuesPtr().get());
  }

}
