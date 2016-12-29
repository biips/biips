#include "storeUnregistered.hpp"
#include "common/Error.hpp"

#include <iostream>

using std::cerr;
using std::endl;
using std::istringstream;

namespace Biips
{
  const char BIIPSTEST_INPUT_VALUES_SEPARATOR = ' ';
  const char BIIPSTEST_INPUT_COLUMN_SEPARATOR = ';';
}

void storeUnregistered(const vector<po::parsed_options> & parsed_sources,
                       const vector<string> & sources_names,
                       vector<string> & monitor,
                       StoredDimMap & dim_map,
                       StoredDataMap & data_map,
                       Scalar & log_norm_const_bench,
                       StoredDataMap & bench_filter_map,
                       StoredDataMap & bench_smooth_map,
                       StoredErrorsMap & errors_filter_map,
                       StoredErrorsMap & errors_smooth_map)
{
  using namespace Biips;

  // Store unregistered
  map<string, Flags> data_dim_defined_map;
  map<string, Flags> bench_filter_dim_defined_map;
  map<string, Flags> bench_smooth_dim_defined_map;

  DimArray::Ptr p_scalar_dim(new DimArray(1, 1));

  for (Size i_src = 0; i_src < parsed_sources.size(); ++i_src)
  {
    const String & source = sources_names[i_src];
    const po::parsed_options & parsed = parsed_sources[i_src];
    for (Size op = 0; op < parsed.options.size(); ++op)
    {
      if (!parsed.options[op].unregistered)
        continue;

      string key = parsed.options[op].string_key;

      if (key == "monitor")
      {
        string var_name;
        for (Size v = 0; v < parsed.options[op].value.size(); ++v)
        {
          istringstream iss(parsed.options[op].value[v]);
          while (iss.good())
          {
            getline(iss, var_name, BIIPSTEST_INPUT_VALUES_SEPARATOR);
            if (var_name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._")
                != string::npos)
              boost::throw_exception(po::error(var_name
                  + " is not a valid variable name."));
            if (var_name.find_first_of("0123456789._") == 0)
              boost::throw_exception(po::error(var_name
                  + " is not a valid variable name."));
            monitor.push_back(var_name);
          }
        }
        continue;
      }

      string::size_type pos;
      if ((pos = key.find('.')) == string::npos)
      {
        cerr << "in source: " << source << ". ";
        boost::throw_exception(po::unknown_option(key));
      }
      if (parsed.options[op].value.empty())
      {
        cerr << "in source: " << source << ". ";
        throw std::runtime_error(String("no value given in '") + key + "'");
      }

      string section = key.substr(0, pos);
      string var = key.substr(pos + 1);

      // dim section
      if (section == "dim")
      {
        if (dim_map.count(var))
        {
          cerr << "in source: " << source << ". ";
          cerr << "Warning: option " << key << " already has value." << endl;
        }
        else
        {
          String value_str;
          Size value;
          vector<Size> dim_vec;
          for (Size v = 0; v < parsed.options[op].value.size(); ++v)
          {
            istringstream iss(parsed.options[op].value[v]);
            while (iss.good())
            {
              getline(iss, value_str, BIIPSTEST_INPUT_VALUES_SEPARATOR);
              istringstream(value_str) >> value;
              dim_vec.push_back(value);
            }
            dim_map[var] = DimArray::Ptr(new DimArray(dim_vec.begin(),
                                                      dim_vec.end()));
          }
        }
        continue;
      }

      map<string, Flags> * p_dim_defined_map = NULL;
      map<string, vector<MultiArray> > * p_datatype_map = NULL;
      map<string, map<Size, vector<Scalar> > > * p_errors_map = NULL;

      // data section
      if (section == "data")
      {
        p_dim_defined_map = &data_dim_defined_map;
        p_datatype_map = &data_map;
      }

      // bench and errors sections

      else if (section == "bench" || section == "errors")
      {
        // check if multiple values
        if (var == "log-norm-const")
        {
          istringstream(parsed.options[op].value[0]) >> log_norm_const_bench;
          continue;
        }

        if ((pos = var.find('.')) == string::npos)
        {
          cerr << "in source: " << source << ". ";
          boost::throw_exception(po::unknown_option(key));
        }
        string sub_section = var.substr(0, pos);
        var = var.substr(pos + 1);

        if (sub_section == "filter")
        {
          // bench section
          if (section == "bench")
          {
            p_dim_defined_map = &bench_filter_dim_defined_map;
            p_datatype_map = &bench_filter_map;
          }
          // errors section

          else if (section == "errors")
            p_errors_map = &errors_filter_map;
        }
        else if (sub_section == "smooth")
        {
          // bench section
          if (section == "bench")
          {
            p_dim_defined_map = &bench_smooth_dim_defined_map;
            p_datatype_map = &bench_smooth_map;
          }
          // errors section

          else if (section == "errors")
            p_errors_map = &errors_smooth_map;
        }
        else
        {
          cerr << "in source: " << source << ". ";
          boost::throw_exception(po::unknown_option(key));
        }
      }
      else
      {
        cerr << "in source: " << source << ". ";
        boost::throw_exception(po::unknown_option(key));
      }

      // data and bench sections
      if (section == "data" || section == "bench")
      {
        map<string, Flags> & dim_defined_map = *p_dim_defined_map;
        map<string, vector<MultiArray> > & datatype_map = *p_datatype_map;

        Size dim = 1;
        if (((pos = var.rfind('.')) != string::npos)
            && var.find_last_not_of("0123456789") == pos)
        {
          istringstream(var.substr(pos + 1)) >> dim;
          var = var.substr(0, pos);
        }

        if (dim < 1)
        {
          cerr << "in source: " << source << ". ";
          boost::throw_exception(po::error(String("index must be > 0 in '"
              + key + "'")));
        }
        Size len = 1;
        if (dim_map.count(var))
          len = dim_map[var]->Length();
        if (dim > len)
        {
          cerr << "in source: " << source << ". ";
          boost::throw_exception(po::error(String("data exceeds dimension in '"
              + key + "'")));
        }

        Bool allocate = datatype_map[var].empty();
        if (allocate)
          dim_defined_map[var] = Flags(len, false);

        if (dim_defined_map[var][dim - 1])
        {
          cerr << "in source: " << source << ". ";
          cerr << "Warning: option " << key << " already has value." << endl;
        }
        else
        {
          dim_defined_map[var][dim - 1] = true;
          String value_str;
          Scalar value;
          for (Size v = 0; v < parsed.options[op].value.size(); ++v)
          {
            istringstream iss(parsed.options[op].value[v]);
            Size t = 0;
            while (iss.good())
            {
              getline(iss, value_str, BIIPSTEST_INPUT_VALUES_SEPARATOR);
              istringstream(value_str) >> value;
              if (allocate)
              {
                if (dim_map.count(var))
                  datatype_map[var].push_back(MultiArray(dim_map[var],
                                                         ValArray::Ptr(new ValArray(dim_map[var]->Length()))));
                else
                  datatype_map[var].push_back(MultiArray(p_scalar_dim,
                                                         ValArray::Ptr(new ValArray(1))));
              }
              datatype_map[var][t].Values()[dim - 1] = value;
              ++t;
            }
          }
        }
        continue;
      }

      // errors section
      if (section == "errors")
      {
        map<string, map<Size, vector<Scalar> > > & errors_map = *p_errors_map;

        string mutation;
        Size n_particles;
        if (((pos = var.rfind('.')) != string::npos)
            && var.find_last_not_of("0123456789") == pos)
        {
          istringstream(var.substr(pos + 1)) >> n_particles;
          mutation = var.substr(0, pos);
        }
        else
        {
          cerr << "in source: " << source << ". ";
          boost::throw_exception(po::unknown_option(key));
        }

        if (errors_map.count(mutation)
            && errors_map.at(mutation).count(n_particles))
        {
          cerr << "in source: " << source << ". ";
          cerr << "Warning: option " << key << " already has value." << endl;
        }
        else
        {
          string value_str;
          Scalar value;
          for (Size v = 0; v < parsed.options[op].value.size(); ++v)
          {
            istringstream iss(parsed.options[op].value[v]);
            while (iss.good())
            {
              getline(iss, value_str, BIIPSTEST_INPUT_VALUES_SEPARATOR);
              istringstream(value_str) >> value;
              errors_map[mutation][n_particles].push_back(value);
            }
          }
        }
      }
    }
  }
}

