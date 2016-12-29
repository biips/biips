#ifndef BIIPS_FUNCERROR_HPP_
#define BIIPS_FUNCERROR_HPP_

#include "common/Error.hpp"
#include "function/Function.hpp"

namespace Biips
{

  class FuncError : public RuntimeError
  {
  public:
    typedef RuntimeError BaseType;

    FuncError(const Function::Ptr & pFunc, const String & msg)
    : BaseType(msg + " in function " + pFunc->Name()) {}

    virtual ~FuncError() throw() {}
  };

}

#endif /* BIIPS_FUNCERROR_HPP_ */
