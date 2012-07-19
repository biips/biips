//                                               -*- C++ -*-
/*! \file   inter_utils.cpp
 * \brief   
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "inter_utils.h"
#include <string>
#include <boost/typeof/typeof.hpp>
#include <compiler/Compiler.hpp>
#include <BiipsBase.hpp>


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


    DimArray::Ptr p_dim(new DimArray(ndims));
    std::copy(dims, dims + ndims , p_dim->begin());

    ValArray::Ptr p_val(new ValArray(m_nb_elems));
    std::replace_copy( m_ptr , m_ptr + m_nb_elems, p_val->begin(), std::numeric_limits<Scalar>::quiet_NaN(), BIIPS_REALNA);
    marray.SetPtr(p_dim, p_val);

    data_map[var_name] = marray;
  }
  if (VERBOSITY>1)
    mbiips_cout << endl;

  return data_map;
}


template<>
void readDataTable<ColumnMajorOrder>(const std::map<String, MultiArray> & dataMap, mxArray ** struct_out)
{
  if (VERBOSITY>1)
    mbiips_cout << PROMPT_STRING << "Reading data table" << endl;


  if (VERBOSITY>1)
    mbiips_cout << INDENT_STRING << "Variables:";


  mwSize dims[] = { 1 };
  typedef char * chaine_carac;
  chaine_carac* field_names= new chaine_carac[dataMap.size()];

  BOOST_AUTO(it, dataMap.begin());
  for(int i = 0; it != dataMap.end(); ++it,++i) {
       const String & var_name = it->first;
       field_names[i] = new char[var_name.size()];
       strcpy(field_names[i], var_name.c_str());
  }

  *struct_out = mxCreateStructArray(1, dims, dataMap.size(), const_cast<const char **>(field_names));
  it = dataMap.begin();
  for(int i = 0; it != dataMap.end(); ++it, ++i) {
       delete [] field_names[i] ;
  }

  delete [] field_names;

  BOOST_AUTO(it_table, dataMap.begin());
  for (; it_table!=dataMap.end(); ++it_table)
  {
    const String & var_name = it_table->first;
    const MultiArray & values_array = it_table->second;

    mwSize ndim = values_array.Dim().size();
    mwSize *dims = new mwSize [ndim];
    std::copy(values_array.Dim().begin(), values_array.Dim().end(), dims);
    mxArray * curr_field = mxCreateNumericArray(ndim, dims, mxDOUBLE_CLASS , mxREAL);

    delete [] dims;

    double * curr_field_ptr = mxGetPr(curr_field);
    std::replace_copy(values_array.Values().begin(), values_array.Values().end(), curr_field_ptr, BIIPS_REALNA, std::numeric_limits<Scalar>::quiet_NaN() );

    int field_number = mxGetFieldNumber(*struct_out, var_name.c_str());
    mxSetFieldByNumber(*struct_out, 0, field_number, curr_field);

    if (VERBOSITY>1)
      mbiips_cout << " " << var_name;
  }
  if (VERBOSITY>1)
    mbiips_cout << endl;

}


void load_base_module()
{
  using namespace Biips;
  loadBaseModule(Compiler::FuncTab(), Compiler::DistTab());
}
