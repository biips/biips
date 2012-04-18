//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
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

/*! \file Compiler.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate: 2011-02-16 15:14:20 +0100 (mer. 16 févr. 2011) $
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from JAGS Compiler class
 */

#include "compiler/Compiler.hpp"
#include "compiler/ParseTree.h"
#include "model/BUGSModel.hpp"
#include "iostream/outStream.hpp"

namespace Biips
{
  class CompileError: public RuntimeError
  {
  public:
    typedef RuntimeError BaseType;

    CompileError(ParseTree const * pTree, const String & msg) :
      BaseType(String("Compilation error on line ") + print(pTree->line())
          + ".")
    {
      if (!msg.empty())
        msg_ += "\n" + msg;
    }

    virtual ~CompileError() throw ()
    {
    }
  };

  NodeId Compiler::constFromTable(ParseTree const * pTree)
  {
    // Get a constant value directly from the data table

    if (!indexExpression_)
      throw LogicError("Can only call constFromTable inside index expression");

    std::map<String, MultiArray>::const_iterator it =
        dataMap_.find(pTree->name());
    if (it == dataMap_.end())
      return NULL_NODEID;

    const MultiArray & marray = it->second;

    IndexRange marray_range(marray.Dim());
    IndexRange subset_range = getRange(pTree, marray_range);
    if (subset_range.IsNull())
      return NULL_NODEID;

    else
    {
      // Range expression successfully evaluated
      NodeId node_id = NULL_NODEID;
      if (subset_range.Length() > 1)
      {
        IndexRangeIterator i(subset_range);
        Size n = subset_range.Length();
        ValArray::Ptr pvalues(new ValArray(n));
        for (Size j = 0; j < n; ++j, i.Next())
        {
          Size offset = marray_range.GetOffset(i);
          (*pvalues)[j] = marray.Values()[offset];
          if ((*pvalues)[j] == BIIPS_REALNA)
            return NULL_NODEID;
        }

        DimArray::Ptr p_dim(new DimArray(subset_range.Dim(false)));
        node_id = model_.GraphPtr()->AddConstantNode(p_dim, pvalues);
      }
      else
      {
        Size offset = marray_range.GetOffset(subset_range.Lower());
        ValArray::Ptr pvalues(new ValArray(1, marray.Values()[offset]));
        if (pvalues->ScalarView() == BIIPS_REALNA)
          return NULL_NODEID;
        else
          node_id = model_.GraphPtr()->AddConstantNode(P_SCALAR_DIM, pvalues);
      }
      indexNodeIds_.push_back(node_id);
      return node_id;
    }
  }

  Bool Compiler::IndexExpression(ParseTree const * pTree, Int & value)
  {
    /*
     Evaluates an index expression.

     Index expressions occur in three contexts:
     1) In the limits of a "for" loop
     2) On the left hand side of a relation
     3) On the right hand side of a relation

     They are scalar, integer-valued, constant expressions.  We
     return true on success and the result is written to the
     parameter value.
     */

    /*
     The counter indexExpression_ is non-zero if we are inside an
     Index expression. This invokes special rules in the functions
     getParameter and getArraySubset.  The counter tracks the levels
     of nesting of index expressions.

     The array indexNodeIds_ holds the Nodes created during the
     evaluation of the index expression.
     */

    indexExpression_++;
    NodeId node_id = GetParameter(pTree);
    indexExpression_--;

    if (node_id == NULL_NODEID || !model_.GraphPtr()->GetObserved()[node_id])
      return false;

    if (!model_.GraphPtr()->GetNode(node_id).Dim().IsScalar())
      throw NodeError(node_id, "Vector value in index expression");

    const ValArray::Ptr & pValues = model_.GraphPtr()->GetValues()[node_id];

    if (!pValues)
      throw NodeError(node_id, "Index value not defined.");

    if (pValues->size() != 1)
      throw NodeError(node_id,
                      String("Index expression evaluates to non-scalar value: ")
                          + print(*pValues));
    Scalar val = pValues->ScalarView();
    if (!checkInteger(val))
      throw NodeError(node_id,
                      String("Index expression evaluates to non-integer value: ")
                          + print(val));
    value = roundInteger(val);

    // destruction of the index nodes
    if (indexExpression_ == 0)
    {
      while (!indexNodeIds_.empty())
      {
        NodeId node_id = indexNodeIds_.back();

        if (node_id != model_.GraphPtr()->GetSize() - 1)
          throw LogicError("Can only remove the last inserted node in the graph.");

        model_.GraphPtr()->PopNode();
        indexNodeIds_.pop_back();
      }
    }
    return true;
  }

