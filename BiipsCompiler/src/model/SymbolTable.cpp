//                                               -*- C++ -*-
/*! \file SymbolTable.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 *
 * COPY: Adapted from JAGS SymTab class
 */

#include "model/SymbolTable.hpp"
#include "model/Model.hpp"
#include "iostream/outStream.hpp"
#include "model/DeparseVisitor.hpp"

namespace Biips
{

  SymbolTable::SymbolTable(Model & model)
   : model_(model)
  {
  }

  void SymbolTable::AddVariable(const String & name, const DimArray & dim)
  {
    if (nodeArraysMap_.count(name))
      throw RuntimeError(String("Name ") + name + " already in use in symbol table");

    NodeArray::Ptr p_node_array(new NodeArray(name, *(model_.GraphPtr()), dim));
    nodeArraysMap_[name] = p_node_array;
  }


  void SymbolTable::InsertNode(NodeId nodeId, const String & name, const IndexRange & range)
  {
    // check that name is an existing variable
    if (!nodeArraysMap_.count(name))
      throw LogicError(String("Name ") + name + " does not exist in the symbol table.");

    // check that nodeId is not already in an existing NodeArray
    for (std::map<String, NodeArray::Ptr>::const_iterator it_table = nodeArraysMap_.begin();
        it_table != nodeArraysMap_.end(); ++it_table)
    {
      if (it_table->second->Contains(nodeId))
        throw LogicError(String("Node ") + print(nodeId) + " already in use in variable " + it_table->first + ".");
    }

    nodeArraysMap_.at(name)->Insert(nodeId, range);
  }


  Bool SymbolTable::Contains(NodeId nodeId) const
  {
    std::map<String, NodeArray::Ptr>::const_iterator it_table = nodeArraysMap_.begin();
    for (; it_table != nodeArraysMap_.end(); ++it_table)
    {
      if (it_table->second->Contains(nodeId))
        return true;
    }

    return false;
  }


  const String & SymbolTable::GetVariableName(NodeId nodeId) const
  {
    std::map<String, NodeArray::Ptr>::const_iterator it_table = nodeArraysMap_.begin();
    for (; it_table != nodeArraysMap_.end(); ++it_table)
    {
      const NodeArray::Ptr & p_array = it_table->second;
      if (p_array->Contains(nodeId))
      {
        IndexRange node_range = p_array->GetRange(nodeId);
          return it_table->first;
      }
    }

    throw RuntimeError(String("Unable to find Node ") + print(nodeId) + " in the symbol table.");
  }

  String SymbolTable::GetName(NodeId nodeId) const
  {
    std::map<String, NodeArray::Ptr>::const_iterator it_table = nodeArraysMap_.begin();
    for (; it_table != nodeArraysMap_.end(); ++it_table)
    {
      const NodeArray::Ptr & p_array = it_table->second;
      if (p_array->Contains(nodeId))
      {
        IndexRange node_range = p_array->GetRange(nodeId);
        if (node_range == p_array->Range())
          return it_table->first;
        else
          return it_table->first + print(node_range);
      }
    }

    //Name not in symbol table: calculate name from parents
    typedef GraphTypes::ParentIterator DirectParentNodeIdIterator;
    DirectParentNodeIdIterator it_parents, it_parents_end;

    boost::tie(it_parents, it_parents_end) = model_.GraphPtr()->GetParents(nodeId);
    Size n_par = std::distance(it_parents, it_parents_end);
    Types<String>::Array par_names(n_par);
    for (Size i=0; it_parents != it_parents_end; ++it_parents, ++i)
      par_names[i] = GetName(*it_parents);

    return deparse(nodeId, *(model_.GraphPtr()), par_names);
  }


  void SymbolTable::WriteData(std::map<String, MultiArray> const & dataMap)
  {
    for(std::map<String, MultiArray>::const_iterator p(dataMap.begin());
        p != dataMap.end(); ++p)
    {
      const String & name = p->first;
      if (Contains(name))
      {
        NodeArray & array = getNodeArray(name);
        if (array.Range().Dim(false) != p->second.Dim())
          throw RuntimeError(String("Dimension mismatch in values supplied for ") + name);

        array.SetData(p->second);
      }
    }
  }


  void SymbolTable::ReadData(std::map<String, MultiArray> & dataMap) const
  {
    std::map<String, NodeArray::Ptr>::const_iterator it_table;
    for (it_table = nodeArraysMap_.begin(); it_table != nodeArraysMap_.end(); ++it_table)
    {
      /* Create a new MultiArray to hold the values from the symbol table */
      MultiArray read_values = it_table->second->GetData();
      /* Only write to the data table if we can find at least one
             non-missing value */
      if (!allMissing(read_values))
      {
        const String & name = it_table->first;
        if (dataMap.find(name) != dataMap.end())
        {
          //Replace any existing entry
          dataMap.erase(name);
        }
        dataMap.insert(std::make_pair(name, read_values));
      }
    }
  }
}
