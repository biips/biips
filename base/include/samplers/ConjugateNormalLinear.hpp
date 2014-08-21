//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Biips is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of Biips.
 *
 * Biips is free software: you can redistribute it and/or modify
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

/*! \file ConjugateNormalLinear.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_CONJUGATENORMALLINEAR_HPP_
#define BIIPS_CONJUGATENORMALLINEAR_HPP_

#include "sampler/NodeSampler.hpp"

namespace Biips
{

  class ConjugateNormalLinear: public NodeSampler
  {
  public:
    typedef ConjugateNormalLinear SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef NodeSampler BaseType;

  protected:
    static const String NAME_;

    friend class ConjugateNormalLinearFactory;

    explicit ConjugateNormalLinear(const Graph & graph) :
      BaseType(graph)
    {
    }

    virtual void sample(const StochasticNode & node);

  public:
    virtual const String & Name() const
    {
      return NAME_;
    }

    virtual ~ConjugateNormalLinear()
    {
    }
  };

  class ConjugateNormalLinearFactory: public NodeSamplerFactory
  {
  public:
    typedef ConjugateNormalLinearFactory SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef ConjugateNormalLinear CreatedType;
    typedef NodeSamplerFactory BaseType;

  protected:
    static Ptr pConjugateNormalLinearFactoryInstance_;
    ConjugateNormalLinearFactory()
    {
    }

  public:
    static BaseType::Ptr Instance()
    {
      return pConjugateNormalLinearFactoryInstance_;
    }
    virtual Bool Create(const Graph & graph,
                        NodeId nodeId,
                        BaseType::CreatedPtr & pNodeSamplerInstance) const;
  };

}

#endif /* BIIPS_CONJUGATENORMALLINEAR_HPP_ */