  IndexRange Compiler::getRange(ParseTree const * pTree,
                                const IndexRange & defaultRange)
  {
    /*
     Evaluate a range expression. If successful, it returns the range
     corresponding to the expression.  If unsuccessful (due to missing
     values) returns a null range.

     The defaultRange argument provides default values if the range
     expression is blank: e.g. foo[] or bar[,1].  The default range
     may be a null range, in which case, missing indices will result in
     failure.
     */

    const std::vector<ParseTree*> & range_list = pTree->parameters();
    const String & name = pTree->name();

    if (range_list.empty())
    {
      //An empty range expression implies the default range
      return defaultRange;
    }

    // Check size and integrity of range expression
    Size size = range_list.size();
    if (!defaultRange.IsNull() && size != defaultRange.NDim(false))
      throw CompileError(pTree, String("Dimension mismatch taking subset of ")
          + name);

    for (Size i = 0; i < size; ++i)
    {
      if (range_list[i]->treeClass() != P_RANGE)
        throw LogicError("Malformed parse tree. Expected range expression");
    }

    // Now step through and evaluate lower and upper index expressions
    IndexRange::Indices lower(size), upper(size);
    for (Size i = 0; i < size; i++)
    {
      switch (range_list[i]->parameters().size())
      {
        case 0:
          // Empty index implies default range
          if (defaultRange.IsNull())
            return defaultRange;
          lower[i] = defaultRange.Lower()[i];
          upper[i] = defaultRange.Upper()[i];
          break;
        case 1:
          // Single index implies lower == upper
          if (!IndexExpression(range_list[i]->parameters()[0], lower[i]))
            return NULL_RANGE;
          else
            upper[i] = lower[i];
          break;
        case 2:
          if (!IndexExpression(range_list[i]->parameters()[0], lower[i])
              || !IndexExpression(range_list[i]->parameters()[1], upper[i]))
            return NULL_RANGE;
          break;
        default:
          throw LogicError("Malformed parse tree in index expression");
      }
    }

    for (Size i = 0; i < size; ++i)
    {
      if (lower[i] > upper[i])
      {
        //Invalid range. We can't use the print method for Range
        //objects to print it as we can't construct a Range object.
        //So do it by hand
        std::ostringstream ostr;
        ostr << "[";
        for (Size j = 0; j < size; ++j)
        {
          if (j > 0)
            ostr << ",";
          if (lower[j] == upper[j])
            ostr << lower[j];
          else
            ostr << lower[j] << ":" << upper[j];
        }
        ostr << "]";
        throw CompileError(pTree, String("Invalid range: ") + ostr.str());
      }
    }

    return IndexRange(lower, upper);
  }

  IndexRange Compiler::variableSubsetRange(ParseTree const *var)
  {
    /*
     Get the range of a subset expression of a variable on the LHS of a
     relation.  This means that the subset expression must be constant.
     */
    if (var->treeClass() != P_VAR)
      throw LogicError("Expecting variable expression");

    const String & name = var->name();
    if (counterMap_.count(name))
      throw CompileError(var,
                         String("Counter cannot appear on LHS of relation: ")
                             + name);

    if (model_.GetSymbolTable().Contains(name))
    {
      const NodeArray & array = model_.GetSymbolTable().GetNodeArray(name);
      // It's a declared node
      const std::vector<ParseTree*> & range_list = var->parameters();

      if (range_list.empty())
      {
        //Missing range implies the whole node
        return array.Range();
      }
      if (range_list.size() != array.Range().NDim(false))
        throw CompileError(var,
                           String("Dimension mismatch in subset expression of ")
                               + name);

      IndexRange range = getRange(var, array.Range());
      if (range.IsNull())
        throw CompileError(var,
                           String("Missing values in subset expression of ")
                               + name);

      return range;
    }
    else
    {
      // Undeclared node
      IndexRange range = getRange(var, NULL_RANGE);
      if (range.IsNull())
        throw CompileError(var,
                           String("Cannot evaluate subset expression for ")
                               + name);

      return range;
    }
  }

  IndexRange Compiler::counterRange(ParseTree const *var)
  {
    /* The range expression for a counter differs from that of
     a variable in that it is
     1) one-dimensional
     2) may not be empty
     Further, no variables are created for counters in the
     Symbol Table
     */
    if (var->treeClass() != P_COUNTER)
      throw LogicError("Expecting counter expression");

    if (var->parameters().size() != 1)
      throw LogicError("Invalid counter expression");

    ParseTree const *prange = var->parameters()[0];
    if (prange->treeClass() != P_RANGE)
      throw LogicError("Expecting range expression");

    Size size = prange->parameters().size();
    if (size < 1 || size > 2)
      throw LogicError(String("Invalid range expression for counter ")
          + var->name());
    Int lower;
    if (!IndexExpression(prange->parameters()[0], lower))
      throw CompileError(var, String("Cannot evaluate lower index of counter ")
          + var->name());

    Int upper;
    if (prange->parameters().size() == 2)
    {
      if (!IndexExpression(prange->parameters()[1], upper))
        throw CompileError(var,
                           String("Cannot evaluate upper index of counter ")
                               + var->name());
    }
    else
      upper = lower;

    if (lower > upper)
      return NULL_RANGE;
    else
      return IndexRange(lower, upper);
  }

