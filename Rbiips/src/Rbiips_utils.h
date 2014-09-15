//                                               -*- C++ -*-
/*
 * Rbiips package for GNU R is an interface to Biips C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Rbiips is derived software based on:
 * Biips, Copyright (C) Inria, 2012
 * rjags, Copyright (C) Martyn Plummer, 2002-2010
 * Rcpp, Copyright (C) Dirk Eddelbuettel and Romain Francois, 2009-2011
 *
 * This file is part of Rbiips.
 *
 * Rbiips is free software: you can redistribute it and/or modify
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
/*! \file RbiipsUtils.h
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef RBIIPS_UTILS_H_
#define RBIIPS_UTILS_H_

#include <Rcpp.h>
#include <vector>
#include <Console.hpp>
#include <common/Error.hpp>
#include "Rostream.h"
#include <common/NumArray.hpp>

#ifndef BEGIN_RBIIPS
#define BEGIN_RBIIPS BEGIN_RCPP
#endif

#ifndef CATCH_RBIIPS
#define CATCH_RBIIPS                                                    \
  }                                                                     \
  catch (Biips::RuntimeError & except)                                  \
  {                                                                     \
    forward_exception_to_r(except);                                          \
  }                                                                     \
  catch (Biips::LogicError & except)                                    \
  {                                                                     \
    forward_exception_to_r(except);
#endif

#ifndef VOID_END_RBIIPS
#define VOID_END_RBIIPS CATCH_RBIIPS VOID_END_RCPP
#endif

#ifndef END_RBIIPS
#define END_RBIIPS CATCH_RBIIPS END_RCPP
#endif

using namespace Biips;
using std::endl;

extern Size VERBOSITY;

inline void checkConsole(SEXP ptr)
{
  // FIXME
}

void load_base_module();

template<typename StorageOrderType>
std::map<String, MultiArray> writeDataTable(SEXP data);


template<typename StorageOrderType>
SEXP readDataTable(const std::map<String, MultiArray> & dataMap);


IndexRange makeRange(const Rcpp::RObject & lower,
                            const Rcpp::RObject & upper);


template<typename StorageOrderType>
SEXP getMonitors(const std::map<String, NodeArrayMonitor> & monitorsMap, const String & type);

template <class InType> 
InType apply(const std::vector<InType> & invec, const Rcpp::Function & fun, int nrhs) {
     
      InType outvec;
      switch(nrhs) {

       case 1: outvec = fun(invec[0]); 
           break;
       case 2: outvec = fun(invec[0], invec[1]);
           break;
       case 3: outvec = fun(invec[0], invec[1], invec[2]);
           break;                        
       case 4: outvec = fun(invec[0], invec[1], invec[2], invec[3]);
           break;                        
       case 5: outvec = fun(invec[0], invec[1], invec[2], invec[3], invec[4]);
           break;                              
       default: throw Biips::LogicError("Too much arguments in RFunction must be <= 5");
                break;
      }
    return outvec;
}

Rcpp::NumericVector convArrayVector(const Biips::NumArray & array );


#endif /* RBIIPS_UTILS_H_ */
