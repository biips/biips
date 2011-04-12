//                                               -*- C++ -*-
/*! \file FunctionTable.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_FUNCTIONTABLE_HPP_
#define BIIPS_FUNCTIONTABLE_HPP_

#include "function/Function.hpp"
#include <map>

namespace Biips
{

  // TODO Names of functions are stored twice:
  // once in the name_ field of the Function object
  // onces in the first key of the funcMap_ field
  // of the FunctionTable object.
  // This can be optimized.
  class FunctionTable
  {
  protected:
    std::map<String, Function::Ptr> funcMap_;

    static const Function::Ptr nullFunc_;

  public:
    FunctionTable() {}

    Bool Insert(const Function::Ptr & pFunc);

    Bool Contains(const String & funcName) const;

    const Function::Ptr & GetFunctionPtr(const String & funcName) const;
    const Function::Ptr & operator [] (const String & funcName) const { return GetFunctionPtr(funcName); };
  };

}

#endif /* BIIPS_FUNCTIONTABLE_HPP_ */
