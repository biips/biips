//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file Utility.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "BiipsConfig.hpp"
#include "common/Utility.hpp"
#include "common/MultiArray.hpp"
#include "common/Utility.hpp"
#ifndef WIN32
#include <pwd.h>
#endif

namespace Biips
{

  static String UserHOME;

  String expandFileName(const char * s)
  {
    if(s[0] != '~') return s;
    String name = s;

#ifdef WIN32
    if(isalpha(s[1])) return s;
#else
    if(name.size() > 1 && s[1] != '/') return s;
#endif

    if (UserHOME.empty())
    {
      char * p = std::getenv("HOME");
      if (p)
        UserHOME = p;

#ifdef WIN32
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
