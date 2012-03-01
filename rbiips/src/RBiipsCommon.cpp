//                                               -*- C++ -*-
/*! \file RBiipsCommon.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "RBiipsCommon.h"
#include <compiler/Compiler.hpp>
#include <BiipsBase.hpp>


void load_base_module()
{
  using namespace Biips;
  BEGIN_RBIIPS
  loadBaseModule(Compiler::FuncTab(), Compiler::DistTab());
  VOID_END_RBIIPS
}

