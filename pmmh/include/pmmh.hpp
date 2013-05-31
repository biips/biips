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
                  size_t init_rng_seed = 42):
                  _console(console),
                  _param_names(param_names),
                  _latent_names(latent_names),
                  _init_values(init_values),
                  _nb_particles(nb_particles),
                  _resample_threshold(resample_threshold),
                  _resample_type(resample_type),
                  _init_rng_seed(init_rng_seed),
                  _pmean(0),
                  _n_iter(0),
                  _VERBOSITY(2)
                  { post_init(); }
         
       
             void update(size_t n_iter);

             ~Pmmh(void) {}

             // this function realizes one step of the pmmh algorithm
             // ie, it apply the selection of one proposal, after
             // evaluating his likelyhood trought the smc.
             
             void one_step_update(void);

           protected:

             Console & _console;
             vector<string> _param_names;
             vector<string> _latent_names;
             vector<MultiArray> _init_values;
             int _nb_particles;
             double _resample_threshold;
             string _resample_type;
             size_t _init_rng_seed;
             double _pmean;
             size_t _n_iter;
             Size _VERBOSITY;
             
             double _log_marg_like;
             double _log_prior;
             
             
             vector<string> _param_varnames,
                            _latent_varnames;
             
             vector<vector<size_t> > _param_lower, 
                                     _param_upper,
                                     _latent_lower, 
                                     _latent_upper;
             
             
             map<String, MultiArray> _sampled_value_map,
                                     _l_step,
                                     _proposal;
             
             size_t _params_total_size; 
            
            // prevent to copy a pmmh
             Pmmh(const Pmmh &);

             void post_init(void); 

    };
}
#endif
