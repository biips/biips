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

/*! \file NodeArray.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate: 2011-02-28 18:15:08 +0100 (lun., 28 févr. 2011) $
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from JAGS NodeArray class
 */

#include "common/IndexRangeIterator.hpp"
#include "model/NodeArray.hpp"
#include "graph/Graph.hpp"
#include "iostream/outStream.hpp"

namespace Biips
{

  NodeArray::NodeArray(const String & name, Graph & graph, const DimArray & dim) :
      name_(name), graph_(graph), range_(dim), nodeIds_(dim.Length(),
                                                        NULL_NODEID), offsets_(dim.Length(),
                                                                               BIIPS_SIZENA)
  {
  }

  Bool NodeArray::IsEmpty(const IndexRange & targetRange) const
  {
    // COPY: Adapted from JAGS NodeArray::isEmpty function
    if (!range_.Contains(targetRange))
      throw LogicError("Range error in NodeArray::IsEmpty");

    for (IndexRangeIterator it(targetRange); !it.AtEnd(); it.Next())
      if (nodeIds_.at(range_.GetOffset(it)) != NULL_NODEID)
        return false;
    return true;
    // ENDCOPY
  }

  void NodeArray::Insert(NodeId nodeId, const IndexRange & targetRange)
  {
    if (nodeId >= graph_.GetSize())
      throw LogicError(String("Attempt to insert non existing node at ") + name_
                       + print(targetRange));

    // COPY: Adapted from JAGS NodeArray::insert function
    if (nodeId == NULL_NODEID)
      throw LogicError(String("Attempt to insert NULL node at ") + name_
                       + print(targetRange));
    if (graph_.GetNode(nodeId).Dim().Drop() != targetRange.Dim(true))
      throw RuntimeError(String("Cannot insert node into ") + name_
                         + print(targetRange) + ". Dimension mismatch");
    if (!range_.Contains(targetRange))
      throw RuntimeError(String("Cannot insert node into ") + name_
                         + print(targetRange) + ". Range out of bounds");
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
    // ENDCOPY

    typedef boost::bimap<NodeId, IndexRange>::value_type Val;
    nodeIdRangeBimap_.insert(Val(nodeId, targetRange));
  }

  Bool NodeArray::findActiveIndices(Types<Size>::Array & ind,
                                    Size k,
                                    const IndexRange::Indices & lower,
                                    const DimArray & dim) const
  {
    // COPY: Adapted from JAGS NodeArray::findActiveIndices function

    /*
     We pay a heavy computational price for the flexibility of
     allowing users to insert multivariate nodes in arbritary
     ways into the NodeArray.

     Suppose we have an array of dimension [3,4,2,5], the lower index
     is [1,2,1,2] and the dimension of the node is [3,2]. Then the
     node could be inserted in 5 different ways.

     [1:3, 2:3, 1, 2] Active indices (0,1)
     [1:3, 2, 1:2, 2]                (0,2)
     [1:3, 2, 1, 2:3]                (0,3)
     [1, 2:4, 1:2, 2]                (1,2)
     [1, 2:4, 1, 2:3]                (1,3)

     We can't have active indices (2,3) because the node won't fit
     */

    if (k == 0)
      ind[k] = 0;
    else
      ind[k] = ind[k - 1] + 1;
    Size m = ind.size();
    Size M = range_.NDim(false);
    for (; ind[k] + m <= M + k; ind[k] += 1)
    {
      if (k == m - 1)
      {
        IndexRange::Indices upper(lower);
        for (Size l = 0; l < m; ++l)
          upper[ind[l]] = upper[ind[l]] + dim[l] - 1;

        IndexRange test_range(lower, upper);
        if (range_.Contains(test_range))
        {
          NodeId nodeId = nodeIds_[range_.GetOffset(lower)];
          Size j = 0;
          Bool ok = true;
          for (IndexRangeIterator i(test_range); !i.AtEnd(); i.Next(), ++j)
          {
            Size offset = range_.GetOffset(i);
            if (nodeIds_[offset] != nodeId || offsets_[offset] != j)
            {
              ok = false;
              break;
            }
          }
          if (ok)
            return true;
        }
      }
      else
      {
        if (findActiveIndices(ind, k + 1, lower, dim))
          return true;
      }
    }
    return false;
    // ENDCOPY
  }

