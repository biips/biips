/*
 * AUTHOR
 *   Catherine Loader, catherine@research.bell-labs.com.
 *   October 23, 2000.
 *
 *  Merge in to R and further tweaks :
 *	Copyright (C) 2000, The R Core Development Team
 *	Copyright (C) 2008, The R Foundation
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
 * DESCRIPTION
 *
 *   To compute the binomial probability, call dbinom(x,n,p).
 *   This checks for argument validity, and calls dbinom_raw().
 *
 *   dbinom_raw() does the actual computation; note this is called by
 *   other functions in addition to dbinom().
 *     (1) dbinom_raw() has both p and q arguments, when one may be represented
 *         more accurately than the other (in particular, in df()).
 *     (2) dbinom_raw() does NOT check that inputs x and n are integers. This
 *         should be done in the calling function, where necessary.
 *         -- but is not the case at all when called e.g., from df() or dbeta() !
 *     (3) Also does not check for 0 <= p <= 1 and 0 <= q <= 1 or NaN's.
 *         Do this in the calling function.
 */

#include "nmath.h"

#include "common/Types.hpp"

#ifndef M_2PI
#define M_2PI       6.283185307179586476925286766559    /* 2*pi */
#endif

double
dbinom_raw(double x, double n, double p, double q)
{
    double lf, lc;

    if (p == 0) return((x == 0) ? 0 : Biips::BIIPS_NEGINF);
    if (q == 0) return((x == n) ? 0 : Biips::BIIPS_NEGINF);

    if (x == 0) {
	if(n == 0) return 0;
	lc = (p < 0.1) ? -bd0(n,n*q) - n*p : n*log(q);
	return( lc );
    }
    if (x == n) {
	lc = (q < 0.1) ? -bd0(n,n*p) - n*q : n*log(p);
	return( lc );
    }
    if (x < 0 || x > n) return( Biips::BIIPS_NEGINF );

    /* n*p or n*q can underflow to zero if n and p or q are small.  This
       used to occur in dbeta, and gives NaN as from R 2.3.0.  */
    lc = stirlerr(n) - stirlerr(x) - stirlerr(n-x) - bd0(x,n*p) - bd0(n-x,n*q);

    /* f = (M_2PI*x*(n-x))/n; could overflow or underflow */
    /* Upto R 2.7.1:
     * lf = log(M_2PI) + log(x) + log(n-x) - log(n);
     * -- following is much better for  x << n : */
    lf = log(M_2PI) + log(x) + log1p(- x/n);

    return lc - 0.5*lf;
}
