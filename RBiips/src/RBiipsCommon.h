//                                               -*- C++ -*-
/*! \file RBiipsCommon.h
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include <Rcpp.h>
#include "Rostream.h"

#ifndef RBIIPSCOMMON_H_
#define RBIIPSCOMMON_H_

#ifndef BEGIN_RBIIPS
#define BEGIN_RBIIPS BEGIN_RCPP
#endif

#ifndef CATCH_RBIIPS
#define CATCH_RBIIPS                                                    \
  }                                                                     \
  catch (Biips::RuntimeError & except)                                  \
  {                                                                     \
    ::Rf_error(except.what());                                          \
  }                                                                     \
  catch (Biips::LogicError & except)                                    \
  {                                                                     \
    ::Rf_error(except.what());
#endif

#ifndef VOID_END_RBIIPS
#define VOID_END_RBIIPS CATCH_RBIIPS VOID_END_RCPP
#endif

#ifndef END_RBIIPS
#define END_RBIIPS CATCH_RBIIPS END_RCPP
#endif

void load_base_module();


#endif /* RBIIPSCOMMON_H_ */
