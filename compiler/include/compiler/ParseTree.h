//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Biips is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of Biips.
 *
 * Biips is free software: you can redistribute it and/or modify
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

/*! \file   ParseTree.h
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: copied from JAGS
 */

#ifndef PARSE_TREE_H_
#define PARSE_TREE_H_

#include <string>
#include <vector>

enum TreeClass { 
    P_VAR, P_RANGE, P_BOUNDS, P_DENSITY, P_LINK, P_COUNTER, 
    P_VALUE, P_STOCHREL, P_DETRMREL, P_FOR,  P_FUNCTION, P_RELATIONS,
    P_VECTOR, P_ARRAY, P_DIM, P_LENGTH, P_SUBSET
};

/**
 * @short Represents a model described in the BUGS language 
 * 
 * ParseTree objects are used to represent a BUGS model description
 * after it has been processed by the parser.
 *
 * ParseTree objects form a tree structure in which the parameters of
 * the node are its children.  In order to enforce a valid tree
 * structure certain restrictions are placed on the setParameters
 * function.
 *
 * ParseTree objects are designed to be dynamically allocated. A
 * ParseTree object takes ownership of its parameters and deletes them
 * when it is deleted.  Hence you only need to delete the top-level
 * ParseTree object.
 *
 */
class ParseTree {
  TreeClass const _tclass;
  std::vector<ParseTree*> _parameters;
  ParseTree const *_parent;
  std::string _name;
  double _value;
  int _line;
public:
  /**
   * Constructor, which sets the treeClass of the ParseTree node.  The
   * treeClass cannot be changed.  When a ParseTree node is first constructed,
   * it has no parent, the vector of parameters is empty, the name is an
   * empty string, the value is set to 0 and the operator is OP_NONE.
   */
  ParseTree(TreeClass tclass, int line = 0);
  /**
   * Destructor.  This deletes the parameters.
   */
  ~ParseTree();

  /**
   * Sets the name, which can later be queried with ParseTree#name.
   * The treeClass of the ParseTree node must be one of P_VAR,
   * P_COUNTER, P_FUNCTION, P_DISTRIBUTION, P_LINK, or P_ARRAY
   */
  void setName(std::string const &name);
  /**
   * Sets the value, which can later be queried with ParseTree::value().
   * The treeClass of the ParseTree node must be P_VALUE.
   */
  void setValue(double value);
  /** 
   * Sets the parameters. As a side effect, "this" becomes the parent
   * node of each of the parameters.
   *
   * Certain restrictions are required to enforce a valid tree
   * structure: "this" must not have a parent; none of the parameters
   * may have parents; "this" may not be a parameter of itself.
   *
   * NULL parameters are allowed.
   */
  void setParameters(std::vector<ParseTree *> const &parameters);
  /**
   * Returns the parameters of the ParseTree node.
   */
  std::vector<ParseTree*> const &parameters() const;
  /**
   * Returns the tree class of the ParseTree node
   */
  TreeClass treeClass() const;
  /**
   * Returns the name for a ParseTree with treeClass P_VAR, P_COUNTER,
   * P_FUNCTION, P_LINK, P_DISTRIBUTION, or P_ARRAY
   */
  std::string const &name() const;
  /**
   * Returns the value for a ParseTree with treeClass P_VALUE.
   */
  double value() const;
  /**
   * Returns the line number.
   */
  int line() const;
};

#endif /* PARSE_TREE_H_ */








