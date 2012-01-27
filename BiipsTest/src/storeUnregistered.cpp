//                                               -*- C++ -*-
/*! \file storeUnregistered.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "storeUnregistered.hpp"
#include <iostream>

namespace Biips
{
  const char BIIPSTEST_INPUT_VALUES_SEPARATOR = ' ';
  const char BIIPSTEST_INPUT_COLUMN_SEPARATOR = ';';
}

void storeUnregistered(const vector<po::parsed_options> & parsed_sources,
                       const vector<string> & sources_names,
                       StoredModelMap & model_map,
                       StoredDimMap & dim_map,
                       StoredDataMap & data_map,
                       Scalar & log_norm_const_bench,
                       StoredDataMap & bench_filter_map,
                       StoredDataMap & bench_smooth_map,
                       StoredErrorsMap & errors_filter_map,
                       StoredErrorsMap & errors_smooth_map)
{
  using namespace std;
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
      string::size_type pos;
      if ((pos = key.find('.')) == string::npos)
      {
        cerr << "in source: " << source << ". ";
        boost::throw_exception(po::unknown_option(key));
      }
      if (parsed.options[op].value.empty())
      {
        cerr << "in source: " << source << ". ";
        boost::throw_exception(po::error(String("no value given in '") + key
            + "'"));
      }

      string section = key.substr(0, pos);
      string var = key.substr(pos + 1);

      // model section
      if (section == "model")
      {
        if (model_map.count(var))
        {
          cerr << "in source: " << source << ". ";
          cerr << "Warning: option " << key << " already has value." << endl;
        }
        else
        {
          string column_str;
          string value_str;
          Scalar value;
          for (Size v = 0; v < parsed.options[op].value.size(); ++v)
          {
            istringstream iss(parsed.options[op].value[v]);
            ValArray::Ptr p_val_array(new ValArray());
            Size n_columns = 0;
            Size n_rows = 0;
            while (iss.good())
            {
              getline(iss, column_str, BIIPSTEST_INPUT_COLUMN_SEPARATOR);
              istringstream iss_column(column_str);
              Size n_rows_current = 0;
              while (iss_column.good())
              {
                getline(iss_column, value_str, BIIPSTEST_INPUT_VALUES_SEPARATOR);
                istringstream(value_str) >> value;
                p_val_array->push_back(value);
                n_rows_current++;
              }
              if (n_columns == 0)
                n_rows = n_rows_current;
              else if (n_rows_current != n_rows)
              {
                cerr << "in source: " << source << ". ";
                boost::throw_exception(po::error(String("columns have different lengths in '")
                    + key + "'"));
              }
              n_columns++;
            }
            DimArray::Ptr p_dim_array;
            if (n_columns == 0)
              p_dim_array = DimArray::Ptr(new DimArray(1, n_rows));
            else
            {
              p_dim_array = DimArray::Ptr(new DimArray(2));
              (*p_dim_array)[0] = n_rows;
              (*p_dim_array)[1] = n_columns;
            }
            model_map[var] = MultiArray(p_dim_array, p_val_array);
          }
        }
      }

      // dim section
      else if (section == "dim")
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
      }

      else
      {
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

        map<string, Flags> & dim_defined_map = *p_dim_defined_map;
        map<string, vector<MultiArray> > & datatype_map = *p_datatype_map;
        map<string, map<Size, vector<Scalar> > > & errors_map = *p_errors_map;

        // data and bench sections
        if (section == "data" || section == "bench")
        {
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
            boost::throw_exception(po::error(String("index must be > 0 in '")
                + key + "'"));
          }
          Size len = 1;
          if (dim_map.count(var))
            len = dim_map[var]->Length();
          if (dim > len)
          {
            cerr << "in source: " << source << ". ";
            boost::throw_exception(po::error(String("data exceeds dimension in '")
                + key + "'"));
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
                  {
                    datatype_map[var].push_back(MultiArray(dim_map[var],
                                                           ValArray::Ptr(new ValArray(dim_map[var]->Length()))));
                  }
                  else
                  {
                    datatype_map[var].push_back(MultiArray(p_scalar_dim,
                                                           ValArray::Ptr(new ValArray(1))));
                  }
                }
                datatype_map[var][t].Values()[dim - 1] = value;
                ++t;
              }
            }
          }
        }

        // errors section
        else if (section == "errors")
        {
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
}

