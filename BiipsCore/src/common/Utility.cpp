//                                               -*- C++ -*-
/*! \file Utility.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "common/Utility.hpp"
#ifndef Win32
#include <pwd.h>
#endif

namespace Biips
{

  static String UserHOME;

  String expandFileName(const char * s)
  {
    if(s[0] != '~') return s;
    String name = s;

#ifdef Win32
    if(isalpha(s[1])) return s;
#else
    if(name.size() > 1 && s[1] != '/') return s;
#endif

    if (UserHOME.empty())
    {
      char * p = std::getenv("HOME");
      if (p)
        UserHOME = p;

#ifdef Win32
      if (UserHOME.empty())
      {
        if (p = std::getenv("USERPROFILE"))
          UserHOME = p;
      }
      if (UserHOME.empty())
      {
        if (p = std::getenv("HOMEDRIVE"))
        {
          UserHOME = p;
          p = std::getenv("HOMEPATH");
          if (p)
            UserHOME += p;
        }
      }
#else
      if (UserHOME.empty())
      {
        struct passwd * pwd = getpwuid(getuid());
        if (pwd)
          String UserHOME = pwd->pw_dir;
      }
#endif
    }

    if (!UserHOME.empty())
    {
      if (name.size() == 1)
        return UserHOME;
      else
        return UserHOME + name.substr(1);
    }
    else return name;
  }
}