#include "common/IndexRangeIterator.hpp"

map<String, MultiArray> transformStoredDataMap(const StoredDataMap & storedDataMap)
{
  using namespace Biips;

  // create a return data map
  map<String, MultiArray> data_map;

  // iterate on all variables in storedDataMap
  for (StoredDataMap::const_iterator it_stored = storedDataMap.begin(); it_stored
      != storedDataMap.end(); ++it_stored)
  {
    // name of the variable
    const String & name = it_stored->first;

    if (it_stored->second.empty())
    {
#ifdef BIIPS_DEBUG
      cout << "\t" << "Skipping data variable: " << name << endl;
#endif
      continue;
    }

    if (it_stored->second.size() == 1)
    {
      data_map.insert(std::make_pair(name, it_stored->second[0]));
      continue;
    }

    DimArray dim_first = it_stored->second[0].Dim();
    Size ndim_first = dim_first.NDim();

    // compute dimension of the bigger MultiArray holding all values
    Size ndim_big = ndim_first + 1;
    DimArray::Ptr p_dim_big(new DimArray(ndim_big));
    for (Size i = 0; i < ndim_first; ++i)
      (*p_dim_big)[i] = dim_first[i];
    p_dim_big->back() = it_stored->second.size();

    // create a bigger MultiArray holding all values
    MultiArray marray_big(p_dim_big,
                          ValArray::Ptr(new ValArray(p_dim_big->Length())));
    IndexRange range_big(marray_big.Dim());

    IndexRange::Indices lower = range_big.Lower();
    IndexRange::Indices upper = range_big.Upper();

    IndexRange range_small(dim_first);

    // copy all sub MultiArray of storedDataMap in the big one
    for (Size i = 0; i < p_dim_big->back(); ++i)
    {
      const MultiArray & marray_small = it_stored->second[i];

      if (marray_small.Dim() != dim_first)
        throw LogicError(String("ERROR: dimension of stored data is inconsistent in variable ")
            + name);

      // create a IndexRange where we are storing each sub MultiArray values
      lower.back() = i + 1;
      upper.back() = i + 1;
      IndexRange sub_range_big(lower, upper);
      IndexRangeIterator it_big(sub_range_big);

      // iterate on all values of the sub MultiArray
      for (IndexRangeIterator it_small(range_small); !it_small.AtEnd(); it_small.Next(), it_big.Next())
      {
        Size offset_small = range_small.GetOffset(it_small);
        Size offset_big = range_big.GetOffset(it_big);
        marray_big.Values()[offset_big] = marray_small.Values()[offset_small];
      }
    }

    // insert the filled bigger MultiArray in the return data map
    data_map.insert(std::make_pair(name, marray_big));
  }

  return data_map;
}
