//                                               -*- C++ -*-
/*! \file FunctionTable.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "function/FunctionTable.hpp"

namespace Biips
{

  const Function::Ptr FunctionTable::nullFunc_;

  Bool FunctionTable::Insert(const Function::Ptr & pFunc)
  {
    // TODO check pFunc
    if (Contains(pFunc->Name()))
      return false;
    funcMap_.insert(std::make_pair(pFunc->Name(), pFunc));
    return true;
  }

  Bool FunctionTable::Contains(const String & funcName) const
  {
    return funcMap_.find(funcName) != funcMap_.end();
  }

  const Function::Ptr & FunctionTable::GetFunctionPtr(const String & funcName) const
  {
    if (Contains(funcName))
      return funcMap_.at(funcName);
    else
      return nullFunc_;
  }
}
