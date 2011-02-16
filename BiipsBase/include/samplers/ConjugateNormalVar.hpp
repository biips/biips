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


  typedef ConjugateSamplerFactory<ConjugateNormalVar> ConjugateNormalVarFactory;

}

#endif /* BIIPS_CONJUGATENORMALVAR_HPP_ */
