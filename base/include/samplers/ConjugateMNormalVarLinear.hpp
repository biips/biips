//                                               -*- C++ -*-
/*
 * BiiPS software is a set of libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
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

/*! \file ConjugateMNormalVarLinear.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_CONJUGATEMNORMALVARLINEAR_HPP_
#define BIIPS_CONJUGATEMNORMALVARLINEAR_HPP_

#include "sampler/NodeSampler.hpp"

namespace Biips
{

  class ConjugateMNormalVarLinear: public NodeSampler
  {
  public:
    typedef ConjugateMNormalVarLinear SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef NodeSampler BaseType;

  protected:
    static const String NAME_;

    friend class ConjugateMNormalVarLinearFactory;

    explicit ConjugateMNormalVarLinear(const Graph & graph) :
      BaseType(graph)
    {
    }

    virtual void sample(const StochasticNode & node);

  public:
    virtual const String & Name() const
    {
      return NAME_;
    }

    virtual ~ConjugateMNormalVarLinear()
    {
    }
  };

  class ConjugateMNormalVarLinearFactory: public NodeSamplerFactory
  {
  public:
    typedef ConjugateMNormalVarLinearFactory SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef ConjugateMNormalVarLinear CreatedType;
    typedef NodeSamplerFactory BaseType;

  protected:
    static Ptr pConjugateMNormalVarLinearFactoryInstance_;
    ConjugateMNormalVarLinearFactory()
    {
    }

  public:
    static BaseType::Ptr Instance()
    {
      return pConjugateMNormalVarLinearFactoryInstance_;
    }
    virtual Bool Create(const Graph & graph,
                        NodeId nodeId,
                        BaseType::CreatedPtr & pNodeSamplerInstance) const;
  };

}

#endif /* BIIPS_CONJUGATEMNORMALVARLINEAR_HPP_ */
