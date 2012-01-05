//                                               -*- C++ -*-
/*! \file Compiler.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate: 2011-02-16 15:14:20 +0100 (mer. 16 févr. 2011) $
 * $LastChangedRevision$
 * $Id$
 *
 * COPY: Adapted from JAGS Compiler class
 */

#ifndef BIIPS_COMPILER_HPP_
#define BIIPS_COMPILER_HPP_

#include "common/Types.hpp"
#include "common/MultiArray.hpp"
#include "common/Counter.hpp"
#include "distribution/Distribution.hpp"
#include "function/Function.hpp"
#include <map>

class ParseTree;

namespace Biips
{

  class BUGSModel;
  class IndexRange;
  class SymbolTable;

  class Compiler;
  typedef void (Compiler::*CompilerMemFn)(ParseTree const *);

  class Compiler
  {
  protected:
    BUGSModel & model_;
    std::map<String, Counter> counterMap_;
    const std::map<String, MultiArray> & dataMap_;
    std::map<String, Flags> constantMaskMap_;
    Size nResolved_, nRelations_;
    Flags isResolvedFlags_;
    Bool strictResolution_;
    Int indexExpression_;
    Types<NodeId>::Array indexNodeIds_;
    std::map<String, Types<Types<Int>::Array>::Array> nodeArrayRangesMap_;

    NodeId getArraySubset(ParseTree const * pTree);
    IndexRange variableSubsetRange(ParseTree const *var);
    IndexRange counterRange(ParseTree const *var);
    IndexRange getRange(ParseTree const * pTree,
                        const IndexRange & defaultRange);

    void traverseTree(ParseTree const * pRelations,
                      CompilerMemFn fun,
                      Bool resetCounter = true);
    void allocate(ParseTree const *pRelations);
    NodeId allocateStochastic(ParseTree const *pStochRelation);
    NodeId allocateLogical(ParseTree const *pRelation);
    void setConstantMask(ParseTree const *pRelations);
    void writeConstantData(ParseTree const *pRelations);
    NodeId getLength(ParseTree const * pTree, const SymbolTable & symtab);
    NodeId getDim(ParseTree const * pTree, const SymbolTable & symtab);
    void getArrayDim(ParseTree const * pTree);
    Bool getParameterVector(ParseTree const * pTree,
                            Types<NodeId>::Array & parents);
    NodeId constFromTable(ParseTree const * pTree);

  public:
    Bool IndexExpression(ParseTree const * p, Int & value);

    NodeId GetParameter(ParseTree const * pTree);

    /*!
     * @param model Model to be created by the compiler.
     *
     * @param dataMap Data table, mapping a variable name onto a
     * multi-dimensional array of values. This is required since some
     * constant expressions in the BUGS language may depend on data
     * values.
     */
    Compiler(BUGSModel & model, const std::map<String, MultiArray> & dataMap);

    /*!
     * Adds variables to the symbol table.
     *
     * @param pVariables vector of ParseTree pointers, each one corresponding
     * to a parsed variable declaration.
     */
    void DeclareVariables(const Types<ParseTree*>::Array & pVariables);

    /*!
     * Adds variables without an explicit declaration to the symbol
     * table. Variables supplied in the data table are added, then
     * any variables that appear on the left hand side of a relation
     *
     * @param pRelations ParseTree corresponding to a parsed model block
     */
    void UndeclaredVariables(ParseTree const *pRelations);

    /*!
     * Traverses the ParseTree creating nodes.
     *
     * @param pRelations ParseTree corresponding to a parsed model block
     */
    void WriteRelations(ParseTree const *pRelations);

    /*!
     * The function table used by the compiler to look up functions by
     * name. It is shared by all Compiler objects.
     *
     * @see Module
     */
    static FunctionTable & FuncTab();
    /*!
     * The distribution table used by the compiler to look up
     * distributions by name. It is shared by all Compiler objects.
     *
     * @see Module
     */
    static DistributionTable & DistTab();

  };

}

#endif /* BIIPS_COMPILER_HPP_ */