  NodeId NodeArray::GetSubset(const IndexRange & subsetRange) //, Model & model)
  {
    // COPY: Adapted from JAGS NodeArray::getSubset function

    //Check validity of target range
    if (!range_.Contains(subsetRange))
      throw RuntimeError(String("Cannot get subset ") + name_
                         + print(subsetRange) + ". Range out of bounds");

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
    DimArray::Ptr p_dim(new DimArray(subsetRange.Dim(true)));
    node_id = graph_.AddAggNode(p_dim, parent_node_ids, offsets);
    // ENDCOPY

    typedef boost::bimap<NodeId, IndexRange>::value_type Val;
    nodeIdRangeBimap_.insert(Val(node_id, subsetRange));
    return node_id;
  }

  NodeId NodeArray::GetNode(const IndexRange & targetRange) const
  {
    boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        nodeIdRangeBimap_.right.find(targetRange);
    if (it != nodeIdRangeBimap_.right.end())
      return it->second;
    else
      return NULL_NODEID;
  }

  Bool NodeArray::Contains(NodeId nodeId) const
  {
    return nodeIdRangeBimap_.left.find(nodeId) != nodeIdRangeBimap_.left.end();
  }

  IndexRange NodeArray::GetRange(NodeId nodeId) const
  {
    //Look in the generated nodes first
    boost::bimap<NodeId, IndexRange>::left_const_iterator it =
        nodeIdRangeBimap_.left.find(nodeId);
    if (it != nodeIdRangeBimap_.left.end())
      return it->second;

    // COPY: Adapted from JAGS NodeArray::getRange function

    /* Find the lower limit of the range. This is easy */
    Size ndim = range_.NDim(false);
    IndexRange::Indices lower(ndim);
    Size j = 0;
    for (; j < range_.Length(); ++j)
    {
      if (nodeIds_[j] == nodeId)
      {
        lower = range_.GetIndex(j);
        break;
      }
    }
    if (j == range_.Length())
      return NULL_RANGE;

    ndim = graph_.GetNode(nodeId).Dim().size();
    Types<Size>::Array ind(ndim, 1);
    if (findActiveIndices(ind, 0, lower, graph_.GetNode(nodeId).Dim()))
    {
      IndexRange::Indices upper = lower;
      for (Size l = 0; l < ndim; ++l)
      {
        upper[ind[l]] = upper[ind[l]] + graph_.GetNode(nodeId).Dim()[l] - 1;
      }
      return IndexRange(lower, upper);
    }
    else
    {
      throw LogicError("Unable to find node range");
    }
    // ENDCOPY
  }

  Bool NodeArray::allNodesMissing() const
  {
    for (Size i = 0; i < range_.Length(); ++i)
    {
      if (nodeIds_[i] != NULL_NODEID)
        return false;
    }
    return true;
  }