  NodeId Compiler::getArraySubset(ParseTree const * pTree)
  {
    NodeId node_id = NULL_NODEID;

    if (pTree->treeClass() != P_VAR)
      throw LogicError("Expecting expression");

    if (counterMap_.count(pTree->name()))
    {
      Counter & counter = counterMap_.at(pTree->name()); //A counter
      if (indexExpression_)
      {
        ValArray::Ptr pVal(new ValArray(1, Scalar(counter[0])));
        node_id = model_.GraphPtr()->AddConstantNode(P_SCALAR_DIM, pVal);
        indexNodeIds_.push_back(node_id);
      }
      else
      {
        // TODO use ConstantFactory
        ValArray::Ptr pVal(new ValArray(1, Scalar(counter[0])));
        node_id = model_.GraphPtr()->AddConstantNode(P_SCALAR_DIM, pVal);
      }
    }
    else
    {
      if (model_.GetSymbolTable().Contains(pTree->name()))
      {
        // FIXME SymbolTable::GetNodeArray returning non const reference ?
        const NodeArray & array =
            model_.GetSymbolTable().GetNodeArray(pTree->name());
        IndexRange subset_range = getRange(pTree, array.Range());
        if (!subset_range.IsNull())
        {
          //A fixed subset
          if (!array.Range().Contains(subset_range))
            throw CompileError(pTree, String("Subset ") + array.Name()
                + print(subset_range) + " out of range");

          //          node_id = array.GetSubset(subset_range);//, model_);
          node_id = model_.GetSymbolTable().GetNodeArraySubset(pTree->name(),
                                                               subset_range);
          if ((node_id == NULL_NODEID) && strictResolution_)
            throw CompileError(pTree, String("Unable to resolve parameter ")
                + array.Name() + print(subset_range)
                + "(one of its ancestors may be undefined)");
        }
        else if (!indexExpression_)
        {
          //A stochastic subset
          // FIXME getMixtureNode
          //node_id = getMixtureNode(pTree, this);
          throw CompileError(pTree,
                             String("Mixture nodes are not implemented yet."));
        }
      }
      else if (strictResolution_)
      {
        //Give an informative error message in case of failure
        CompileError(pTree, String("Unknown parameter ") + pTree->name());
      }

      if ((node_id == NULL_NODEID) && indexExpression_)
      {
        //It is possible to evaluate an index expression before
        //any Nodes are available from the symbol table.
        node_id = constFromTable(pTree);
      }
    }

    return node_id;
  }

  static const Function::Ptr & getFunctionPtr(ParseTree const * pTree,
                                              const FunctionTable & functab)
  {
    if (pTree->treeClass() != P_FUNCTION)
      throw LogicError("Malformed parse tree: Expected function");

    const Function::Ptr & p_func = functab.GetPtr(pTree->name());
    if (!p_func)
      throw CompileError(pTree, String("Unknown function: ") + pTree->name());

    return p_func;
  }

  NodeId Compiler::getLength(ParseTree const * pTree,
                             const SymbolTable & symtab)
  {
    if (pTree->treeClass() != P_LENGTH)
      throw LogicError("Malformed parse tree. Expecting dim expression");

    ParseTree const * var = pTree->parameters()[0];
    if (var->treeClass() != P_VAR)
      throw LogicError("Malformed parse tree. Expecting variable name");

    if (symtab.Contains(var->name()))
    {
      const NodeArray & array = symtab.GetNodeArray(var->name());
      IndexRange subset_range = getRange(var, array.Range());
      if (subset_range.IsNull())
        return NULL_NODEID;
      else
      {
        if (indexExpression_)
        {
          ValArray::Ptr pVal(new ValArray(1, Scalar(subset_range.Length())));
          NodeId node_id = model_.GraphPtr()->AddConstantNode(P_SCALAR_DIM,
                                                              pVal);
          indexNodeIds_.push_back(node_id);
          return node_id;
        }
        else
        {
          // TODO use a ConstantFactory
          ValArray::Ptr pVal(new ValArray(1, Scalar(subset_range.Length())));
          return model_.GraphPtr()->AddConstantNode(P_SCALAR_DIM, pVal);
        }
      }
    }
    else
      return NULL_NODEID;
  }

