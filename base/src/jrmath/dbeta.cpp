/*
 *  AUTHOR
 *    Catherine Loader, catherine@research.bell-labs.com.
 *    October 23, 2000.
 *
 *  Merge in to R:
 *	Copyright (C) 2000, The R Core Development Team
 *  Changes to case a, b < 2, use logs to avoid underflow
 *	Copyright (C) 2006, The R Core Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 *
 *
 *  DESCRIPTION
 *    Beta density,
 *                   (a+b-1)!     a-1       b-1
 *      p(x;a,b) = ------------ x     (1-x)
 *                 (a-1)!(b-1)!
 *
 *               = (a+b-1) dbinom(a-1; a+b-2,x)
 *
 *    The basic formula for the log density is thus
 *    (a-1) log x + (b-1) log (1-x) - lbeta(a, b)
 *    If either a or b <= 2 then 0 < lbeta(a, b) < 710 and so no
 *    term is large.  We use Loader's code only if both a and b > 2.
 */

#include "nmath.h"

#include "common/Types.hpp"

namespace Biips
{

double dbeta(double x, double a, double b)
{
    if (x < 0 || x > 1) return(BIIPS_NEGINF);
    if (x == 0) {
	if(a > 1) return(BIIPS_NEGINF);
	if(a < 1) return(BIIPS_POSINF);
	/* a == 1 : */ return(log(b));
    }
    if (x == 1) {
	if(b > 1) return(BIIPS_NEGINF);
	if(b < 1) return(BIIPS_POSINF);
	/* b == 1 : */ return(log(a));
    }
    if (a <= 2 || b <= 2)
	return (a-1)*log(x) + (b-1)*log1p(-x) - lgamma(a + b) - lgamma(a) - lgamma(b);
    else
	return log(a+b-1) + dbinom_raw(a-1, a+b-2, x, 1-x);
}

}
