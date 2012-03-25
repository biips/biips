//                                               -*- C++ -*-
/*! \file StochasticNode.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_STOCHASTICNODE_HPP_
#define BIIPS_STOCHASTICNODE_HPP_

#include "Node.hpp"
#include "distribution/Distribution.hpp"

namespace Biips
{

  class StochasticNode: public Node
  {
  public:
    typedef StochasticNode SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Types<SelfType>::IteratorPair IteratorPair;

  protected:
    typedef NumArray::StorageType StorageType;

    Distribution::Ptr pPrior_;
    NodeId lowerNodeId_;
    NodeId upperNodeId_;

  public:
    const Distribution::Ptr & PriorPtr() const
    {
      return pPrior_;
    }
    const String & PriorName() const
    {
      return pPrior_->Name();
    }

    void Sample(ValArray & values,
                const NumArray::Array & paramValues,
                const NumArray::Pair & boundValues,
                Rng & rng) const
    {
      pPrior_->Sample(values, paramValues, boundValues, rng);
    }
    Scalar LogPriorDensity(const NumArray & x,
                           const NumArray::Array & paramValues,
                           const NumArray::Pair & boundValues) const
    {
      return pPrior_->LogDensity(x, paramValues, boundValues);
    }
    void UnboundedSupport(ValArray & lower,
                          ValArray & upper,
                          const NumArray::Array & paramValues) const
    {
      pPrior_->UnboundedSupport(lower, upper, paramValues);
    }

    NodeId Lower() const
    {
      return lowerNodeId_;
    }

    NodeId Upper() const
    {
      return upperNodeId_;
    }

    Bool IsLowerBounded() const
    {
      return lowerNodeId_ != NULL_NODEID;
    }
    Bool IsUpperBounded() const
    {
      return upperNodeId_ != NULL_NODEID;
    }
    Bool IsBounded() const
    {
      return IsLowerBounded() || IsUpperBounded();
    }

    StochasticNode(const DimArray::Ptr pDim,
                   const Distribution::Ptr & pPrior,
                   const Types<NodeId>::Array & parameters,
                   NodeId lower = NULL_NODEID,
                   NodeId upper = NULL_NODEID);

    virtual ~StochasticNode()
    {
    }
  };

} /* namespace Biips */

#endif /* BIIPS_STOCHASTICNODE_HPP_ */
