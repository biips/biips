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

    explicit ConjugateMNormal(const Graph & graph) : BaseType(graph) {}

    virtual void formLikeParamContrib(NodeId likeId,
        MultiArray::Array & likeParamContribValues);
    virtual MultiArray::Array postParam(const MultiArray::Array & priorParamContribValues,
        const MultiArray::Array & likeParamContribValues) const;
    virtual Scalar computeLogIncrementalWeight(const MultiArray & sampledData,
        const MultiArray::Array & priorParamValues,
        const MultiArray::Array & postParamValues,
        const MultiArray::Array & LikeParamContrib);

  public:
    virtual const String & Name() const { return NAME_; };

  };


  typedef ConjugateSamplerFactory<ConjugateMNormal> ConjugateMNormalFactory;

}

#endif /* BIIPS_CONJUGATEMNORMAL_HPP_ */
