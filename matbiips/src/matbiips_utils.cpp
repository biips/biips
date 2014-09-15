//                                               -*- C++ -*-
/*! \file   matbiips_utils.cpp
 * \brief   
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "matbiips_utils.h"
#include <string>
#include <boost/typeof/typeof.hpp>
#include <compiler/Compiler.hpp>
#include <BiipsBase.hpp>

Size VERBOSITY = 1;
Bool BASE_MODULE_LOADED = false;

std::deque<Console_ptr> consoles(0);
std::deque<ProgressBar_ptr> progress(0);

Size GetConsoleId(const std::deque<Console_ptr> & consoles,
                   const mxArray * pm,
                   const String & name_func) {
  if (!mxIsNumeric(pm))
    myMexErrMsg(name_func, name_func+": the console id argument must be numeric");
  Size id = static_cast<Size>(*mxGetPr(pm));
  if ((id >= consoles.size()) || (consoles[id] == NULL)) {
    myMexErrMsg(name_func, name_func+": the console with id "+print(id)+" does not exist");
  }
  return id;
}

Size  GetProgressBarId(const std::deque<ProgressBar_ptr> progress,
                       const mxArray * pm,
                       const String & name_func) {
  if (!mxIsNumeric(pm))
    myMexErrMsg(name_func, name_func+": the progress bar id argument must be numeric");
  Size id = static_cast<Size>(*mxGetPr(pm));
  if ((id >= progress.size()) || (progress[id] == NULL)) {
    myMexErrMsg(name_func, name_func+": the progress bar with id "+print(id)+" does not exist");
  }
  return id;
}

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
                       mxGetNaN(), BIIPS_REALNA);
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
  char ** field_names= new char *[dataMap.size()];

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
    mwSize *dims = new mwSize[ndim];
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
  BASE_MODULE_LOADED = true;
}

template<>
void getMonitors<ColumnMajorOrder>(const std::map<String, NodeArrayMonitor> & monitorsMap, 
                                   const String & type, mxArray ** struct_out)
{

  mwSize dims[] = { 1 };
  char ** field_names= new char *[monitorsMap.size()];

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

    const char * curr_field_names[] = { "values", "weights", "ess", "discrete", "iterations",
                                        "conditionals", "name", "lower", "upper", "type"};
    mwSize curr_dims[] = { 1 };
    mxArray * curr_field = mxCreateStructArray(1, curr_dims, sizeof(curr_field_names)/sizeof(char *),
                                               curr_field_names);
    int field_number = mxGetFieldNumber(*struct_out, var_name.c_str());
    mxSetFieldByNumber(*struct_out, 0, field_number, curr_field);

    // retrieve dimensions

    mwSize ndim_part = monitor.GetValues().Dim().size();
    mwSize *dims_part = new mwSize[ndim_part];
    std::copy(monitor.GetValues().Dim().begin(),monitor.GetValues().Dim().end(), dims_part);

    mwSize ndim_arr = monitor.GetRange().Dim().size();
    mwSize *dims_arr = new mwSize[ndim_arr];
    std::copy(monitor.GetRange().Dim().begin(),monitor.GetRange().Dim().end(), dims_arr);

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
    const ValArray & discrete_val = monitor.GetDiscrete().Values();
    sub_field = mxCreateNumericArray(ndim_arr, dims_arr, mxDOUBLE_CLASS , mxREAL);
    std::copy(discrete_val.begin(), discrete_val.end(), mxGetPr(sub_field));
    mxSetFieldByNumber(curr_field, 0, 3, sub_field);

    // iterations assignement
    const ValArray & iter_val = monitor.GetIterations().Values() + 1;
    sub_field = mxCreateNumericArray(ndim_arr, dims_arr, mxDOUBLE_CLASS , mxREAL);
    std::copy(iter_val.begin(), iter_val.end(), mxGetPr(sub_field));
    mxSetFieldByNumber(curr_field, 0, 4, sub_field);

    //conditionals assignment
    const Types<Types<String>::Array>::Array & cond = monitor.GetConditionalNodeNames();
    Size len = monitor.GetRange().Length();
    if (cond.size() == len) {
      // one conditional per component
      sub_field = mxCreateCellArray(ndim_arr, dims_arr);
      for (Size i=0; i < len; ++i)
      {
        mwSize ndimcell = cond[i].size();
        mwSize celldims[] = { ndimcell };
        mxArray * cell = mxCreateCellArray(1, celldims);
        for (Size j=0; j<cond[i].size(); ++j)
        {
          mxArray * value = mxCreateString(cond[i][j].c_str());
          mxSetCell(cell, j, value);
        }
        mxSetCell(sub_field, i, cell);
      }
      mxSetFieldByNumber(curr_field, 0, 5, sub_field);
    }
    else if (cond.size() == 1) {
      // same conditional for each component
      mwSize ndimcell = cond[0].size();
      mwSize celldims[] = { ndimcell };
      mxArray * cell = mxCreateCellArray(1, celldims);
      for (Size j=0; j<cond[0].size(); ++j)
      {
        mxArray * value = mxCreateString(cond[0][j].c_str());
        mxSetCell(cell, j, value);
      }
      mxSetFieldByNumber(curr_field, 0, 5, cell);
    }
    else {
      myMexErrMsg("getMonitors:invalid_conditionals", "conditionals must either be of the same size as the node array or of size 1.");
    }

    //name assignment
    sub_field = mxCreateString(monitor.GetName().c_str());
    mxSetFieldByNumber(curr_field, 0, 6, sub_field);

    //lower assignement
    const IndexRange::Indices & lower_ind = monitor.GetRange().Lower();
    sub_field = mxCreateDoubleMatrix(1, lower_ind.size(), mxREAL);
    std::copy(lower_ind.begin(), lower_ind.end(), mxGetPr(sub_field));
    mxSetFieldByNumber(curr_field, 0, 7, sub_field);

    //upper assignement
    const IndexRange::Indices & upper_ind = monitor.GetRange().Upper();
    sub_field = mxCreateDoubleMatrix(1, upper_ind.size(), mxREAL);
    std::copy(upper_ind.begin(), upper_ind.end(), mxGetPr(sub_field));
    mxSetFieldByNumber(curr_field, 0, 8, sub_field);

    //type assignment
    sub_field = mxCreateString(type.c_str());
    mxSetFieldByNumber(curr_field, 0, 9, sub_field);


    delete [] dims_part;
    delete [] dims_arr;

  }

}
