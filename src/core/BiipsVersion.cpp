extern "C" {
  void BiipsCore_is_present(void) {}
}

#include "BiipsConfig.hpp"
#include "BiipsVersion.hpp"
#include "common/Utility.hpp"

namespace Biips
{
  const String & BiipsVersion()
  {
    static const String VERSION = BIIPS_VERSION;
    return VERSION;
  }
}
