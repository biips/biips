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

using namespace Biips;
using std::endl;

typedef Console * Console_ptr;
std::deque<Console_ptr> consoles;

#define CheckConsoleId(id) if ((id >= consoles.size()) || (consoles[id] == NULL))\
mexErrMsgTxt("clear_console : the console id does not exist")

template<typename StorageOrderType>
static std::map<String, MultiArray> writeDataTable(double * data);

template<>
std::map<String, MultiArray> writeDataTable<ColumnMajorOrder>(double * data)
{
  std::map<String, MultiArray> data_map;

  if (VERBOSITY>1)
    mbiips_cout << PROMPT_STRING << "Writing data table" << endl;

  Rcpp::List data_list(data);
  if (!data_list.hasAttribute("names"))
  {
    mbiips_cerr << "Warning: Missing variable names" << endl;
    return data_map;
  }

  if (VERBOSITY>1)
    mbiips_cout << INDENT_STRING << "Variables:";

  Rcpp::CharacterVector names = data_list.attr("names");
  for (int i=0; i<names.size(); ++i)
  {
    String var_name(names[i]);
    if (VERBOSITY>1)
      mbiips_cout << " " << var_name;

    Rcpp::NumericVector r_vec = data_list[var_name];
    MultiArray marray;

    if (!r_vec.hasAttribute("dim"))
    {
      DimArray::Ptr p_dim(new DimArray(1, r_vec.size()));
      ValArray::Ptr p_val(new ValArray(r_vec.size()));
      std::replace_copy(r_vec.begin(), r_vec.end(), p_val->begin(), NA_REAL, BIIPS_REALNA);
      marray.SetPtr(p_dim, p_val);
    }
    else
    {
      Rcpp::IntegerVector r_dim = r_vec.attr("dim");
      DimArray::Ptr p_dim(new DimArray(r_dim.begin(), r_dim.end()));
      ValArray::Ptr p_val(new ValArray(r_vec.size()));
      std::replace_copy(r_vec.begin(), r_vec.end(), p_val->begin(), NA_REAL, BIIPS_REALNA);
      marray.SetPtr(p_dim, p_val);
    }

    data_map[var_name] = marray;
  }
  if (VERBOSITY>1)
    mbiips_cout << endl;

  return data_map;
}



#endif
