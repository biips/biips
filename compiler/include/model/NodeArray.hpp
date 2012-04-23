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

/*! \file NodeArray.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate: 2011-02-28 18:15:08 +0100 (lun., 28 févr. 2011) $
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from JAGS NodeArray class
 */

#ifndef BIIPS_NODEARRAY_HPP_
#define BIIPS_NODEARRAY_HPP_

#include "common/Types.hpp"
#include "common/IndexRange.hpp"
#include <map>

#include <boost/bimap.hpp>

namespace Biips
{

  class Graph;
  class MultiArray;
  class Rng;

  class NodeArray
  {
  public:
    typedef NodeArray SelfType;
    typedef Types<NodeArray>::Ptr Ptr;

  protected:
    const String name_;
    Graph & graph_;
    IndexRange range_;
    Types<NodeId>::Array nodeIds_;
    Types<Size>::Array offsets_;
    boost::bimap<NodeId, IndexRange> nodeIdRangeBimap_;

    /* Forbid copying */
    NodeArray(const NodeArray & from);
    NodeArray & operator=(const NodeArray & rhs);

    Bool findActiveIndices(Types<Size>::Array & ind,
                           Size k,
                           const IndexRange::Indices & lower,
                           const DimArray & dim) const;

    Bool allNodesMissing() const;

  public:
    NodeArray(const String & name, Graph & graph, const DimArray & dim);

    const String & Name() const
    {
      return name_;
    }

    Bool IsEmpty(const IndexRange & targetRange) const;

    void Insert(NodeId nodeId, const IndexRange & targetRange);

    NodeId GetSubset(const IndexRange & subsetRange);

    // Accessors
    const IndexRange & Range() const
    {
      return range_;
    }

    IndexRange GetRange(NodeId nodeId) const;

    NodeId GetNode(const IndexRange & targetRange) const;

    const boost::bimap<NodeId, IndexRange> & NodeIdRangeBimap() const
    {
      return nodeIdRangeBimap_;
    }

    Bool Contains(NodeId nodeId) const;

    /**
     * Set data, creating a constant node for any non-missing value.  An
     * exception is thrown if any of the non-missing values corresponds
     * to an existing node in the array.
     */
    void SetData(const MultiArray & value);

    Bool ChangeData(const MultiArray & value,
                    std::map<Size, NodeId> & logicChildrenByRank,
                    std::map<Size, NodeId> & stoChildrenByRank,
                    Bool mcmc = true);

    Bool SampleData(std::map<Size, NodeId> & logicChildrenByRank, std::map<
        Size, NodeId> & stoChildrenByRank, Rng * pRng, Bool mcmc = true);

    /**
     * Gets the values of nodes that have been inserted into
     * the array.
     */
    MultiArray GetData() const;
  };

}

#endif /* BIIPS_NODEARRAY_HPP_ */
