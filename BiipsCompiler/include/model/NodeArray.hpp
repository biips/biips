//                                               -*- C++ -*-
/*! \file NodeArray.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate: 2011-02-28 18:15:08 +0100 (lun., 28 févr. 2011) $
 * $LastChangedRevision$
 * $Id$
 *
 * COPY: Nearly copied and pasted from JAGS NodeArray class
 */

#ifndef BIIPS_NODEARRAY_HPP_
#define BIIPS_NODEARRAY_HPP_

#include "common/Types.hpp"
#include "common/IndexRange.hpp"

#include <boost/bimap.hpp>

namespace Biips
{

  class Graph;
  class MultiArray;

  class NodeArray
  {
  public:
    typedef NodeArray SelfType;
    typedef Types<NodeArray>::Ptr Ptr;

  protected:
    const String name_;
    Graph * pGraph_;
    IndexRange range_;
    Types<NodeId>::Array nodeIds_;
    Types<Size>::Array offsets_;
    boost::bimap<NodeId, IndexRange> nodeIdRangeBimap_;

    /* Forbid copying */
    NodeArray(const NodeArray & from);
    NodeArray & operator=(const NodeArray & rhs);

//    Bool findActiveIndices(Types<Size>::Array & ind, Size k,
//        const IndexRange::Indices & lower, const DimArray & dim) const;

  public:
    NodeArray(const String & name, Graph * pGraph, const DimArray & dim);

    const String & Name() const { return name_; }

    Bool IsEmpty(const IndexRange & targetRange) const;

    void Insert(NodeId nodeId, const IndexRange & targetRange);

    NodeId GetSubset(const IndexRange & subsetRange);//, Model & model);

    // Accessors
    const IndexRange & Range() const { return range_; }

    const IndexRange & GetRange(NodeId nodeId) const;

    NodeId GetNode(const IndexRange & targetRange) const;

    const boost::bimap<NodeId, IndexRange> & NodeIdRangeBimap() const { return nodeIdRangeBimap_; };

    Bool Contains(NodeId nodeId) const;

    /*!
     * Set data. If a value is
     * given for an index with no node, then a new ConstantNode is created
     * with that value.
     */
    void SetData(const MultiArray & value);//, Model & model);
  };

}

#endif /* BIIPS_NODEARRAY_HPP_ */
