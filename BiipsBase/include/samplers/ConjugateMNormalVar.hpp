//                                               -*- C++ -*-
/*! \file ConjugateMNormalVar.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_CONJUGATEMNORMALVAR_HPP_
#define BIIPS_CONJUGATEMNORMALVAR_HPP_

#include "distributions/DMNormVar.hpp"
#include "samplers/ConjugateSampler.hpp"

namespace Biips
{

  class ConjugateMNormalVar : public ConjugateSampler<DMNormVar, DMNormVar, 0>
  {
  public:
    typedef ConjugateMNormalVar SelfType;
    typedef ConjugateSampler<DMNormVar, DMNormVar, 0> BaseType;

    static const String NAME_;

  protected:
    friend class ConjugateSamplerFactory<SelfType>;

    explicit ConjugateMNormalVar(const Graph * pGraph )
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


  typedef ConjugateSamplerFactory<ConjugateMNormalVar> ConjugateMNormalVarFactory;

}

#endif /* BIIPS_CONJUGATEMNORMALVAR_HPP_ */
