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

  class ConjugateNormal : public ConjugateSampler<DNorm, DNorm, 0>
  {
  public:
    typedef ConjugateNormal SelfType;
    typedef ConjugateSampler<DNorm, DNorm, 0> BaseType;

    static const String NAME_;

  protected:
    friend class ConjugateSamplerFactory<SelfType>;

    explicit ConjugateNormal(const Graph * pGraph )
     : BaseType(pGraph) {};

    virtual MultiArray::Array initLikeParamContrib() const;
    virtual void formLikeParamContrib(NodeId likeId,
        MultiArray::Array & likeParamContribValues);
    virtual MultiArray::Array postParam(const MultiArray::Array & priorParamContribValues,
        const MultiArray::Array & likeParamContribValues) const;
    virtual Scalar computeLogWeight(const MultiArray & sampledData,
        const MultiArray::Array & priorParamValues,
        const MultiArray::Array & postParamValues,
        const MultiArray::Array & LikeParamContrib);

  public:
    virtual const String & Name() const { return NAME_; };

  };


  typedef ConjugateSamplerFactory<ConjugateNormal> ConjugateNormalFactory;

}

#endif /* BIIPS_CONJUGATENORMAL_HPP_ */