  // FIXME
  NodeId Compiler::getDim(ParseTree const * pTree, const SymbolTable & symtab)
  {
    if (pTree->treeClass() != P_DIM)
      throw LogicError("Malformed parse tree. Expecting dim expression");

    ParseTree const * var = pTree->parameters()[0];
    if (var->treeClass() != P_VAR)
      throw LogicError("Malformed parse tree. Expecting variable name");

    if (symtab.Contains(var->name()))
    {
      const NodeArray & array = symtab.GetNodeArray(var->name());
      IndexRange subset_range = getRange(var, array.Range());
      if (subset_range.IsNull())
        return NULL_NODEID;
      else
      {
        DimArray idim = subset_range.Dim(false);
        ValArray::Ptr p_val(new ValArray(idim.size()));
        for (Size j = 0; j < idim.size(); ++j)
          (*p_val)[j] = Scalar(idim[j]);

        DimArray::Ptr p_dim(new DimArray(1, idim.size()));

        if (indexExpression_)
        {
          NodeId node_id = model_.GraphPtr()->AddConstantNode(p_dim, p_val);
          indexNodeIds_.push_back(node_id);
          return node_id;
        }
        else
        {
          // TODO use a ConstantFactory
          return model_.GraphPtr()->AddConstantNode(p_dim, p_val);
        }
      }
    }
    else
      return NULL_NODEID;
  }

  /*
   * Evaluates the expression t, and returns a pointer to a Node. If the
   * expression cannot be evaluated, a NULL pointer is returned.
   */
  NodeId Compiler::GetParameter(ParseTree const * pTree)
  {
    Types<NodeId>::Array parents;
    NodeId node_id = NULL_NODEID;

    switch (pTree->treeClass())
    {
      case P_VALUE:
        if (indexExpression_)
        {
          ValArray::Ptr pVal(new ValArray(1, Scalar(pTree->value())));
          node_id = model_.GraphPtr()->AddConstantNode(P_SCALAR_DIM, pVal);
          indexNodeIds_.push_back(node_id);
        }
        else
        {
          // TODO use ConstantFactory
          ValArray::Ptr pVal(new ValArray(1, Scalar(pTree->value())));
          node_id = model_.GraphPtr()->AddConstantNode(P_SCALAR_DIM, pVal);
        }
        break;
      case P_VAR:
        node_id = getArraySubset(pTree);
        break;
      case P_LENGTH:
        node_id = getLength(pTree, model_.GetSymbolTable());
        break;
      case P_DIM:
        node_id = getDim(pTree, model_.GetSymbolTable());
        break;
      case P_LINK:
        // FIXME LinkFunction
        throw LogicError("LinkFunctions are not implemented yet.");
        //        if (GetParameterVector(pTree, parents))
        //        {
        //          LinkFunction const *link = funcTab().findLink(pTree->name());
        //          if (!link)
        //            throw CompileError(pTree, "Unknown link function:", pTree->name());
        //          node_id = _logicalfactory.getNode(FunctionPtr(link), parents, model_);
        //        }
        //        break;
      case P_FUNCTION:
        if (getParameterVector(pTree, parents))
        {
          const Function::Ptr & p_func = getFunctionPtr(pTree, FuncTab());
          if (indexExpression_)
          {
            // TODO use LogicalFactory
            node_id = model_.GraphPtr()->AddLogicalNode(p_func, parents);
            indexNodeIds_.push_back(node_id);
          }
          else
          {
            // TODO use LogicalFactory
            node_id = model_.GraphPtr()->AddLogicalNode(p_func, parents);
          }
        }
        break;
      default:
        throw LogicError("Malformed parse tree.");
        break;
    }

    if (node_id != NULL_NODEID && indexExpression_)
    {
      //Random variables in index expressions must be observed
      NodeType node_type = model_.GraphPtr()->GetNode(node_id).GetType();
      if (node_type == STOCHASTIC && !model_.GraphPtr()->GetObserved()[node_id])
        node_id = NULL_NODEID;
    }

    return node_id;
  }

  /*
   * Before creating the node y <- foo(a,b), or z ~ dfoo(a,b), the parent
   * nodes must a,b be created. This expression evaluates the vector(a,b)
   * Arguments are the same as for getParameter.
   */
  Bool Compiler::getParameterVector(ParseTree const * pTree,
                                    Types<NodeId>::Array & parents)
  {
    if (!parents.empty())
      throw LogicError("parent vector must be empty in getParameterVector");

    switch (pTree->treeClass())
    {
      case P_FUNCTION:
      case P_LINK:
      case P_DENSITY:
        for (Size i = 0; i < pTree->parameters().size(); ++i)
        {
          NodeId node_id = GetParameter(pTree->parameters()[i]);
          if (node_id != NULL_NODEID)
            parents.push_back(node_id);
          else
          {
            parents.clear();
            return false;
          }
        }
        break;
      default:
        throw LogicError("Invalid Parse Tree.");
    }
    return true;
  }

