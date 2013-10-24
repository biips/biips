#define BOOST_TEST_MODULE pmmh_test
#include <boost/test/included/unit_test.hpp>
#include "pmmh.hpp"
#include "include/common/Error.hpp"
#include <boost/assign/list_of.hpp>
#include "iostream/DumpReader.hpp"
#include <iostream>
#include <map>

using namespace std;
using namespace Biips;
using namespace boost::assign;

struct nireFixture {

    nireFixture(): 
        console(Console(cout, cerr))
        {
        map<String, MultiArray> data_map;

        string data_file_name = "data_jags.R";
        string model_file_name = "hmm_1d_nonlin_param.bug";

        Size verbosity = 2;
        ifstream ifs_dump(data_file_name.c_str());
        if (ifs_dump.fail())
          throw RuntimeError(String("Failed to open file ") + data_file_name);

        DumpReader dump_reader(ifs_dump);
        while (ifs_dump.good())
        {
          if (!dump_reader.next())
            throw RuntimeError(String("Error parsing file: ") + data_file_name);

          DimArray::Ptr dim_ptr(
              new DimArray(dump_reader.dims().begin(), dump_reader.dims().end()));
          ValArray::Ptr val_ptr(new ValArray());
          
          if (dump_reader.is_int())
          {
            if (dump_reader.has_na())
            {
              val_ptr->resize(dump_reader.int_values().size());
              replace_copy(dump_reader.int_values().begin(),
                           dump_reader.int_values().end(), val_ptr->begin(),
                           Scalar(BIIPS_INTNA), BIIPS_REALNA);
            }
            else
            {
              val_ptr->assign(dump_reader.int_values().begin(),
                              dump_reader.int_values().end());
            }
          }
          else
          {
            val_ptr->assign(dump_reader.double_values().begin(),
                            dump_reader.double_values().end());
          }
          if (val_ptr->empty())
            throw RuntimeError(
                String("Error reading data: variable ") + dump_reader.name()
                + " has no values.");
          if (dim_ptr->empty())
            dim_ptr->push_back(val_ptr->size());
          else if (dim_ptr->Length() != val_ptr->size())
            throw RuntimeError(
                String("Error reading data: dimensions of variable ")
                + dump_reader.name() + " do not match the number of values.");

          data_map.insert(make_pair(dump_reader.name(), MultiArray(dim_ptr, val_ptr)));
        }
        // Make a console
        // ------------------

        // Check model syntax
        // ------------------
        if (!console.CheckModel(model_file_name, verbosity > 0))
          throw RuntimeError("Model syntax is incorrect.");

        // Load Base module
        // ------------------
        if (!console.LoadBaseModule(verbosity > 0))
          throw RuntimeError("Failed to load Base module.");

        // Compile model
        // ------------------
        data_rng_seed = 42;
        if (!console.Compile(data_map, false, data_rng_seed, verbosity))
           throw RuntimeError("Failed to compile model.");
        
        
        param_names.push_back("log_prec_y");
        init_values.push_back(MultiArray(0.));
        nb_particles = 100;


   } 

   Console console;
   vector<string> param_names;
   vector<MultiArray> init_values; 
   size_t nb_particles;
   vector<string> latent_names; 
   int data_rng_seed; 

};

BOOST_FIXTURE_TEST_SUITE(s, nireFixture)

BOOST_AUTO_TEST_CASE(constructeur) {

   Pmmh pmm1(console,
             param_names,
             latent_names,
             init_values,
             nb_particles); 

   pmm1.update(100);
   pmm1.sample(100, 1);
}


BOOST_AUTO_TEST_SUITE_END()
