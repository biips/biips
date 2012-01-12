//                                               -*- C++ -*-
/*! \file ConjugateNormal.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_CONJUGATENORMAL_HPP_
#define BIIPS_CONJUGATENORMAL_HPP_

#include "distributions/DNorm.hpp"
#include "samplers/ConjugateSampler.hpp"

namespace Biips
{

  class ConjugateNormal: public ConjugateSampler<DNorm, DNorm, 0>
  {
  public:
    typedef ConjugateNormal SelfType;
    typedef ConjugateSampler<DNorm, DNorm, 0> BaseType;

    static const String NAME_;

  protected:
    friend class ConjugateSamplerFactory<SelfType> ;

    explicit ConjugateNormal(const Graph & graph) :
      BaseType(graph)
    {
    }

    virtual NumArray::Array initLikeParamContrib() const;
    virtual void formLikeParamContrib(NodeId likeId,
                                      NumArray::Array & likeParamContribValues);
    virtual NumArray::Array
    postParam(const NumArray::Array & priorParamContribValues,
              const NumArray::Array & likeParamContribValues) const;
    virtual Scalar
    computeLogIncrementalWeight(const NumArray & sampledData,
                                const NumArray::Array & priorParamValues,
                                const NumArray::Array & postParamValues,
                                const NumArray::Array & LikeParamContrib);

  public:
    virtual const String & Name() const
    {
      return NAME_;
    }

  };

  typedef ConjugateSamplerFactory<ConjugateNormal> ConjugateNormalFactory;

}

#endif /* BIIPS_CONJUGATENORMAL_HPP_ */
