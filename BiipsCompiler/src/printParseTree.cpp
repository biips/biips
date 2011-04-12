//                                               -*- C++ -*-
/*! \file printParseTree.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate: 2011-02-16 15:14:20 +0100 (mer. 16 févr. 2011) $
 * $LastChangedRevision$
 * $Id$
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
    return tree_class_map;
  }

  void printParseTree(std::ostream & os, ParseTree * const &parseTree, Size level)
  {
    using namespace std;
    static const Size INDENT = 4;

    static const map<TreeClass, String> & tree_class_map = treeClassMap();
    os << String(INDENT*level, ' ') << tree_class_map.at(parseTree->treeClass()) << ": ";
    switch(parseTree->treeClass()) {
    case P_VAR: case P_COUNTER: case P_FUNCTION: case P_DENSITY: case P_LINK:
    case P_ARRAY:
      os << parseTree->name();
      break;
    default:
      break;
    }
    if (parseTree->treeClass() == P_VALUE)
      os << " = " << parseTree->value();
    os << endl;
    for ( vector<ParseTree*>::const_iterator it_param = parseTree->parameters().begin(); it_param != parseTree->parameters().end(); ++it_param)
    {
      printParseTree(os, *it_param, level+1);
    }
  }
}
