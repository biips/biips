#include "BiipsConfig.hpp"
#include "common/Utility.hpp"
#include "common/MultiArray.hpp"
#include "common/Utility.hpp"
#ifndef _WIN32
#include <pwd.h>
#endif

namespace Biips
{

  static String UserHOME;

  String expandFileName(const char * s)
  {
    if(s[0] != '~') return s;
    String name = s;

#ifdef _WIN32
    if(isalpha(s[1])) return s;
#else
    if(name.size() > 1 && s[1] != '/') return s;
#endif

    if (UserHOME.empty())
    {
      char * p = std::getenv("HOME");
      if (p)
        UserHOME = p;

#ifdef _WIN32
      if (UserHOME.empty())
      {
        if ( (p = std::getenv("USERPROFILE")) )
          UserHOME = p;
      }
      if (UserHOME.empty())
      {
        if ( (p = std::getenv("HOMEDRIVE")) )
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


  void printRecursion(std::ostream & os, const ValArray & val, const IndexRange & range, IndexRange::SizeType dim, IndexRange::Indices & indices)
  {
    typedef IndexRange::IndexType IndexType;
    typedef IndexRange::SizeType SizeType;
    typedef IndexRange::Indices Indices;

    if ( dim > 1 )
    {
      indices[dim] = range.Lower()[dim];
      while ( indices[dim] <= range.Upper()[dim] )
      {
        printRecursion(os, val, range, dim-1, indices); // here is the recursion
        ++indices[dim];
      }
    }

    else if ( dim == 1 )
    {
      if ( indices.size() > 2 )
      {
        os << "(:, :";
        for (Size i = 2; i < indices.size(); ++i)
          os << ", " << indices[i];
        os << ")" << std::endl;
      }

      indices[dim-1] = range.Lower()[dim-1];
      while ( indices[dim-1] <= range.Upper()[dim-1] )
      {
        indices[dim] = range.Lower()[dim];
        while ( indices[dim] < range.Upper()[dim] )
        {
          os << val[range.GetOffset(indices)] << " ";
          ++indices[dim];
        }
        os << val[range.GetOffset(indices)];
        if ( indices[dim-1] < range.Upper()[dim-1])
          os << std::endl;
        ++indices[dim-1];
      }
    }

    else if ( dim == 0 )
    {
      indices[dim] = range.Lower()[dim];
      while ( indices[dim] < range.Upper()[dim] )
      {
        os << val[range.GetOffset(indices)] << std::endl;
        ++indices[dim];
      }
      os << val[range.GetOffset(indices)];
    }
  }

}