  void NodeArray::SetData(const MultiArray & value)
  {
    // check dropped dimensions
    if (value.Dim().Drop() != range_.Dim(true))
      throw RuntimeError(String("Dimension mismatch when setting value of node array ")
                         + Name());

    // first if all values are not missing
    // and no node have been inserted yet
    if (!anyMissing(value) && allNodesMissing())
    {
      //Insert a new unique constant node holding
      // all the values
      // using the dimension from the nodearray
      NodeId cnode_id = graph_.AddConstantNode(range_.DimPtr(), value.ValuesPtr());
      Insert(cnode_id, range_);
      return;
    }

    // COPY: Adapted from JAGS NodeArray::setData function
    const ValArray & x = value.Values();

    //Gather all the nodes for which a data value is supplied
    //    std::set<NodeId> set_nodes; // from JAGS 2.1
    for (Size i = 0; i < range_.Length(); ++i)
    {
      if (x[i] != BIIPS_REALNA)
      {
        if (nodeIds_[i] == NULL_NODEID)
        {
          //Insert a new constant node
          ValArray::Ptr p_val(new ValArray(1, x[i]));
          NodeId cnode_id = graph_.AddConstantNode(P_SCALAR_DIM, p_val);
          Insert(cnode_id, IndexRange(range_.GetIndex(i)));
        }
        else
        {
          // from JAGS 2.1
          //          //Existing node for which we must set value
          //          set_nodes.insert(nodeIds_[i]);
          throw LogicError("Error in NodeArray::SetData"); // from JAGS 3.1
        }
      }
    }

    // from JAGS 2.1
    //    Size N = value.Length();
    //    for (std::set<NodeId>::const_iterator p = set_nodes.begin();
    //        p != set_nodes.end(); ++p)
    //    {
    //      //Step through each node
    //      NodeId node_id = *p;
    //      Size len = graph_.GetNode(node_id).Dim().Length();
    //      ValArray::Ptr p_node_value(new ValArray(len));
    //
    //      //Get vector of values for this node
    //      for (Size i = 0; i < N; ++i)
    //      {
    //        if (nodeIds_[i] == node_id)
    //        {
    //          if (offsets_[i] > len)
    //            throw LogicError("Invalid offset in NodeArray::setValue");
    //          else
    //            (*p_node_value)[offsets_[i]] = x[i];
    //        }
    //      }
    //      // If there are any missing values, they must all be missing
    //      Bool missing = ((*p_node_value)[0] == BIIPS_REALNA);
    //      for (Size j = 1; j < len; ++j)
    //      {
    //        if (((*p_node_value)[j] == BIIPS_REALNA) != missing)
    //          throw NodeError(node_id, "Values supplied for node are partially missing");
    //      }
    //      if (!missing)
    //      {
    //        graph_.SetObsValue(node_id, p_node_value);
    //      }
    //    }
    // ENDCOPY
  }

  Bool NodeArray::ChangeData(IndexRange range,
                             const MultiArray & value,
                             std::map<Size, NodeId> & logicChildrenByRank,
                             std::map<Size, NodeId> & stoChildrenByRank,
                             Bool mcmc)
  {
    if (range.IsNull())
      range = range_;
    else if (!range_.Contains(range))
      throw RuntimeError(String("Can not change data: variable ") + Name()
                         + "does not contain subrange " + print(range));

    if (value.Dim().Drop() != range.Dim(true))
      throw RuntimeError(String("Dimension mismatch when setting value of node array ")
                         + Name());

    if (anyMissing(value))
      throw RuntimeError("Can not change data: there are missing values.");

    Bool set_observed_nodes = false;

    std::set<NodeId> changed_nodes;

    // check that nodes are valid
    if (mcmc)
    {
      NodeId id = GetNode(range);
      if (id == NULL_NODEID)
        throw RuntimeError(String("Can not change data: variable ") + Name()
                           + print(range)
                           + " does not match one node exactly.");

      changed_nodes.insert(id);

      if (graph_.GetNode(id).GetType() != STOCHASTIC)
        throw RuntimeError("Can not change data: node is not stochastic.");

      // check node is not discrete
      // current mcmc is Metropolis-Hastings
      // with gaussian random walk: impossible on discrete values
      if (graph_.GetDiscrete()[id])
        throw RuntimeError(String("Can not change data: node is discrete."));

      // TODO check that it has (indirect) unobserved stochastic children ?

      // check node is observed
      if (!graph_.GetObserved()[id])
      {
        // otherwise set observed
        graph_.SetObserved(id);
        set_observed_nodes = true;
      }
    }
    else
    {
      for (boost::bimap<NodeId, IndexRange>::const_iterator it =
          nodeIdRangeBimap_.begin(); it != nodeIdRangeBimap_.end(); ++it)
      {
        NodeId id = it->left;
        const IndexRange & sub_range = it->right;

        if (!sub_range.Overlaps(range))
          continue;

        NodeType type = graph_.GetNode(id).GetType();
        if (type == LOGICAL)
          throw RuntimeError("Can not change data: node is logical.");

        changed_nodes.insert(id);

        // check node is observed
        if (type == STOCHASTIC && !graph_.GetObserved()[id])
        {
          // otherwise set observed
          graph_.SetObserved(id);
          set_observed_nodes = true;
        }
      }
    }

    // change values
    for (IndexRangeIterator it_subrange(range); !it_subrange.AtEnd();
        it_subrange.Next())
    {
      Size offset = range_.GetOffset(it_subrange);
      NodeId id = nodeIds_[offset];

      Size sub_offset = range.GetOffset(it_subrange);
      Scalar new_val = value.Values()[sub_offset];

      (*graph_.GetValues()[id])[sub_offset] = new_val;
    }

    // get observed logical children
    // and update discreteness
    for (std::set<NodeId>::const_iterator it = changed_nodes.begin();
        it != changed_nodes.end(); ++it)
    {
      graph_.GetLogicalChildrenByRank(*it, logicChildrenByRank);
      if (!mcmc)
        graph_.UpdateDiscreteness(*it, stoChildrenByRank);
    }

    return set_observed_nodes;
  }

