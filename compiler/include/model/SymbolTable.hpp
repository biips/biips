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

/*! \file SymbolTable.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from JAGS SymTab class
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

    NodeArray & getNodeArray(const String & name)
    {
      return *(nodeArraysMap_.at(name));
    }

    void update_children(const std::map<Size, NodeId> & logicChildrenByRank,
                         std::map<Size, NodeId> & stoChildrenByRank);
  public:
    explicit SymbolTable(Model & model);

    void AddVariable(const String & name, const DimArray & dim);

    void InsertNode(NodeId nodeId,
                    const String & name,
                    const IndexRange & range);

    const NodeArray & GetNodeArray(const String & name) const
    {
      return *(nodeArraysMap_.at(name));
    }

    NodeId GetNodeArraySubset(const String & name,
                              const IndexRange & subsetRange) const
    {
      return nodeArraysMap_.at(name)->GetSubset(subsetRange);
    }

    Bool Contains(const String & name) const
    {
      return nodeArraysMap_.count(name);
    }

    Bool Contains(NodeId nodeId) const;

    const String & GetVariableName(NodeId nodeId) const;

    String GetName(NodeId nodeId) const;

    Size GetSize() const
    {
      return nodeArraysMap_.size();
    }

    Bool Empty() const
    {
      return nodeArraysMap_.empty();
    }

    /**
     * Creates constant nodes in all the NodeArrays in symbol table
     * with values from the given data table.
     *
     * @param dataMap Data table from which values will be read.
     *
     * @see NodeArray#SetData
     */
    void WriteData(const std::map<String, MultiArray> & dataMap);

    Bool ChangeData(const std::map<String, MultiArray> & dataMap,
                    Bool mcmc);

    Bool SampleData(const Types<String>::Array & variableNames,
                    Rng * pRng,
                    std::map<String, MultiArray> & dataMap);

    /**
     * Reads the current value of selected nodes in the symbol table and
     * writes the result to the data table.
     *
     * @param data_table Data table to which results will be written.
     * New entries will be created for the selected nodes.  However, a
     * new entry is not created if, in the symbol table, all nodes
     * corresponding to the selection are missing. Existing entries in
     * the data table will be overwritten.
     */
    void ReadData(std::map<String, MultiArray> & dataMap) const;
  };

}

#endif /* BIIPS_SYMBOLTABLE_HPP_ */