  NodeId Compiler::allocateStochastic(ParseTree const *pStochRelation)
  {
    ParseTree const *p_distribution = pStochRelation->parameters()[1];

    // Create the parameter vector
    Types<NodeId>::Array parameters;
    if (!getParameterVector(p_distribution, parameters))
      return NULL_NODEID;

    // Set upper and lower bounds
    NodeId lower_bound_id = NULL_NODEID, upper_bound_id = NULL_NODEID;
    if (pStochRelation->parameters().size() == 3)
    {
      //Truncated distribution
      const ParseTree * truncated = pStochRelation->parameters()[2];
      switch (truncated->treeClass())
      {
        case P_BOUNDS: // case P_INTERVAL: // FIXME: JAGS version > 3 seems to have a P_INTERVAL treeClass value
          break;
        default:
          throw LogicError("Invalid parse tree");
      }
      const ParseTree * lower = truncated->parameters()[0];
      const ParseTree * upper = truncated->parameters()[1];
      if (lower)
      {
        lower_bound_id = GetParameter(lower);
        if (lower_bound_id == NULL_NODEID)
          return NULL_NODEID;
      }
      if (upper)
      {
        upper_bound_id = GetParameter(upper);
        if (upper_bound_id == NULL_NODEID)
          return NULL_NODEID;
      }
    }

    /*
     Check data table to see if this is an observed node.  If it is,
     we put the data in a array of doubles pointed to by this_data,
     and set data_length equal to the length of the array
     */
    ValArray::Ptr p_this_data;
    Size data_length = 0;

    ParseTree *var = pStochRelation->parameters()[0];
    std::map<String, MultiArray>::const_iterator q = dataMap_.find(var->name());
    if (q != dataMap_.end())
    {
      const ValArray & data_value = q->second.Values();
      IndexRange data_range(q->second.Dim());

      IndexRange target_range = variableSubsetRange(var);
      data_length = target_range.Length();
      p_this_data = ValArray::Ptr(new ValArray(data_length));

      Size i = 0;
      Size nmissing = 0;
      for (IndexRangeIterator p(target_range); !p.AtEnd(); p.Next())
      {
        Size j = data_range.GetOffset(p);
        if (data_value[j] == BIIPS_REALNA)
        {
          ++nmissing;
        }
        (*p_this_data)[i++] = data_value[j];
      }
      if (nmissing == data_length)
      {
        p_this_data.reset();
        data_length = 0;
      }
      else if (nmissing != 0)
      {
        p_this_data.reset();
        throw CompileError(var, var->name() + print(target_range)
            + " has missing values");
      }
    }

    // Check that distribution exists
    const String & distname = p_distribution->name();
    const Distribution::Ptr & p_dist = DistTab().GetPtr(distname);
    if (!p_dist)
      throw CompileError(p_distribution, String("Unknown distribution: ")
          + distname);

    // FIXME Observable Functions
    //    if (!p_this_data) {
    //      /*
    //             Special rule for observable functions, which exist both as
    //             a Function and a Distribution.  If the node is unobserved,
    //             and we find a function matched to the distribution in
    //             obsFuncTab, then we create a Logical Node instead.
    //       */
    //      FunctionPtr const &func = obsFuncTab().find(dist);
    //      if (!isNULL(func)) {
    //        //FIXME: Why are we not using a factory here?
    //        LogicalNode *lnode = LogicalFactory::newNode(func, parameters);
    //        _model.addNode(lnode);
    //        return lnode;
    //      }
    //    }


    // FIXME: copied from JAGS 3.1.0 which has a P_INTERVAL treeClass value
    //    /*
    //       We allow BUGS-style interval censoring notation for
    //       compatibility but only allow it if there are no free parameters
    //       in the distribution
    //    */
    //    if (stoch_relation->parameters().size() == 3) {
    //        ParseTree const *t = stoch_relation->parameters()[2];
    //        if (t->treeClass() == P_INTERVAL) {
    //            for (unsigned int i = 0; i < parameters.size(); ++i) {
    //                if (!parameters[i]->isObserved()) {
    //                    CompileError(stoch_relation,
    //                                 "BUGS I(,) notation is not allowed unless",
    //                                 "all parameters are fixed");
    //                }
    //            }
    //        }
    //    }

    Bool obs = p_this_data;

    NodeId snode_id = model_.GraphPtr()->AddStochasticNode(p_dist,
                                                           parameters,
                                                           obs,
                                                           lower_bound_id,
                                                           upper_bound_id);

    // If Node is observed, set the data
    if (obs)
      model_.GraphPtr()->SetObsValue(snode_id, p_this_data);

    return snode_id;
  }

