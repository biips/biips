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

    virtual DataType::Array initLikeParamContrib() const;
    virtual void formLikeParamContrib(NodeId likeId,
        DataType::Array & likeParamContribValues);
    virtual DataType::Array postParam(const DataType::Array & priorParamContribValues,
        const DataType::Array & likeParamContribValues) const;
    virtual Scalar computeLogWeight(const DataType & sampledData,
        const DataType::Array & priorParamValues,
        const DataType::Array & postParamValues,
        const DataType::Array & LikeParamContrib);

  public:
    virtual const String & Name() const { return NAME_; };

  };


  typedef ConjugateSamplerFactory<ConjugateNormal> ConjugateNormalFactory;

}

#endif /* BIIPS_CONJUGATENORMAL_HPP_ */
