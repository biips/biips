//                                               -*- C++ -*-
/*! \file SymbolTable.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 *
 * COPY: Nearly copied and pasted from JAGS SymTab class
 */

#ifndef BIIPS_SYMBOLTABLE_HPP_
#define BIIPS_SYMBOLTABLE_HPP_

#include "common/Types.hpp"
#include "model/NodeArray.hpp"

#include <map>

namespace Biips
{

  class Model;

  class SymbolTable
  {
  protected:
    Model & model_;
    std::map<String, NodeArray::Ptr> nodeArraysMap_;

    NodeArray & getNodeArray(const String & name) { return *(nodeArraysMap_.at(name)); }

  public:
    explicit SymbolTable(Model & model);

    void AddVariable(const String & name, const DimArray & dim);

    void InsertNode(NodeId nodeId, const String & name, const IndexRange & range);

    const NodeArray & GetNodeArray(const String & name) const { return *(nodeArraysMap_.at(name)); }

    NodeId GetNodeArraySubset(const String & name, const IndexRange & subsetRange) const { return nodeArraysMap_.at(name)->GetSubset(subsetRange); }

    Bool Contains(const String & name) const { return nodeArraysMap_.count(name); }

    Bool Contains(NodeId nodeId) const;

    const String & GetVariableName(NodeId nodeId) const;

    String GetName(NodeId nodeId) const;

    Size GetSize() const { return nodeArraysMap_.size(); }

    Bool Empty() const { return nodeArraysMap_.empty(); }

    /*!
     * Writes values from the given data table to the NodeArrays in the
     * symbol table. All nodes whose values are set are considered as
     * observed nodes and have the same value in all chains.
     *
     * @param data_table Data table from which values will be read.
     *
     * @see NodeArray#setData
     */
    void WriteData(std::map<String, MultiArray> const & dataMap);
  };

}

#endif /* BIIPS_SYMBOLTABLE_HPP_ */
