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

static Size VERBOSITY = 2;

#define CheckConsoleId(id) if ((id >= consoles.size()) || (consoles[id] == NULL))\
mexErrMsgTxt("clear_console : the console id does not exist")

template<typename StorageOrderType>
std::map<String, MultiArray> writeDataTable(const mxArray * data);


template<typename StorageOrderType>
void readDataTable(const std::map<String, MultiArray> & dataMap, mxArray ** struct_out);


void load_base_module();

#endif
