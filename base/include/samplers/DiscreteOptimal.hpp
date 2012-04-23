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

/*! \file DiscreteOptimal.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DISCRETEOPTIMAL_HPP_
#define BIIPS_DISCRETEOPTIMAL_HPP_

#include "sampler/NodeSampler.hpp"

namespace Biips
{

  class DiscreteOptimal: public NodeSampler
  {
  public:
    typedef DiscreteOptimal SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    static const String NAME_;

    friend class DiscreteOptimalFactory;

    explicit DiscreteOptimal(const Graph & graph) :
      NodeSampler(graph)
    {
    }

    virtual void sample(const StochasticNode & node);

  public:
    virtual const String & Name() const
    {
      return NAME_;
    }

    virtual ~DiscreteOptimal()
    {
    }
  };

  class DiscreteOptimalFactory: public NodeSamplerFactory
  {
  public:
    typedef DiscreteOptimalFactory SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef DiscreteOptimal CreatedType;
    typedef NodeSamplerFactory BaseType;

  protected:
    static Ptr pDiscreteOptimalFactoryInstance_;
    DiscreteOptimalFactory()
    {
    }

  public:
    static BaseType::Ptr Instance()
    {
      return pDiscreteOptimalFactoryInstance_;
    }
    virtual Bool Create(const Graph & graph,
                        NodeId nodeId,
                        BaseType::CreatedPtr & pNodeSamplerInstance) const;
  };

}

#endif /* BIIPS_DISCRETEOPTIMAL_HPP_ */
