#ifndef BIIPS_PMMH_HPP_
#define BIIPS_PMMH_HPP_

#include "Console.hpp"
#include <string>
#include <vector>

using namespace std;

namespace Biips
{

    class Pmmh {
         public: 
          
             Pmmh(Console & console,
                  vector<string> & param_names,
                  vector<string> & latent_names,
                  vector<MultiArray> & init_values,
                  int nb_particles,
                  double resample_threshold = 0.5,
                  const string & resample_type = "stratified",
                  size_t init_rng_seed):
                  _console(console),
                  _param_names(param_names),
                  _latent_names(latent_names),
                  _init_values(init_values),
                  _nb_particles(nb_particles),
                  _resample_threshold(resample_threshold),
                  _resample_type(resample_type),
                  _init_rng_seed(init_rng_seed){ post_init();}
         
       
             void update(size_t n_iter);

             ~Pmmh(void) {}

          protected:

             Console & _console;
             vector<string> _param_names;
             vector<string> _latent_names;
             vector<MultiArray> _init_values;
             int _nb_particles;
             double _resample_threshold = 0.5;
             string _resample_type = "stratified";
             size_t _init_rng_seed;
             MultiArray data;
             map<String, MultiArray> _sampled_value_map;
             

             // prevent to copy a pmmh
             Pmmh(const Pmmh &);

             void post_init(void); 

             void one_step_update(void);
    };
}
#endif
