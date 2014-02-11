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
#include <cstdio>
#include "iostream/ProgressBar.hpp"
using namespace Biips;
using std::endl;
extern Size VERBOSITY;
typedef boost::shared_ptr<Console>  Console_ptr;
typedef boost::shared_ptr<ProgressBar>  ProgressBar_ptr;


template<typename StorageOrderType>
std::map<String, MultiArray> writeDataTable(const mxArray * data);


template<typename StorageOrderType>
void readDataTable(const std::map<String, MultiArray> & dataMap, mxArray ** struct_out);


void load_base_module();

template<typename StorageOrderType>
void getMonitors(const std::map<String, NodeArrayMonitor> & monitorsMap, 
                 const String & type, mxArray ** struct_out);

IndexRange makeRange(const mxArray * lower, const mxArray * upper);

inline  
void CheckRhs(int nrhs, int nb, String name_func) { 
  if (nrhs != (nb + 1)) { 
     char id_error[1024];
     std::sprintf(id_error, "inter_biips:%s", name_func.c_str());
     mexErrMsgIdAndTxt(id_error, " %s must have %d argument(s)", name_func.c_str(), nb);
  }
}

inline 
Size  GetConsoleId(const std::deque<Console_ptr> consoles, 
                   const mxArray * pm,
		   String name_func) {
   Size id = static_cast<Size>(*mxGetPr(pm));
   if ((id >= consoles.size()) || (consoles[id] == NULL)) {
       char id_error[1024];
       sprintf(id_error, "inter_biips:%s", name_func.c_str());
       mexErrMsgIdAndTxt(id_error, " %s : the console with id %d does not exist", name_func.c_str(), id);
   }
   return id;
}


inline
String GetString(const mxArray * pm) {
  char * c  = mxArrayToString(pm);
  String str(c);
  return str;
}

inline 
Size  GetProgressBarId(const std::deque<ProgressBar_ptr> progress, 
                   const mxArray * pm,
		   String name_func) {
   Size id = static_cast<Size>(*mxGetPr(pm));
   if ((id >= progress.size()) || (progress[id] == NULL)) {
       char id_error[1024];
       sprintf(id_error, "inter_biips:%s", name_func.c_str());
       mexErrMsgIdAndTxt(id_error, " %s : the progress bar with id %d does not exist", name_func.c_str(), id);
   }
   return id;
}

// macros for arguments checking

// FIXME : does not throw exception
#define CheckArgIsCell(nb) \
 if (!mxIsCell(prhs[(nb)]))\
    mbiips_cerr << name_func  << " " << nb << " -th argument must be a cell" << endl

#define CheckArgIsString(nb) \
 if (!mxIsChar(prhs[(nb)]))\
    mbiips_cerr << name_func  << " " << nb << " -th argument must be a string" << endl

#define CheckArgIsStruct(nb) \
 if (!mxIsStruct(prhs[(nb)]))\
    mbiips_cerr << name_func  << " " << nb << " -th argument must be a struct" << endl

#define CheckArgIsDouble(nb) \
 if (!mxIsDouble(prhs[(nb)]))\
    mbiips_cerr << name_func  << " " << nb << " -th argument must be double (in matlab)" << endl

#define CheckArgIsLogical(nb) \
 if (!mxIsLogical(prhs[(nb)]))\
    mbiips_cerr << name_func  << " " << nb << " -th argument must be logical" << endl

// FIXME : not informative message
#define CheckIsString(m)\
 if (!mxIsChar((m)))\
    mbiips_cerr << name_func  << " " << " some argument must be a string" << endl

#define CheckIsDouble(m)\
 if (!mxIsDouble((m)))\
    mbiips_cerr << name_func  << " " <<  " some argument must be double precision" << endl

#endif /* INTER_UTILS_H_ */
