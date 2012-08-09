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


Size VERBOSITY = 1;
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
    std::replace_copy( m_ptr , m_ptr + m_nb_elems, p_val->begin(), 
                       std::numeric_limits<Scalar>::quiet_NaN(), BIIPS_REALNA);
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
       std::strcpy(field_names[i], var_name.c_str());
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
    std::replace_copy(values_array.Values().begin(), values_array.Values().end(), curr_field_ptr, 
                      BIIPS_REALNA, std::numeric_limits<Scalar>::quiet_NaN() );

    int field_number = mxGetFieldNumber(*struct_out, var_name.c_str());
    mxSetFieldByNumber(*struct_out, 0, field_number, curr_field);

    if (VERBOSITY>1)
      mbiips_cout << " " << var_name;
  }
  if (VERBOSITY>1)
    mbiips_cout << endl;

}

IndexRange makeRange(const mxArray * lower, const mxArray * upper)
{
  
  
  if (mxIsEmpty(lower) || mxIsEmpty(upper))
    return IndexRange();

  mwSize lowerSize = mxGetNumberOfElements(lower);
  mwSize upperSize = mxGetNumberOfElements(upper); 
  if (lowerSize != upperSize)
    throw LogicError("length mismatch between lower and upper limits");



  IndexRange::Indices lind(mxGetPr(lower), mxGetPr(lower) + lowerSize);
  IndexRange::Indices uind(mxGetPr(upper), mxGetPr(upper) + upperSize);

  IndexRange r = IndexRange(lind, uind);
  return r;
}

void load_base_module()
{
  using namespace Biips;
  loadBaseModule(Compiler::FuncTab(), Compiler::DistTab());
}

template<>
void getMonitors<ColumnMajorOrder>(const std::map<String, NodeArrayMonitor> & monitorsMap, 
                                   const String & type, mxArray ** struct_out)
{

  mwSize dims[] = { 1 };
  typedef char * chaine_carac;
  chaine_carac* field_names= new chaine_carac[monitorsMap.size()];
  
  BOOST_AUTO(it, monitorsMap.begin());
  for(int i = 0; it != monitorsMap.end(); ++it,++i) {
       const String & var_name = it->first;
       field_names[i] = new char[var_name.size()];
       std::strcpy(field_names[i], var_name.c_str());
  }

  *struct_out = mxCreateStructArray(1, dims, monitorsMap.size(), const_cast<const char **>(field_names));
  it = monitorsMap.begin();
  
  // 
  for(int i = 0; it != monitorsMap.end(); ++it, ++i) {
       delete [] field_names[i] ;
  }

  delete [] field_names;

  BOOST_AUTO(it_table, monitorsMap.begin());
  for (; it_table!=monitorsMap.end(); ++it_table)
  {
    const String & var_name = it_table->first;
    const NodeArrayMonitor & monitor = it_table->second;

    const char * curr_field_names[] = { "values", "weigths", "ess", "discrete",
                                         "name", "lower", "upper", "type"};
    mwSize curr_dims[] = { 1 };
    mxArray * curr_field = mxCreateStructArray(1, curr_dims, sizeof(curr_field_names)/sizeof(chaine_carac), 
                                               curr_field_names);
    int field_number = mxGetFieldNumber(*struct_out, var_name.c_str());
    mxSetFieldByNumber(*struct_out, 0, field_number, curr_field);

    // retrieve dimensions
    
    mwSize ndim_part = monitor.GetValues().Dim().size();
    mwSize *dims_part = new mwSize [ndim_part];
    std::copy(monitor.GetValues().Dim().begin(),monitor.GetValues().Dim().end(), dims_part);
    
    mwSize ndim_arr = monitor.GetValues().Dim().size();
    mwSize *dims_arr = new mwSize [ndim_arr];
    std::copy(monitor.GetValues().Dim().begin(),monitor.GetValues().Dim().end(), dims_arr);
    
    // values assignement 
    const ValArray & values_val = monitor.GetValues().Values();
    mxArray * sub_field = mxCreateNumericArray(ndim_part, dims_part, mxDOUBLE_CLASS , mxREAL);
    
    std::replace_copy(values_val.begin(), values_val.end(), mxGetPr(sub_field), 
                      BIIPS_REALNA, std::numeric_limits<Scalar>::quiet_NaN() ); 
    
    mxSetFieldByNumber(curr_field, 0, 0, sub_field);
     
    // weight assignement 
    const ValArray & weight_val = monitor.GetWeights().Values();
    sub_field = mxCreateNumericArray(ndim_part, dims_part, mxDOUBLE_CLASS , mxREAL);
    std::copy(weight_val.begin(), weight_val.end(), mxGetPr(sub_field));
    mxSetFieldByNumber(curr_field, 0, 1, sub_field);
   
    // ess assignement 
    const ValArray & ess_val = monitor.GetESS().Values();
    sub_field = mxCreateNumericArray(ndim_arr, dims_arr, mxDOUBLE_CLASS , mxREAL);
    std::copy(ess_val.begin(), ess_val.end(), mxGetPr(sub_field));
    mxSetFieldByNumber(curr_field, 0, 2, sub_field);

    // discrete assignement 
    const ValArray & discrete_val = monitor.GetESS().Values();
    sub_field = mxCreateNumericArray(ndim_arr, dims_arr, mxDOUBLE_CLASS , mxREAL);
    std::copy(discrete_val.begin(), discrete_val.end(), mxGetPr(sub_field));
    mxSetFieldByNumber(curr_field, 0, 3, sub_field);

    //name assignment
    mxArray * sub_name = mxCreateString(monitor.GetName().c_str());
    mxSetFieldByNumber(curr_field, 0, 4, sub_name);

    //lower assignement
    const IndexRange::Indices & lower_ind = monitor.GetRange().Lower();
    sub_field = mxCreateDoubleMatrix(1, lower_ind.size(), mxREAL);
    std::copy(lower_ind.begin(), lower_ind.end(), mxGetPr(sub_field));
    mxSetFieldByNumber(curr_field, 0, 5, sub_field);

    //upper assignement
    const IndexRange::Indices & upper_ind = monitor.GetRange().Upper();
    sub_field = mxCreateDoubleMatrix(1, upper_ind.size(), mxREAL);
    std::copy(upper_ind.begin(), upper_ind.end(), mxGetPr(sub_field));
    mxSetFieldByNumber(curr_field, 0, 6, sub_field);

    //name assignment
    sub_name = mxCreateString(type.c_str());
    mxSetFieldByNumber(curr_field, 0, 7, sub_name);

    delete [] dims_part;
    delete [] dims_arr;

  }


}  
