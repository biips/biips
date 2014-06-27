//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file ConjugateMNormalCov.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_CONJUGATEMNORMALCOV_HPP_
#define BIIPS_CONJUGATEMNORMALCOV_HPP_

#include "distributions/DMNormVar.hpp"
#include "samplers/ConjugateSampler.hpp"

namespace Biips
{

  class ConjugateMNormalCov: public ConjugateSampler<DMNormVar, DMNormVar, 0>
  {
  public:
    typedef ConjugateMNormalCov SelfType;
    typedef ConjugateSampler<DMNormVar, DMNormVar, 0> BaseType;

    static const String NAME_;

  protected:
    friend class ConjugateSamplerFactory<SelfType> ;

    explicit ConjugateMNormalCov(const Graph & graph) :
      BaseType(graph)
    {
    }

    virtual void formLikeParamContrib(NodeId likeId,
                                      MultiArray::Array & likeParamContribValues);
    virtual MultiArray::Array
    postParam(const NumArray::Array & priorParamContribValues,
              const MultiArray::Array & likeParamContribValues) const;
    virtual Scalar
    computeLogIncrementalWeight(const NumArray & sampledData,
                                const NumArray::Array & priorParamValues,
                                const NumArray::Array & postParamValues,
                                const MultiArray::Array & likeParamContrib);

  public:
    virtual const String & Name() const
    {
      return NAME_;
    }

  };

  typedef ConjugateSamplerFactory<ConjugateMNormalCov>
      ConjugateMNormalCovFactory;

}

#endif /* BIIPS_CONJUGATEMNORMALCOV_HPP_ */
