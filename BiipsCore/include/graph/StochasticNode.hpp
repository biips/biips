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
    typedef MultiArray::StorageType StorageType;

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

    MultiArray Sample(const MultiArray::Array & paramValues,
                      const MultiArray::Pair & boundValues,
                      Rng & rng) const
    {
      return pPrior_->Sample(paramValues, boundValues, rng);
    }
    Scalar LogPriorDensity(const MultiArray & x,
                           const MultiArray::Array & paramValues,
                           const MultiArray::Pair & boundValues) const
    {
      return pPrior_->LogDensity(x, paramValues, boundValues);
    }
    MultiArray::Pair UnboundedSupport(const MultiArray::Array & paramValues) const
    {
      return pPrior_->UnboundedSupport(paramValues);
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
