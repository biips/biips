//                                               -*- C++ -*-
/*! \file printParseTree.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate: 2011-02-16 15:14:20 +0100 (mer. 16 févr. 2011) $
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_PRINTPARSETREE_HPP_
#define BIIPS_PRINTPARSETREE_HPP_


#include "compiler/ParseTree.h"
namespace Biips
{
  void printParseTree(std::ostream & os, ParseTree * const &parseTree, Size level = 0);
}


#endif /* BIIPS_PRINTPARSETREE_HPP_ */
