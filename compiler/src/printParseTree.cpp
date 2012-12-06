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

/*! \file   printParseTree.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate: 2011-02-16 15:14:20 +0100 (mer. 16 févr. 2011) $
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "common/Types.hpp"
#include "printParseTree.hpp"
#include <map>
#include <iostream>

namespace Biips
{
  static const std::map<TreeClass, String> & treeClassMap()
  {
    static std::map<TreeClass, String> tree_class_map;
    if (tree_class_map.empty())
    {
      tree_class_map[P_VAR] = "VAR";
      tree_class_map[P_RANGE] = "RANGE";
      tree_class_map[P_BOUNDS] = "BOUNDS";
      tree_class_map[P_DENSITY] = "DENSITY";
      tree_class_map[P_LINK] = "LINK";
      tree_class_map[P_COUNTER] = "COUNTER";
      tree_class_map[P_VALUE] = "VALUE";
      tree_class_map[P_STOCHREL] = "STOCHREL";
      tree_class_map[P_DETRMREL] = "DETRMREL";
      tree_class_map[P_FOR] = "FOR";
      tree_class_map[P_FUNCTION] = "FUNCTION";
      tree_class_map[P_RELATIONS] = "RELATIONS";
      tree_class_map[P_VECTOR] = "VECTOR";
      tree_class_map[P_ARRAY] = "ARRAY";
      tree_class_map[P_DIM] = "DIM";
      tree_class_map[P_LENGTH] = "LENGTH";
      tree_class_map[P_SUBSET] = "SUBSET";

    }
    return tree_class_map;
  }

  void printParseTree(std::ostream & os,
                      ParseTree * const &parseTree,
                      Size level)
  {
    using namespace std;
    static const Size INDENT = 4;

    static const map<TreeClass, String> & tree_class_map = treeClassMap();
    os << String(INDENT * level, ' ')
       << tree_class_map.at(parseTree->treeClass()) << ": ";
    switch (parseTree->treeClass())
    {
      case P_VAR:
      case P_COUNTER:
      case P_FUNCTION:
      case P_DENSITY:
      case P_LINK:
      case P_ARRAY:
        os << parseTree->name();
        break;
      default:
        break;
    }
    if (parseTree->treeClass() == P_VALUE)
      os << " = " << parseTree->value();
    os << endl;
    for (vector<ParseTree*>::const_iterator it_param =
        parseTree->parameters().begin();
        it_param != parseTree->parameters().end(); ++it_param)
    {
      printParseTree(os, *it_param, level + 1);
    }
  }
}
