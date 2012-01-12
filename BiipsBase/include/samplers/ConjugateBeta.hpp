//                                               -*- C++ -*-
/*! \file ConjugateBeta.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_CONJUGATEBETA_HPP_
#define BIIPS_CONJUGATEBETA_HPP_

#include "distributions/DBeta.hpp"
#include "distributions/DBin.hpp"
#include "samplers/ConjugateSampler.hpp"

namespace Biips
{

  class ConjugateBeta: public ConjugateSampler<DBeta, DBin, 1>
  {
  public:
    typedef ConjugateBeta SelfType;
    typedef ConjugateSampler<DBeta, DBin, 1> BaseType;

    static const String NAME_;

  protected:
    friend class ConjugateSamplerFactory<SelfType> ;

    explicit ConjugateBeta(const Graph & graph) :
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

  typedef ConjugateSamplerFactory<ConjugateBeta> ConjugateBetaFactory;
}

#endif /* BIIPS_CONJUGATEBETA_HPP_ */
