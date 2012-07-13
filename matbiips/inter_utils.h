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
#include <string>
#include <Console.hpp>
#include "Mostream.h"
#include <boost//typeof/typeof.hpp>
using namespace Biips;
using std::endl;

static Size VERBOSITY = 2;

#define CheckConsoleId(id) if ((id >= consoles.size()) || (consoles[id] == NULL))\
mexErrMsgTxt("clear_console : the console id does not exist")

template<typename StorageOrderType>
std::map<String, MultiArray> writeDataTable(const mxArray * data);

template<>
std::map<String, MultiArray> writeDataTable<ColumnMajorOrder>(const mxArray *  data)
{
  std::map<String, MultiArray> data_map;

  if (VERBOSITY>1)
    mbiips_cout << PROMPT_STRING << "Writing data table" << endl;

  if (VERBOSITY>1)
    mbiips_cout << INDENT_STRING << "Variables:";

  for (int i=0; i< mxGetNumberOfFields(data); ++i)
  {
    String var_name = mxGetFieldNameByNumber(data, i); 
    if (VERBOSITY>1)
      mbiips_cout << " " << var_name;

    mxArray * m = mxGetFieldByNumber(data,0, i);
    double * m_ptr = mxGetPr(m);
    mwSize m_nb_elems = mxGetNumberOfElements(m);
    mwSize ndims = mxGetNumberOfDimensions(m);
    const mwSize * dims = mxGetDimensions(m);
    MultiArray marray;

    
    // we eliminate all dimension equal to one , ie squeeze (in matlab)
    std::vector<mwSize> dims_sans_un;
    
    BOOST_AUTO (equal_to_one, std::bind1st(std::equal_to<int>(),1));
    if (std::count_if(dims, dims + ndims, equal_to_one) == ndims) {
          dims_sans_un.push_back(1);
    }
    else {
         std::remove_copy_if(dims, dims + ndims, back_inserter(dims_sans_un),equal_to_one);
    }
    DimArray::Ptr p_dim(new DimArray(dims_sans_un.size()));
    std::copy(dims_sans_un.begin(), dims_sans_un.end() , p_dim->begin());
    
    ValArray::Ptr p_val(new ValArray(m_nb_elems));
    std::replace_copy( m_ptr , m_ptr + m_nb_elems, p_val->begin(), BIIPS_REALNA, BIIPS_REALNA);
    marray.SetPtr(p_dim, p_val);

    data_map[var_name] = marray;
  }
  if (VERBOSITY>1)
    mbiips_cout << endl;

  return data_map;
}

#endif
