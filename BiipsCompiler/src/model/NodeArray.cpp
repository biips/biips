//                                               -*- C++ -*-
/*! \file NodeArray.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate: 2011-02-28 18:15:08 +0100 (lun., 28 févr. 2011) $
 * $LastChangedRevision$
 * $Id$
 *
 * COPY: Nearly copied and pasted from JAGS NodeArray class
 */

#include "common/IndexRangeIterator.hpp"
#include "model/NodeArray.hpp"
#include "graph/Graph.hpp"
#include "print/print.hpp"

namespace Biips
{

  NodeArray::NodeArray(const String & name, Graph * pGraph, const DimArray & dim)
   : name_(name), pGraph_(pGraph), range_(dim), nodeIds_(dim.Length(), NULL_NODEID), offsets_(dim.Length(), BIIPS_SIZENA)
  {
  }

  Bool NodeArray::IsEmpty(const IndexRange & targetRange) const
  {
    // COPY: Copied an pasted from NodeArray::isEmpty function of JAGS
    // and then modified to fit Biips code
    // COPY: ********** from here **********
    if (!range_.Contains(targetRange))
      throw LogicError("Range error in NodeArray::IsEmpty");

    for (IndexRangeIterator it(targetRange); !it.AtEnd(); it.Next())
      if (offsets_.at(range_.GetOffset(it)) != BIIPS_SIZENA)
        return false;
    return true;
    // COPY: ********** to here **********
  }

  void NodeArray::Insert(NodeId nodeId, const IndexRange & targetRange)
  {
    if (nodeId >= pGraph_->GetSize() )
      throw LogicError(String("Attempt to insert non existing node at ") + name_ +
          print(targetRange));

    // COPY: Copied an pasted from NodeArray::insert function of JAGS
    // and then modified to fit Biips code
    // COPY: ********** from here **********
    if (pGraph_->GetNode(nodeId).Dim() != targetRange.Dim(true))
      throw RuntimeError(String("Cannot insert node into ") + name_ +
          print(targetRange) + ". Dimension mismatch");
    if (!range_.Contains(targetRange))
      throw RuntimeError(String("Cannot insert node into ") + name_ +
          print(targetRange) + ". Range out of bounds");
    if (!IsEmpty(targetRange))
      throw RuntimeError(String("Node ") + name_ + print(targetRange)
                          + " overlaps previously defined nodes");

    IndexRangeIterator it_j(targetRange);
    for (Size k = 0; !it_j.AtEnd(); it_j.Next(), ++k)
    {
      Size offset = range_.GetOffset(it_j);
      nodeIds_.at(offset) = nodeId;
      offsets_.at(offset) = k;
    }
    // COPY: ********** to here **********

    typedef boost::bimap<NodeId, IndexRange>::value_type Val;
    nodeIdRangeBimap_.insert(Val(nodeId, targetRange));
  }


//  Bool NodeArray::findActiveIndices(Types<Size>::Array & ind, Size k,
//      const IndexRange::Indices & lower, const DimArray & dim) const
//  {
//    // COPY: Copied an pasted from NodeArray::findActiveIndices function of JAGS
//    // and then modified to fit Biips code
//    // COPY: ********** from here **********
//
//    /*
//       We pay a heavy computational price for the flexibility of
//       allowing users to insert multivariate nodes in arbritary
//       ways into the NodeArray.
//
//       Suppose we have an array of dimension [3,4,2,5], the lower index
//       is [1,2,1,2] and the dimension of the node is [3,2]. Then the
//       node could be inserted in 5 different ways.
//
//       [1:3, 2:3, 1, 2] Active indices (0,1)
//       [1:3, 2, 1:2, 2]                (0,2)
//       [1:3, 2, 1, 2:3]                (0,3)
//       [1, 2:4, 1:2, 2]                (1,2)
//       [1, 2:4, 1, 2:3]                (1,3)
//
//       We can't have active indices (2,3) because the node won't fit
//    */
//
//    if (k == 0)
//      ind[k] = 0;
//    else
//      ind[k] = ind[k-1] + 1;
//    Size m = ind.size();
//    Size M = range_.NDim(false);
//    for (;ind[k] + m <= M + k; ind[k] += 1)
//    {
//      if (k == m - 1) {
//        IndexRange::Indices upper(lower);
//        for (Size l = 0; l < m; ++l)
//        {
//          upper[ind[l]] = upper[ind[l]] + dim[l] - 1;
//        }
//        IndexRange test_range(lower, upper);
//        if (range_.Contains(test_range))
//        {
//          NodeId nodeId = nodeIds_[range_.GetOffset(lower)];
//          Size j = 0;
//          Bool ok = true;
//          for (IndexRangeIterator i(test_range); !i.AtEnd(); i.Next(), ++j)
//          {
//            Size offset = range_.GetOffset(i);
//            if (nodeIds_[offset] != nodeId || offsets_[offset] != j)
//            {
//              ok = false;
//              break;
//            }
//          }
//          if (ok)
//            return true;
//        }
//      }
//      else {
//        if (findActiveIndices(ind, k+1, lower, dim))
//          return true;
//      }
//    }
//    return false;
//    // COPY: ********** to here **********
//  }


