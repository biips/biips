#ifndef BIIPS_DISTERROR_HPP_
#define BIIPS_DISTERROR_HPP_

#include "common/Error.hpp"
#include "distribution/Distribution.hpp"

namespace Biips
{

  class DistError : public RuntimeError
  {
  public:
    typedef RuntimeError BaseType;

    DistError(const Distribution::Ptr & pDist, const String & msg)
      : BaseType(msg + " in distribution " + pDist->Name()) {}

    virtual ~DistError() throw() {}
  };

}

#endif /* BIIPS_DISTERROR_HPP_ */
