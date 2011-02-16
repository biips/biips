//                                               -*- C++ -*-
/*! \file ConjugateMNormal.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_CONJUGATEMNORMAL_HPP_
#define BIIPS_CONJUGATEMNORMAL_HPP_

#include "distributions/DMNorm.hpp"
#include "samplers/ConjugateSampler.hpp"

namespace Biips
{

  class ConjugateMNormal : public ConjugateSampler<DMNorm, DMNorm, 0>
  {
  public:
    typedef ConjugateMNormal SelfType;
    typedef ConjugateSampler<DMNorm, DMNorm, 0> BaseType;

    static const String NAME_;

  protected:
    friend class ConjugateSamplerFactory<SelfType>;

    explicit ConjugateMNormal(const Graph * pGraph )
     : BaseType(pGraph) {};

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


  typedef ConjugateSamplerFactory<ConjugateMNormal> ConjugateMNormalFactory;

}

#endif /* BIIPS_CONJUGATEMNORMAL_HPP_ */