  NodeId Compiler::allocateLogical(ParseTree const *pRelation)
  {
    ParseTree *expression = pRelation->parameters()[1];
    NodeId node_id = NULL_NODEID;
    Types<NodeId>::Array parents;

    switch (expression->treeClass())
    {
      case P_VALUE:
      {
        ValArray::Ptr pVal(new ValArray(1, Scalar(expression->value())));
        node_id = model_.GraphPtr()->AddConstantNode(P_SCALAR_DIM, pVal);
        /* The reason we aren't using a ConstantFactory here is to ensure
         that the nodes are correctly named */
        break;
      }
      case P_VAR:
      case P_FUNCTION:
      case P_LINK:
      case P_LENGTH:
      case P_DIM:
        node_id = GetParameter(expression);
        break;
      default:
        throw LogicError("Malformed parse tree in Compiler::allocateLogical");
        break;
    }

    /*
     Check that there are no values in the data table corresponding to
     this node.
     */
    ParseTree *var = pRelation->parameters()[0];
    std::map<String, MultiArray>::const_iterator q = dataMap_.find(var->name());
    if (q != dataMap_.end())
    {
      const ValArray & data_value = q->second.Values();
      IndexRange data_range(q->second.Dim());
      IndexRange target_range = variableSubsetRange(var);

      for (IndexRangeIterator p(target_range); !p.AtEnd(); p.Next())
      {
        Size j = data_range.GetOffset(p);
        if (data_value[j] != BIIPS_REALNA)
        {
          throw CompileError(var, var->name() + print(target_range)
              + " is a logical node and cannot be observed");
        }
      }
    }

    return node_id;
  }

  void Compiler::allocate(ParseTree const *pRelations)
  {
    if (isResolvedFlags_[nRelations_])
      return;

    NodeId node_id = NULL_NODEID;

    switch (pRelations->treeClass())
    {
      case P_STOCHREL:
        node_id = allocateStochastic(pRelations);
        break;
      case P_DETRMREL:
        node_id = allocateLogical(pRelations);
        break;
      default:
        throw LogicError("Malformed parse tree in Compiler::allocate");
        break;
    }

    SymbolTable & symtab = model_.GetSymbolTable();
    if (node_id != NULL_NODEID)
    {
      ParseTree *var = pRelations->parameters()[0];
      if (!symtab.Contains(var->name()))
      {
        //Undeclared array. It's size is inferred from the dimensions of
        //the newly created node
        symtab.AddVariable(var->name(),
                           model_.GraphPtr()->GetNode(node_id).Dim());
        const NodeArray & array = symtab.GetNodeArray(var->name());
        symtab.InsertNode(node_id, var->name(), array.Range()); // TODO check this code
        //array.Insert(node_id, array.Range());
      }
      else
      {
        // Check if a node is already inserted into this range
        IndexRange range = variableSubsetRange(var);
        const NodeArray & array = symtab.GetNodeArray(var->name());
        if (array.GetNode(range) != NULL_NODEID)
        {
          throw CompileError(var, String("Attempt to redefine node")
              + var->name() + print(range));
        }
        symtab.InsertNode(node_id, var->name(), range); // TODO check this code
        //array.Insert(node_id, range);
      }
      nResolved_++;
      isResolvedFlags_[nRelations_] = true;
    }
  }

  void Compiler::setConstantMask(ParseTree const *pRelations)
  {
    ParseTree const *var = pRelations->parameters()[0];
    const String & name = var->name();
    std::map<String, Flags>::iterator p = constantMaskMap_.find(name);
    if (p == constantMaskMap_.end())
      return;

    std::map<String, MultiArray>::const_iterator q = dataMap_.find(name);
    if (q == dataMap_.end())
      throw LogicError("Error in Compiler::setConstantMask");

    IndexRange range = variableSubsetRange(var);
    const IndexRange var_range = IndexRange(q->second.Dim());
    if (!var_range.Contains(range))
      throw LogicError("Invalid range in Compiler::setConstantMask.");

    Flags & mask = p->second;
    for (IndexRangeIterator i(range); !i.AtEnd(); i.Next())
    {
      mask[var_range.GetOffset(i)] = false;
    }
  }

