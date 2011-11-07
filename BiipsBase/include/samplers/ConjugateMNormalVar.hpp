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

    explicit ConjugateMNormalVar(const Graph & graph) : BaseType(graph) {}

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


  typedef ConjugateSamplerFactory<ConjugateMNormalVar> ConjugateMNormalVarFactory;

}

#endif /* BIIPS_CONJUGATEMNORMALVAR_HPP_ */
