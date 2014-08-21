//                                               -*- C++ -*-
/*! \file matbiips_utils.h
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef MATBIIPS_UTILS_H_
#define MATBIIPS_UTILS_H_

#include "mex.h"
#include <deque>
#include <Console.hpp>
#include <common/Utility.hpp>
#include "Mostream.h"
#include <cstring>
#include <cstdio>
#include "iostream/ProgressBar.hpp"
using namespace Biips;
using std::endl;

extern Size VERBOSITY;
extern Bool BASE_MODULE_LOADED;

typedef boost::shared_ptr<Console>  Console_ptr;
typedef boost::shared_ptr<ProgressBar>  ProgressBar_ptr;

extern std::deque<Console_ptr> consoles;
extern std::deque<ProgressBar_ptr> progress;

inline
void myMexErrMsg(const String & identifier, const String & err_msg) {
	mexErrMsgIdAndTxt((String("matbiips:")+identifier).c_str(), err_msg.c_str());
}

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
void CheckRhs(int nrhs, int nb, const String & name_func) {
	if (nrhs != (nb + 1)) {
		myMexErrMsg(name_func, name_func+" must have "+print(nb)+" argument(s)");
	}
}

Size GetConsoleId(const std::deque<Console_ptr> & consoles,
                   const mxArray * pm,
                   const String & name_func);

Size  GetProgressBarId(const std::deque<ProgressBar_ptr> progress,
        const mxArray * pm,
        const String & name_func);

inline
String GetString(const mxArray * pm) {
	char * c  = mxArrayToString(pm);
	String str(c);
	mxFree(c);
	return str;
}


// macros for arguments checking

#define CheckArgIsCell(nb) \
		if (!mxIsCell(prhs[(nb)]))\
		myMexErrMsg(name_func, name_func+" "+print(nb)+"-th argument must be a cell");

#define CheckArgIsString(nb) \
		if (!mxIsChar(prhs[(nb)]))\
		myMexErrMsg(name_func, name_func+" "+print(nb)+"-th argument must be a string");

#define CheckArgIsStruct(nb) \
		if (!mxIsStruct(prhs[(nb)]))\
		myMexErrMsg(name_func, name_func+" "+print(nb)+"-th argument must be a struct");

#define CheckArgIsDouble(nb) \
		if (!mxIsDouble(prhs[(nb)]))\
		myMexErrMsg(name_func, name_func+" "+print(nb)+"-th argument must be a double");

#define CheckArgIsNumeric(nb) \
        if (!mxIIsNumeric(prhs[(nb)]))\
        myMexErrMsg(name_func, name_func+" "+print(nb)+"-th argument must be numeric");

#define CheckArgIsLogical(nb) \
		if (!mxIsLogical(prhs[(nb)]))\
		myMexErrMsg(name_func, name_func+" "+print(nb)+"-th argument must be a logical");

// FIXME : not informative message
#define CheckIsString(m)\
		if (!mxIsChar((m)))\
		myMexErrMsg(name_func, name_func+": some argument must be a string");

#define CheckIsNumeric(m)\
        if (!mxIsNumeric((m)))\
        myMexErrMsg(name_func, name_func+": some argument must be numeric");

#define CheckIsDouble(m)\
		if (!mxIsDouble((m)))\
		myMexErrMsg(name_func, name_func+": some argument must be a double");

#endif /* MATBIIPS_UTILS_H_ */
