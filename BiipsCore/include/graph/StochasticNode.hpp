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

  //! StochasticNode concrete class
  /*!
   * Represents a stochastic node in a DAG, defined by a distribution.
   *
   * This type of node is defined by ~ operator in BUGS language,
   * followed by a distribution identifier
   * and its parameters between parentheses.
   * For example :
   * x ~ dnorm(mu, var)
   */
  class StochasticNode : public Node
  {
  public:
    typedef StochasticNode SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Types<SelfType>::IteratorPair IteratorPair;

  protected:
    typedef MultiArray::StorageType StorageType;

    Distribution::Ptr pPrior_;

  public:
    virtual NodeType GetType() const { return STOCHASTIC; };
    virtual void AcceptVisitor(NodeVisitor & vis);
    virtual void AcceptVisitor(ConstNodeVisitor & vis) const;

    const String & PriorName() const { return pPrior_->Name(); };

    MultiArray Sample(const MultiArray::Array & paramValues, Rng * pRng) const { return pPrior_->Sample(paramValues, pRng); };
    Scalar LogLike(const MultiArray & x, const MultiArray::Array & paramValues) const { return pPrior_->LogPdf(x, paramValues); };

    StochasticNode(const DimArray::Ptr pDim, const Distribution::Ptr & pPrior, const Types<NodeId>::Array & parameters) : Node(pDim, parameters), pPrior_(pPrior) {}

    virtual ~StochasticNode() {};
  };


} /* namespace Biips */

#endif /* BIIPS_STOCHASTICNODE_HPP_ */
