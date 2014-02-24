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

/*! \file NodeSampler.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_NODESAMPLER_HPP_
#define BIIPS_NODESAMPLER_HPP_

#include "graph/NodeVisitor.hpp"

namespace Biips
{

  class Graph;

  class NodeSampler: public ConstNodeVisitor
  {
  public:
    typedef NodeSampler SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    typedef GraphTypes::NodeValuesMap NodeValuesMap;
    typedef GraphTypes::FlagsMap FlagsMap;

    friend class GetNodeValueVisitor;

    const Graph & graph_;
    NodeValuesMap * pNodeValuesMap_;
    FlagsMap * pSampledFlagsMap_;
    Rng * pRng_;
    Scalar logIncrementalWeight_;
    Bool membersSet_;

    static const String NAME_;

    inline NodeValuesMap & nodeValuesMap()
    {
      return *pNodeValuesMap_;
    }
    inline FlagsMap & sampledFlagsMap()
    {
      return *pSampledFlagsMap_;
    }

    virtual void visit(const ConstantNode & node)
    {
    }
    virtual void visit(const LogicalNode & node);
    virtual void visit(const StochasticNode & node);
    virtual void sample(const StochasticNode & node);

  public:
    Scalar LogIncrementalWeight()
    {
      return logIncrementalWeight_;
    }

    virtual const String &
    Name() const
    {
      return NAME_;
    }

    template<typename NodeValuesMapType, typename FlagsMapType>
    void SetMembers(NodeValuesMapType & nodeValues,
                    FlagsMapType & sampledFlags,
                    Rng * pRng);
    void Sample(NodeId nodeId);

    explicit NodeSampler(const Graph & graph) :
      graph_(graph), pNodeValuesMap_(NULL), pSampledFlagsMap_(NULL),
      pRng_(NULL), logIncrementalWeight_(0.0), membersSet_(false)
    {
    }

    virtual ~NodeSampler()
    {
    }
  };

  class NodeSamplerFactory
  {
  public:
    typedef NodeSamplerFactory SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef NodeSampler CreatedType;
    typedef Types<CreatedType>::Ptr CreatedPtr;

  protected:
    static Ptr pFactoryInstance_;
    NodeSamplerFactory()
    {
    }

  public:
    static Ptr Instance()
    {
      return pFactoryInstance_;
    } // TODO throw exception
    virtual Bool Create(const Graph & graph,
                        NodeId nodeId,
                        CreatedPtr & pNodeSamplerInstance) const
    {
      pNodeSamplerInstance = CreatedPtr(new CreatedType(graph));
      return true;
    }
    virtual ~NodeSamplerFactory()
    {
    }
  };

  template<typename NodeValuesMapType, typename FlagsMapType>
  void NodeSampler::SetMembers(NodeValuesMapType & nodeValues,
                               FlagsMapType & sampledFlags,
                               Rng * pRng)
  {
    pNodeValuesMap_ = &nodeValues;
    pSampledFlagsMap_ = &sampledFlags;
    pRng_ = pRng;
    logIncrementalWeight_ = 0.0; // FIXME
    membersSet_ = true;
  }

}

#endif /* BIIPS_NODESAMPLER_HPP_ */
