#ifndef BIIPS_CONSTANTFACTORY_HPP_
#define BIIPS_CONSTANTFACTORY_HPP_

#include "graph/Graph.hpp"

namespace Biips
{

  inline bool lt(Scalar value1, Scalar value2)
  {
//    return value1 < value2 - 16*std::numeric_limits<Scalar>::epsilon();
    return value1 < value2 - std::numeric_limits<Scalar>::epsilon();
  }

  /**
   * @short STL function object for the map class using Scalar as a key
   */
  struct ltScalar
  {
    bool operator()(Scalar arg1, Scalar arg2) const
    {
      return lt(arg1, arg2);
    }
  };

  struct ltMultiArray
  {
    bool operator()(const MultiArray & arg1, const MultiArray & arg2) const;
  };

  /**
   * @short Factory for ConstantNode objects
   *
   * The purpose of a ConstantFactory is to avoid unnecessary
   * duplication of constant nodes by having a container class and
   * factory for them that will create and/or lookup constant nodes
   * based on their value.
   */
  class ConstantFactory
  {
    Graph & graph_;
    std::map<Scalar, NodeId, ltScalar> constMap_;
    std::map<MultiArray, NodeId, ltMultiArray> mvConstMap_;
  public:
    ConstantFactory(Graph & graph) :
        graph_(graph)
    {
    }
    /**
     * Get a constant node with a given value.  The results are cached,
     * so if a request is repeated, the same node will be returned.
     * If a node is newly allocated, it is added to the given model.
     */
    NodeId GetNode(Scalar value);
    /**
     * Get a multivariate constant node. The results are cached
     * so that if a request is repeated, the same node will be returned.
     * If a node is newly allocated, it is added to the given model.
     */
    NodeId GetNode(const DimArray::Ptr & pDim, const ValArray::Ptr & pVal);

    NodeId NewNode(const DimArray::Ptr & pDim, const ValArray::Ptr & pVal);
  };

} /* namespace Biips */
#endif /* CONSTBIIPS_CONSTANTFACTORY_HPP_ANTFACTORY_HPP_ */