  void Compiler::getArrayDim(ParseTree const *pTree)
  {
    /*
     Called by traverseTree, this function calculates the size
     of all arrays from the left-hand side of all
     relations, and stores the results in the map nodeArrayRangesMap_.
     */

    ParseTree const *var = pTree->parameters()[0];
    const String & name = var->name();

    if (var->parameters().empty())
    {
      //No index expession => No info on array size
      return;
    }

    IndexRange new_range = variableSubsetRange(var);

    typedef IndexRange::Indices Indices;

    std::map<String, Types<Indices>::Array>::iterator it =
        nodeArrayRangesMap_.find(name);
    if (it == nodeArrayRangesMap_.end())
    {
      //Create a new entry
      Types<Indices>::Array ivec;
      ivec.push_back(new_range.Lower());
      ivec.push_back(new_range.Upper());
      nodeArrayRangesMap_.insert(std::make_pair(name, ivec));
    }
    else
    {
      //Check against the existing entry, and modify if necessary
      Size ndim = it->second[0].size();
      if (new_range.NDim(false) != ndim)
        throw CompileError(var, String("Inconsistent dimensiosn for array ")
            + name);

      else
      {
        for (Size j = 0; j < ndim; ++j)
        {
          it->second[0][j] = std::min(it->second[0][j], new_range.Lower()[j]);
          it->second[1][j] = std::max(it->second[1][j], new_range.Upper()[j]);
        }
      }
    }
  }

  void Compiler::writeConstantData(ParseTree const *pRelations)
  {
    /*
     Values supplied in the data table, but which DO NOT
     appear on the left-hand side of a relation, are constants.
     We have to find these values in order to create the
     constant nodes that form the top level of any graphical
     model.
     */

    //First we set up the constant mask, setting all values to true by
    //default
    for (std::map<String, MultiArray>::const_iterator it = dataMap_.begin(); it
        != dataMap_.end(); ++it)
    {
      std::pair<String, Flags> a_pair;
      a_pair.first = it->first;
      a_pair.second = Flags(it->second.Length(), true);
      constantMaskMap_.insert(a_pair);
    }

    //Now traverse the parse tree, setting node array subsets that
    //correspond to the left-hand side of any relation to be false
    traverseTree(pRelations, &Compiler::setConstantMask);

    //Create a temporary copy of the data table containing only
    //data for constant nodes
    std::map<String, MultiArray> temp_data_table;
    // Caution: the default MultiArray copy constructor is copying pointers.
    // We do not want a direct copy of dataMap_ because we would obtain copies of the pointers
    // and modifications of temp_data_table values would apply to dataMap_.
    // We need to explicitly copy the values of MultiArrays.
    for (std::map<String, MultiArray>::const_iterator it = dataMap_.begin(); it
        != dataMap_.end(); ++it)
    {
      // allocate memory
      DimArray::Ptr p_dim(new DimArray(it->second.Dim()));
      ValArray::Ptr p_val(new ValArray(it->second.Values()));
      temp_data_table.insert(std::make_pair(it->first, MultiArray(p_dim, p_val)));
    }

    for (std::map<String, MultiArray>::iterator it = temp_data_table.begin(); it
        != temp_data_table.end(); ++it)
    {
      const String & name = it->first;
      MultiArray & temp_data = it->second;
      const Flags & const_mask = constantMaskMap_.find(name)->second;
      for (Size i = 0; i < temp_data.Length(); ++i)
      {
        if (!const_mask[i])
        {
          temp_data.Values()[i] = BIIPS_REALNA;
        }
      }
    }

    model_.GetSymbolTable().WriteData(temp_data_table);
  }

  void Compiler::WriteRelations(ParseTree const *pRelations)
  {
    writeConstantData(pRelations);

    // Set up boolean vector for nodes to indicate whether they are
    // resolved or not.
    isResolvedFlags_ = Flags(nRelations_, false);

    for (Size N = nRelations_; N > 0; N -= nResolved_)
    {
      nResolved_ = 0;
      traverseTree(pRelations, &Compiler::allocate);
      if (nResolved_ == 0)
      {
        // Try again, but this time throw an exception from getSubsetNode
        strictResolution_ = true;
        traverseTree(pRelations, &Compiler::allocate);
        // If that didn't work (but it should!) just throw a generic message
        throw RuntimeError("Unable to resolve relations");
      }
    }
    isResolvedFlags_.clear();
  }

  void Compiler::traverseTree(ParseTree const * pRelations,
                              CompilerMemFn fun,
                              Bool resetCounter)
  {
    /*
     Traverse parse tree, expanding FOR loops and applying function
     fun to relations.
     */

    if (resetCounter)
      nRelations_ = 0;

    const std::vector<ParseTree*> & relation_list = pRelations->parameters();
    for (std::vector<ParseTree*>::const_iterator it_p_tree =
        relation_list.begin(); it_p_tree != relation_list.end(); ++it_p_tree)
    {
      ParseTree *var;

      switch ((*it_p_tree)->treeClass())
      {
        case P_FOR:
          var = (*it_p_tree)->parameters()[0];
          if (!counterRange(var).IsNull())
          {
            counterMap_.insert(std::make_pair(var->name(), counterRange(var)));
            for (Counter & counter = counterMap_.at(var->name()); !counter.AtEnd(); counter.Next())
            {
              traverseTree((*it_p_tree)->parameters()[1], fun, false);
            }
            counterMap_.erase(var->name());
          }
          break;
        case P_STOCHREL:
        case P_DETRMREL:
          (this->*fun)(*it_p_tree);
          nRelations_++;
          break;
        default:
          throw LogicError("Malformed parse tree in Compiler::traverseTree");
          break;
      }
    }
  }

