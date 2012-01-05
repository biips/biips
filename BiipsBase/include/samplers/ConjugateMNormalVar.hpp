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

  class ConjugateMNormalVar: public ConjugateSampler<DMNormVar, DMNormVar, 0>
  {
  public:
    typedef ConjugateMNormalVar SelfType;
    typedef ConjugateSampler<DMNormVar, DMNormVar, 0> BaseType;

    static const String NAME_;

  protected:
    friend class ConjugateSamplerFactory<SelfType> ;

    explicit ConjugateMNormalVar(const Graph & graph) :
      BaseType(graph)
    {
    }

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

  typedef ConjugateSamplerFactory<ConjugateMNormalVar>
      ConjugateMNormalVarFactory;

}

#endif /* BIIPS_CONJUGATEMNORMALVAR_HPP_ */
