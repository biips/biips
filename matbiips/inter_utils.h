//                                               -*- C++ -*-
/*! \file inter_utils.h 
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef INTER_UTILS_H_
#define INTER_UTILS_H_

#include "mex.h"
#include <Console.hpp>
#include "Mostream.h"
#include <cstring>

using namespace Biips;
using std::endl;
extern Size VERBOSITY;
typedef Console * Console_ptr;



template<typename StorageOrderType>
std::map<String, MultiArray> writeDataTable(const mxArray * data);


template<typename StorageOrderType>
void readDataTable(const std::map<String, MultiArray> & dataMap, mxArray ** struct_out);


void load_base_module();

// macros for arguments checking
//
#if 1

inline  
void CheckRhs(int nrhs, int nb, String name_func) { 
  if (nrhs != (nb + 1)) { 
     char id_error[1024];
     sprintf(id_error, "inter_biips:%s", name_func.c_str());
     mexErrMsgIdAndTxt(id_error, " %s must have %d argument(s)", name_func.c_str(), nb);
  }
}

inline 
Size  GetConsoleId(const std::deque<Console_ptr> consoles, 
                   const mxArray * pm,
		   String name_func) {
   Size id = static_cast<int>(*mxGetPr(pm));
   if ((id >= consoles.size()) || (consoles[id] == NULL)) {
       char id_error[1024];
       sprintf(id_error, "inter_biips:%s", name_func.c_str());
       mexErrMsgIdAndTxt(id_error, " %s : the console with id %d does not exist", name_func.c_str(), id);
   return id;
   }
}

#define CheckArgIsCell(nb) \
 if (!mxIsCell(prhs[(nb)]))\
    mbiips_cerr << name_func  << " " << id << " -th argument must be a cell" << endl

#define CheckArgIsString(nb) \
 if (!mxIsChar(prhs[(nb)]))\
    mbiips_cerr << name_func  << " " << id << " -th argument must be a string" << endl

#define CheckArgIsStruct(nb) \
 if (!mxIsStruct(prhs[(nb)]))\
    mbiips_cerr << name_func  << " " << id << " -th argument must be a string" << endl

#define CheckIsString(m)\
 if (!mxIsChar((m)))\
    mbiips_cerr << name_func  << " " << " some argument must be a string" << endl


#endif


IndexRange makeRange(const mxArray * lower, const mxArray * upper);


#endif