  Compiler::Compiler(BUGSModel & model,
                     const std::map<String, MultiArray> & dataMap) :
    model_(model), dataMap_(dataMap), nResolved_(0), nRelations_(0),
        strictResolution_(false), indexExpression_(0)
  {
    if (!model_.GraphPtr()->Empty())
      throw LogicError("Non empty graph in Compiler constructor.");
    if (!model_.GetSymbolTable().Empty())
      throw LogicError("Non empty symbol table in Compiler constructor.");
  }

  void Compiler::DeclareVariables(const Types<ParseTree*>::Array & pVariables)
  {
    Types<ParseTree*>::Array::const_iterator it_p_var;
    for (it_p_var = pVariables.begin(); it_p_var != pVariables.end(); ++it_p_var)
    {
      if ((*it_p_var)->treeClass() != P_VAR)
        throw std::invalid_argument("Expected variable expression");
    }

    for (it_p_var = pVariables.begin(); it_p_var != pVariables.end(); ++it_p_var)
    {
      ParseTree const * node_dec = *it_p_var;
      const String & name = node_dec->name();
      Size ndim = node_dec->parameters().size();
      if (ndim == 0)
      {
        // Variable is scalar
        model_.GetSymbolTable().AddVariable(name, *P_SCALAR_DIM);
      }
      else
      {
        // Variable is an array
        DimArray dim(ndim);
        for (Size i = 0; i < ndim; ++i)
        {
          Int dim_i;
          if (!IndexExpression(node_dec->parameters()[i], dim_i))
            throw CompileError(node_dec,
                               String("Unable to calculate dimensions of node ")
                                   + name);
          if (dim_i <= 0)
            // FIXME
            throw CompileError(node_dec,
                               String("Non-positive dimension for node ")
                                   + name + " , dimension = " + print(dim_i));
          dim[i] = static_cast<Size> (dim_i);
        }
        model_.GetSymbolTable().AddVariable(name, dim);
      }
    }
  }

  void Compiler::UndeclaredVariables(ParseTree const *pRelations)
  {
    // Get undeclared variables from data table
    for (std::map<String, MultiArray>::const_iterator it = dataMap_.begin(); it
        != dataMap_.end(); ++it)
    {
      const String & name = it->first;
      if (model_.GetSymbolTable().Contains(name))
      {
        const NodeArray & array = model_.GetSymbolTable().GetNodeArray(name);
        if (it->second.Dim() != array.Range().Dim(false))
        {
          throw RuntimeError(String("Dimensions of ") + name
              + " in declaration (" + print(array.Range())
              + ") conflict with dimensions in data ("
              + print(IndexRange(it->second.Dim())) + ")");
        }
      }
      else
      {
        model_.GetSymbolTable().AddVariable(name, it->second.Dim());
      }
    }

    // Infer the dimension of remaining nodes from the relations
    traverseTree(pRelations, &Compiler::getArrayDim);

    typedef IndexRange::Indices Indices;
    std::map<String, Types<Indices>::Array>::const_iterator it =
        nodeArrayRangesMap_.begin();
    for (; it != nodeArrayRangesMap_.end(); ++it)
    {
      if (model_.GetSymbolTable().Contains(it->first))
      {
        //Node already declared. Check consistency
        const NodeArray & array =
            model_.GetSymbolTable().GetNodeArray(it->first);
        const Indices & upper = array.Range().Upper();
        if (upper.size() != it->second[1].size())
          throw RuntimeError(String("Dimension mismatch between data and model for node ")
              + it->first);

        for (Size j = 0; j < upper.size(); ++j)
        {
          if (it->second[1][j] > upper[j])
            throw RuntimeError(String("Index out of range for node ")
                + it->first);
        }
      }
      else
      {
        //Node not declared. Use inferred size
        const Indices & upper = it->second[1];
        Size ndim = upper.size();
        DimArray dim(ndim);
        for (Size j = 0; j < ndim; ++j)
        {
          if (upper[j] <= 0)
            throw RuntimeError(String("Invalid index for node ") + it->first);
          else
            dim[j] = static_cast<Size> (upper[j]);
        }

        model_.GetSymbolTable().AddVariable(it->first, dim);
      }
    }
  }

  FunctionTable & Compiler::FuncTab()
  {
    static FunctionTable funcTab_;
    return funcTab_;
  }

  DistributionTable & Compiler::DistTab()
  {
    static DistributionTable distTab_;
    return distTab_;
  }

}
