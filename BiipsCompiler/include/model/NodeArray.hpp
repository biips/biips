//                                               -*- C++ -*-
/*! \file NodeArray.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate: 2011-02-28 18:15:08 +0100 (lun., 28 févr. 2011) $
 * $LastChangedRevision$
 * $Id$
 *
 * COPY: Adapted from JAGS NodeArray class
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

    void ChangeData(const MultiArray & value);

    /**
     * Gets the values of nodes that have been inserted into
     * the array.
     */
    MultiArray GetData() const;
  };

}

#endif /* BIIPS_NODEARRAY_HPP_ */
