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

/*! \file FiniteSampler.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_FINITESAMPLER_HPP_
#define BIIPS_FINITESAMPLER_HPP_

#include "sampler/NodeSampler.hpp"

namespace Biips
{

  class FiniteSampler: public NodeSampler
  {
  public:
    typedef FiniteSampler SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    static const String NAME_;
    int lower_, upper_;

    friend class FiniteSamplerFactory;

    explicit FiniteSampler(const Graph & graph, int lower, int upper) :
      NodeSampler(graph), lower_(lower), upper_(upper)
    {
    }

    virtual void sample(const StochasticNode & node);

  public:
    virtual const String & Name() const
    {
      return NAME_;
    }

    virtual ~FiniteSampler()
    {
    }
  };


  class FiniteSamplerFactory: public NodeSamplerFactory
  {
  public:
    typedef FiniteSamplerFactory SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef FiniteSampler CreatedType;
    typedef NodeSamplerFactory BaseType;

  protected:
    static Ptr pFiniteSamplerFactoryInstance_;
    FiniteSamplerFactory()
    {
    }

  public:
    static BaseType::Ptr Instance()
    {
      return pFiniteSamplerFactoryInstance_;
    }
    virtual Bool Create(const Graph & graph,
                        NodeId nodeId,
                        BaseType::CreatedPtr & pNodeSamplerInstance) const;
  };

}

#endif /* BIIPS_FINITESAMPLER_HPP_ */