  void NodeArray::SampleData(IndexRange range,
                             Rng * pRng,
                             std::map<Size, NodeId> & logicChildrenByRank)
  {
    // FIXME lazy copy-paste from ChangeData method

    if (range.IsNull())
      range = range_;
    else if (!range_.Contains(range))
      throw RuntimeError(String("Can not sample data: variable ") + Name()
                         + "does not contain subrange " + print(range));

    Bool set_observed_nodes = false;

    // check that node is valid
    NodeId id = GetNode(range);
    if (id == NULL_NODEID)
      throw RuntimeError(String("Can not sample data: variable ") + Name()
                         + print(range) + " does not match one node exactly.");

    if (graph_.GetNode(id).GetType() != STOCHASTIC)
      throw RuntimeError("Can not sample data: node is not stochastic.");

    // check node is not discrete
    // current mcmc is Metropolis-Hastings
    // with gaussian random walk: impossible on discrete values
    if (graph_.GetDiscrete()[id])
      throw RuntimeError(String("Can not sample data: node is discrete."));

    // TODO check that it has (indirect) unobserved stochastic children ?

    // check node is unobserved
    if (graph_.GetObserved()[id])
      throw LogicError(String("Can not sample data: node is already observed."));

    graph_.SetObserved(id);

    // sample value
    graph_.SampleValue(id, pRng, true);

    // get observed logical children
    graph_.GetLogicalChildrenByRank(id, logicChildrenByRank);
  }

  void NodeArray::RemoveData(IndexRange range)
  {
    // FIXME lazy copy-paste from ChangeData method

    if (range.IsNull())
      range = range_;
    else if (!range_.Contains(range))
      throw RuntimeError(String("Can not sample data: variable ") + Name()
                         + "does not contain subrange " + print(range));

    std::set<NodeId> removed_nodes;

    // check that nodes are valid
    for (boost::bimap<NodeId, IndexRange>::const_iterator it =
        nodeIdRangeBimap_.begin(); it != nodeIdRangeBimap_.end(); ++it)
    {
      const IndexRange & sub_range = it->right;
      if (!range.Contains(sub_range))
      {
        if (range.Overlaps(sub_range))
          throw LogicError(String("Can not remove data: variable ") + Name()
                           + print(range)
                           + " does not contain full nodes only.");
        continue;
      }

      NodeId id = it->left;
      if (graph_.GetNode(id).GetType() != STOCHASTIC)
        throw LogicError(String("Can not remove data: node is not stochastic."));

      // check node is observed
      if (!graph_.GetObserved()[id])
        throw LogicError(String("Can not remove data: node is not observed."));

      removed_nodes.insert(id);
    }
    for (std::set<NodeId>::const_iterator it(removed_nodes.begin());
        it != removed_nodes.end(); ++it)
    {
      graph_.SetUnobserved(*it);
    }
  }

  MultiArray NodeArray::GetData() const
  {
    Size len = range_.Length();
    DimArray::Ptr p_dim(new DimArray(range_.Dim(false)));
    ValArray::Ptr p_value(new ValArray(len, BIIPS_REALNA));
    for (Size j = 0; j < len; ++j)
    {
      NodeId node_id = nodeIds_[j];
      if (node_id != NULL_NODEID && graph_.GetObserved()[node_id])
        (*p_value)[j] = (*(graph_.GetValues()[node_id]))[offsets_[j]];
    }

    return MultiArray(p_dim, p_value);
  }
}
