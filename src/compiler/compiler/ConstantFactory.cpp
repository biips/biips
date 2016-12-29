#include "compiler/ConstantFactory.hpp"

namespace Biips
{

  bool lt(DimArray const &dim1, DimArray const &dim2)
  {
    if (dim1.size() == dim2.size())
    {
      for (Size i = 0; i < dim1.size(); ++i)
      {
        if (dim1[i] == dim2[i])
          continue;
        else
          return dim1[i] < dim2[i];
      }
    }
    else
      return dim1.size() < dim2.size();

    return false;
  }

  /* Comparison function for ValArray of equal length */
  bool lt(ValArray const &value1, ValArray const &value2)
  {
    for (Size i = 0; i < value1.size(); ++i)
    {
      if (lt(value1[i], value2[i]))
        return true;
      else if (lt(value2[i], value1[i]))
        return false;
    }
    return false;
  }

  bool ltMultiArray::operator()(const MultiArray & arg1,
                                const MultiArray & arg2) const
  {
    if (lt(arg1.Dim(), arg2.Dim()))
      return true;
    else if (lt(arg2.Dim(), arg1.Dim()))
      return false;
    else
      return lt(arg2.Values(), arg1.Values());
  }

  NodeId ConstantFactory::GetNode(Scalar value)
  {
    std::map<Scalar, NodeId, ltScalar>::const_iterator it =
        constMap_.find(value);

    if (it != constMap_.end())
      return it->second;

    ValArray::Ptr pval(new ValArray(1, value));
    NodeId id = NewNode(P_SCALAR_DIM, pval);

    constMap_[value] = id;

    return id;
  }

  NodeId ConstantFactory::GetNode(const DimArray::Ptr & pDim,
                                  const ValArray::Ptr & pVal)
  {
    MultiArray marray(pDim, pVal);

    std::map<MultiArray, NodeId>::const_iterator it = mvConstMap_.find(marray);

    if (it != mvConstMap_.end())
      return it->second;

    // Create a new constant node
    NodeId id = NewNode(pDim, pVal);

    mvConstMap_[marray] = id;

    return id;
  }

  NodeId ConstantFactory::NewNode(const DimArray::Ptr & pDim,
                                  const ValArray::Ptr & pVal)
  {
    return graph_.AddConstantNode(pDim, pVal);
  }

} /* namespace Biips */
