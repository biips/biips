//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Biips is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of Biips.
 *
 * Biips is free software: you can redistribute it and/or modify
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

/*! \file ValArray.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include <numeric>

#include "common/ValArray.hpp"

#include <boost/accumulators/numeric/functional.hpp>

namespace Biips
{
  namespace func = boost::numeric::functional;

  ValArray ValArray::operator-() const
  {
    ValArray ans(size());
    std::transform(begin(), end(), ans.begin(), std::negate<value_type>());
    return ans;
  }
  ValArray ValArray::operator !() const
  {
    return Apply(std::logical_not<Scalar>());
  }

  ValArray & ValArray::operator +=(const ValArray & rhs)
  {
    std::transform(begin(),
                   end(),
                   rhs.begin(),
                   begin(),
                   std::plus<value_type>());
    return *this;
  }
  ValArray & ValArray::operator +=(Scalar val)
  {
    std::for_each(begin(), end(), std::bind2nd(func::plus_assign<value_type,
        const value_type>(), val));
    return *this;
  }
  ValArray & ValArray::operator -=(const ValArray & rhs)
  {
    std::transform(begin(),
                   end(),
                   rhs.begin(),
                   begin(),
                   std::minus<value_type>());
    return *this;
  }
  ValArray & ValArray::operator -=(Scalar val)
  {
    std::for_each(begin(), end(), std::bind2nd(func::minus_assign<value_type,
        const value_type>(), val));
    return *this;
  }

  ValArray & ValArray::operator *=(const ValArray & rhs)
  {
    std::transform(begin(), end(), rhs.begin(), begin(), std::multiplies<
        value_type>());
    return *this;
  }
  ValArray & ValArray::operator *=(Scalar val)
  {
    std::for_each(begin(), end(), std::bind2nd(func::multiplies_assign<
        value_type, const value_type>(), val));
    return *this;
  }
  ValArray & ValArray::operator /=(const ValArray & rhs)
  {
    std::transform(begin(), end(), rhs.begin(), begin(), std::divides<
        value_type>());
    return *this;
  }
  ValArray & ValArray::operator /=(Scalar val)
  {
    std::for_each(begin(), end(), std::bind2nd(func::divides_assign<value_type,
        const value_type>(), val));
    return *this;
  }

  ValArray ValArray::operator ==(const ValArray & rhs) const
  {
    return Apply(std::equal_to<Scalar>(), rhs);
  }
  ValArray ValArray::operator !=(const ValArray & rhs) const
  {
    return Apply(std::not_equal_to<Scalar>(), rhs);
  }
  ValArray ValArray::operator >(const ValArray & rhs) const
  {
    return Apply(std::greater<Scalar>(), rhs);
  }
  ValArray ValArray::operator <(const ValArray & rhs) const
  {
    return Apply(std::less<Scalar>(), rhs);
  }
  ValArray ValArray::operator >=(const ValArray & rhs) const
  {
    return Apply(std::greater_equal<Scalar>(), rhs);
  }
  ValArray ValArray::operator <=(const ValArray & rhs) const
  {
    return Apply(std::less_equal<Scalar>(), rhs);
  }

  ValArray ValArray::operator ==(Scalar val) const
  {
    return Apply(std::bind2nd(std::equal_to<Scalar>(), val));
  }
  ValArray ValArray::operator !=(Scalar val) const
  {
    return Apply(std::bind2nd(std::not_equal_to<Scalar>(), val));
  }
  ValArray ValArray::operator >(Scalar val) const
  {
    return Apply(std::bind2nd(std::greater<Scalar>(), val));
  }
  ValArray ValArray::operator <(Scalar val) const
  {
    return Apply(std::bind2nd(std::less<Scalar>(), val));
  }
  ValArray ValArray::operator >=(Scalar val) const
  {
    return Apply(std::bind2nd(std::greater_equal<Scalar>(), val));
  }
  ValArray ValArray::operator <=(Scalar val) const
  {
    return Apply(std::bind2nd(std::less_equal<Scalar>(), val));
  }

  ValArray ValArray::operator &&(const ValArray & rhs) const
  {
    return Apply(std::logical_and<Scalar>(), rhs);
  }
  ValArray ValArray::operator ||(const ValArray & rhs) const
  {
    return Apply(std::logical_or<Scalar>(), rhs);
  }

  ValArray ValArray::operator &&(Scalar val) const
  {
    return Apply(std::bind2nd(std::logical_and<Scalar>(), val));
  }
  ValArray ValArray::operator ||(Scalar val) const
  {
    return Apply(std::bind2nd(std::logical_or<Scalar>(), val));
  }

  Scalar ValArray::Sum() const
  {
    return std::accumulate(begin(), end(), 0.0);
  }

  ValArray operator -(Scalar val, const ValArray & rhs)
  {
    ValArray ans(rhs.size(), val);
    ans -= rhs;
    return ans;
  }

  ValArray operator /(Scalar val, const ValArray & rhs)
  {
    ValArray ans(rhs.size(), val);
    ans /= rhs;
    return ans;
  }

  ValArray operator &&(Scalar val, const ValArray & rhs)
  {
    return rhs.Apply(std::bind1st(std::logical_and<Scalar>(), val));
  }
  ValArray operator ||(Scalar val, const ValArray & rhs)
  {
    return rhs.Apply(std::bind1st(std::logical_or<Scalar>(), val));
  }

  ValArray operator ==(Scalar val, const ValArray & rhs)
  {
    return rhs.Apply(std::bind1st(std::equal_to<Scalar>(), val));
  }
  ValArray operator !=(Scalar val, const ValArray & rhs)
  {
    return rhs.Apply(std::bind1st(std::not_equal_to<Scalar>(), val));
  }
  ValArray operator >(Scalar val, const ValArray & rhs)
  {
    return rhs.Apply(std::bind1st(std::greater<Scalar>(), val));
  }
  ValArray operator <(Scalar val, const ValArray & rhs)
  {
    return rhs.Apply(std::bind1st(std::less<Scalar>(), val));
  }
  ValArray operator >=(Scalar val, const ValArray & rhs)
  {
    return rhs.Apply(std::bind1st(std::greater_equal<Scalar>(), val));
  }
  ValArray operator <=(Scalar val, const ValArray & rhs)
  {
    return rhs.Apply(std::bind1st(std::less_equal<Scalar>(), val));
  }

  inline Scalar sqrtScalar(Scalar s)
  {
    return std::sqrt(s);
  }

  std::ostream & operator << (std::ostream & os, const ValArray & val)
  {
    if (!val.empty())
    {
      os << val[0];
      for (Size i = 1; i< val.size(); ++i)
        os << ", " << val[i];
      os << std::endl;
    }
    return os;
  }
}

namespace std
{
  Biips::ValArray sqrt(const Biips::ValArray & val)
  {
    return val.Apply(&Biips::sqrtScalar);
  }
}

