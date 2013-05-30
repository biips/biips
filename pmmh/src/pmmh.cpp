#include "pmmh.hpp"
#include "parse_varname.hpp"
#include "boost/typeof/typeof.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/range/numeric.hpp"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <cmath>
#include <functional>
using namespace boost;

namespace Biips {

    void Pmmh::post_init(void) {
  
        vector<vector<size_t> > param_lower, param_upper;
        vector<string> param_varnames;

        Size VERBOSITY = 2;

        parse_varnames(_param_names, param_varnames, param_lower, param_upper); 
     
        if (_init_values.size() != param_varnames.size())
           throw LogicError("init_values and param_names must have the same size!");
        
        // loop on parameters
        // we copy the init_values into the smc
        double log_prior = 0.;
        _params_total_size = 0;
        
        for(i = 0; i < param_varnames.size() ;  ++i) {
              
            // a remplacer
            int VERBOSITY = 2;
            Bool mcmc = false;
            string & var_name = param_varnames[i];
           
            // calcul de la taille totale du vecteur de parametres
            _params_total_size += _init_values[i].Length();

            IndexRange::Indices lind(param_lower[i].begin(), param_lower[i].end());
            IndexRange::Indices uind(param_upper[i].begin(), param_upper[i].end());
            BOOST_AUTO(range, IndexRange(lind, uind));
            
            MultiArray marray;
            Size ndims = _init_values[i].NDim();
            BOOST_AUTO(dims, _init_values[i].Dim());
            BOOST_AUTO(r_vec, _init_values[i].Values()); 
            
            DimArray::Ptr p_dim (new DimArray(ndims));
            copy(dims.begin(), dims.end(), p_dim->begin());

            Size r_vec_nb_elems = _init_values[i].Length();
            ValArray::Ptr p_val (new ValArray(r_vec_nb_elems));
            replace_copy(r_vec.begin() ,r_vec.end(), p_val->begin(), numeric_limits<Scalar>::quiet_NaN(), BIIPS_REALNA);
            marray.SetPtr(p_dim, p_val);
            
            _console.ChangeData(var_name, range, marray, mcmc, VERBOSITY);
            _sampled_value_map[var_name] = marray;
            // a verifier si pas trop long
            _proposal[var_name] = marray.Clone();
            _l_step[var_name] = marray.Clone();
            _lstep[var_name] = marray.Clone();
            _step[var_name].Values().assign(marray.Length(), log(0.1));


            // log prior density 
            double log_p;
           
            string message1 = string("for variable ") + var_name + string("cannot compute log prior");
            if (!_console.GetLogPriorDensity(log_p, var_name, range)) throw LogicError(message1.c_str());
            
            string message2 = string("variable ") + varname + string(" has a NaN log_prior");
            if (std::isnan(log_p)) throw LogicError(message2.c_str());
            
            log_prior += log_p;
        
            
        
        }
        // latent_names

        vector<vector<size_t> > latent_lower, latent_upper;
        vector<string> latent_varnames;

        parse_varnames(_latent_names, latent_varnames, latent_lower, latent_upper); 
       
        bool latent_monitored = true;
        for(int i = 0; i < latent_varnames.size(); ++i){
            
            IndexRange::Indices lind(latent_lower[i].begin(), latent_lower[i].end());
            IndexRange::Indices uind(latent_upper[i].begin(), latent_upper[i].end());
            BOOST_AUTO(latent_range, IndexRange(lind, uind));
            
            if (!_console.IsGenTreeSmoothMonitored(latent_varnames[i], latent_range, false)) {
                _console.SetGenTreeSmoothMonitor(latent_varnames[i], latent_range);
                latent_monitored = false; 
            }
        }

        if (!_console.SamplerBuilt()) {
           _console.BuildSampler(false, VERBOSITY);
        }

        bool sampler_at_end = _console.ForwardSamplerAtEnd();

        // run smc
        if (!sampler_at_end || !latent_monitored) {
            if (!sampler_at_end) {
                if (!_console.RunForwardSampler(_nb_particles, _init_rng_seed, 
                                                    _resample_type, _resample_threshold, VERBOSITY, false))
                   throw RuntimeError("Failed to run forward sampler");
            }
        }
       
        double log_marg_like;
        if (!_console.GetLogNormConst(log_marg_like))
             throw RuntimeError("Failed to get log normalizing constant.");
         
        if (std::isnan(log_marg_like))
             throw RuntimeError("Failed to get log marginal likelihood: NaN.");
        if (std::isinf(log_marg_like))
             throw RuntimeError("Failed to get log marginal likelihood: infinite value.");
   
        if (latent_varnames.size() > 0) {
            
            if(!_console.DumpSampledGenTreeSmoothParticle(_sampled_value_map))
              throw RuntimeError("Failed to get sampled smooth particle.");
            

            if(_sampled_value_map.size() == 0) {
                  int rng_seed = 42;
                  _console.SampleGenTreeSmoothParticle(rng_seed);
                  if(!_console.DumpSampledGenTreeSmoothParticle(_sampled_value_map))
                    throw RuntimeError("Failed to get sampled smooth particle.");
            }
        }
        int v = 15;
    } // fin Pmmh::pos_init

   void Pmmh::one_step_update() {

      int fails = 0;
      size_t & d = _params_total_size;
      double coef = 2.38 / sqrt(d);  
      
      random::mt19937 mt;
      normal_distribution<double> norm_dist;
      variate_generator<random::mt19937 & , normal_distribution<double> > norm_gen(mt, norm_dist);
      
      
      // version avec composantes independantes
      for (int i = 0; i < param_names.size(); ++i) {
             auto param = _sampled_value_map[param_names[i]].second;
             auto ptr = param.Values();
             for(int j = 0; j < param.Length() ; ++j) {
              prop[j] = ptr[i] + coef * exp(_lstep[i]) * norm_gen();
      }
      }           
         throw RuntimeError("PMMH proposal have NAN values");
         
      double log_prior_prop = 0;

      
      

   }



} // fin namespace Biips




