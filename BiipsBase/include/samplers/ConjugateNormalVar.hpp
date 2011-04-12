//                                               -*- C++ -*-
/*! \file ConjugateNormalVar.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_CONJUGATENORMALVAR_HPP_
#define BIIPS_CONJUGATENORMALVAR_HPP_

#include "distributions/DNormVar.hpp"
#include "samplers/ConjugateSampler.hpp"

namespace Biips
{

  class ConjugateNormalVar : public ConjugateSampler<DNormVar, DNormVar, 0>
  {
  public:
    typedef ConjugateNormalVar SelfType;
    typedef ConjugateSampler<DNormVar, DNormVar, 0> BaseType;

    static const String NAME_;

  protected:
    friend class ConjugateSamplerFactory<SelfType>;

    explicit ConjugateNormalVar(const Graph * pGraph )
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


  typedef ConjugateSamplerFactory<ConjugateNormalVar> ConjugateNormalVarFactory;

}

#endif /* BIIPS_CONJUGATENORMALVAR_HPP_ */