  NodeId NodeArray::GetSubset(const IndexRange & subsetRange)//, Model & model)
  {
    // COPY: Copied an pasted from NodeArray::getSubset function of JAGS
    // and then modified to fit Biips code
    // COPY: ********** from here **********

    //Check validity of target range
      if (!range_.Contains(subsetRange))
          throw RuntimeError(String("Cannot get subset ") + name_ +
                              print(subsetRange) + ". Range out of bounds");

    /* If range corresponds to a set node, then return this */
    NodeId node_id = GetNode(subsetRange);
    if (node_id != NULL_NODEID)
      return node_id;

    /* Otherwise create an aggregate node */
    Types<NodeId>::Array parent_node_ids;
    Types<Size>::Array offsets;
    for (IndexRangeIterator it(subsetRange); !it.AtEnd(); it.Next())
    {
      Size offset = range_.GetOffset(it);
      if (nodeIds_.at(offset) == NULL_NODEID)
        return NULL_NODEID;

      parent_node_ids.push_back(nodeIds_[offset]);
      offsets.push_back(offsets_.at(offset));
    }
    node_id = pGraph_->AddAggNode(subsetRange.Dim(true), parent_node_ids, offsets);
    //model.addNode(node_id);
    // COPY: ********** to here **********

    typedef boost::bimap<NodeId, IndexRange>::value_type Val;
    nodeIdRangeBimap_.insert(Val(node_id, subsetRange));
    return node_id;
  }


  NodeId NodeArray::GetNode(const IndexRange & targetRange) const
  {
    boost::bimap<NodeId, IndexRange>::right_const_iterator it = nodeIdRangeBimap_.right.find(targetRange);
    if (it != nodeIdRangeBimap_.right.end())
      return it->second;
    else
      return NULL_NODEID;
  }


  Bool NodeArray::Contains(NodeId nodeId) const
  {
    return nodeIdRangeBimap_.left.find(nodeId) != nodeIdRangeBimap_.left.end();
  }


  const IndexRange & NodeArray::GetRange(NodeId nodeId) const
  {
    boost::bimap<NodeId, IndexRange>::left_const_iterator it = nodeIdRangeBimap_.left.find(nodeId);
    if (it != nodeIdRangeBimap_.left.end())
      return it->second;
    else
      return NULL_RANGE;

//    // COPY: Copied an pasted from NodeArray::getRange function of JAGS
//    // and then modified to fit Biips code
//    // COPY: ********** from here **********
//
//    //Look in the generated nodes first
//    for (std::map<IndexRange, NodeId>::const_iterator p = generatedSubsets_.begin();
//        p != generatedSubsets_.end(); ++p)
//    {
//      if (nodeId == p->second)
//        return p->first;
//    }
//
//    /* Find the lower limit of the range. This is easy */
//    Size ndim = range_.NDim(false);
//    IndexRange::Indices lower(ndim);
//    Size j = 0;
//    for (; j < range_.Length(); ++j)
//    {
//      if (nodeIds_[j] == nodeId)
//      {
//        lower = range_.GetIndex(j);
//        break;
//      }
//    }
//    if (j == range_.Length())
//    {
//      return NULL_RANGE;
//    }
//
//    ndim = pGraph_->GetNode(nodeId).Dim().size();
//    Types<Size>::Array ind(ndim, 1);
//    if (findActiveIndices(ind, 0, lower, pGraph_->GetNode(nodeId).Dim()))
//    {
//      IndexRange::Indices upper = lower;
//      for (Size l = 0; l < ndim; ++l)
//      {
//        upper[ind[l]] = upper[ind[l]] + pGraph_->GetNode(nodeId).Dim()[l] - 1;
//      }
//      return IndexRange(lower, upper);
//    }
//    else
//    {
//      throw LogicError("Unable to find node range");
//    }
//    // COPY: ********** to here **********
  }

  void NodeArray::SetData(const MultiArray & value)//, Model & model)
  {
    // COPY: Copied an pasted from NodeArray::setData function of JAGS
    // and then modified to fit Biips code
    // COPY: ********** from here **********
    if (!(range_ == IndexRange(value.Dim())))
      throw RuntimeError(String("Dimension mismatch when setting value of node array ") + Name());

    Size N = value.Length();
    const ValArray & x = value.Values();

    //Gather all the nodes for which a data value is supplied
    std::set<NodeId> set_nodes;
    for (Size i = 0; i < range_.Length(); ++i)
    {
      if (x[i] != BIIPS_REALNA)
      {
        if (nodeIds_[i] == NULL_NODEID)
        {
          //Insert a new constant node
          ValArray::Ptr pVal(new ValArray(1, x[i]));
          NodeId cnode_id = pGraph_->AddConstantNode(P_SCALAR_DIM, pVal);
          //model.addNode(cnode_id);
          Insert(cnode_id, IndexRange(range_.GetIndex(i)));
        }
        else
        {
          //Existing node for which we must set value
          set_nodes.insert(nodeIds_[i]);
        }
      }
    }

    for (std::set<NodeId>::const_iterator p = set_nodes.begin();
        p != set_nodes.end(); ++p)
    {
      //Step through each node
      NodeId node_id = *p;
      Size len = pGraph_->GetNode(node_id).Dim().Length();
      ValArray::Ptr p_node_value(new ValArray(len));

      //Get vector of values for this node
      for (Size i = 0; i < N; ++i)
      {
        if (nodeIds_[i] == node_id)
        {
          if (offsets_[i] > len)
            throw LogicError("Invalid offset in NodeArray::setValue");
          else
            (*p_node_value)[offsets_[i]] = x[i];
        }
      }
      // If there are any missing values, they must all be missing
      Bool missing = ((*p_node_value)[0] == BIIPS_REALNA);
      for (Size j = 1; j < len; ++j)
      {
        if (((*p_node_value)[j] == BIIPS_REALNA) != missing)
          throw NodeError(node_id, "Values supplied for node are partially missing");
      }
      if (!missing)
      {
        pGraph_->SetObsValue(node_id, p_node_value);
      }
    }
    // COPY: ********** to here **********
  }

}