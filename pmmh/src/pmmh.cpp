#include "pmmh.hpp"

namespace Biips {

    void Pmmh::post_init(void) {
  
        vector<vector<size_t> > param_lower, param_upper;
        vector<string> param_varnames;

        parse_varnames(_param_names, param_varnames, param_lower, param_upper); 
     
        if (_init_values.size() != param_varnames.size())
           throw LogicError("init_values and param_names must have the same size!");
        
        // loop on parameters
        // we copy the init_values into the smc
        double log_prior = 0.;
        for(int i = 0; i < param_varnames.size() ;  ++i) {
              
            // a remplacer
            int VERBOSITY = 2;
            Bool mcmc = false;

            IndexRange::Indices lind(param_lower[i].begin(), param_lower[i].end());
            IndexRange::Indices uind(param_upper[i].begin(), param_upper[i].end());
            auto range = IndexRange(lind, uind);
            
            Multiarray marray;
            auto ndims = _init_values[i].NDim():
            auto dims = _init_values[i].DimPtr();
            auto r_vec =  _init_values[i].ValuesPtr(); 
            
            auto p_dim = new DimArray(ndims);
            copy(dims, dims + ndims , p_dim->begin());

            auto p_val = new ValArray(r_vec_nb_elems);
            replace_copy(r_vec ,r_vec + r_vec_nb_elems, p_val->begin(), numeric_limits<Scalar>::quiet_NaN(), BIIPS_REALNA);
            marray.SetPtr(p_dim, p_val);
            _console_ptr->ChangeData(param_varnames[i], range, marray, mcmc, VERBOSITY);
        
            // log prior density 
            double log_p;
            string message1 = "for variable " + parse_varnames[i] + "cannot compute log prior";
            if (!_console_ptr->GetLogPriorDensity(log_p, param_varnames[i], range))
               throwLogicError(message1.c_str());
            string message2 = "variable " + parse_varnames[i] + " has a NaN log_prior";
            if (isnan(log_p))
               throw LogicalError(message2.c_str());
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
            auto latent_range = IndexRange(lind, uind);
            
            if (!_console_ptr->IsGenTreeSmoothMonitored(latent_varnames[i], latent_range, false)) {
                _console_ptr->SetGenTreeSmoothMonitor(latent_varnames[i], latent_range);
                latent_monitored = false; 
            }
        }

    

        if (!_console_ptr->SamplerBuilt()) {
           _console_ptr->BuildSampler(false, VERBOSITY);
        }

        bool sampler_at_end = _console_ptr->ForwardSamplerAtEnd();

        // run smc
        if (!sampler_at_end || !latent_monitored) {
            if (!sampler_at_end) {
                if (!_console_ptr->RunForwardSampler(_nb_particles, _init_rng_seed, 
                                                    _resample_type, _resample_threshold, VERBOSITY, false);
            }
        }
       
        double log_marg_like;
        if (!console_ptr->GetLogNormConst(log_marg_like))
             throw RuntimeError("Failed to get log normalizing constant.");
         
        if (isnan(log_marg_like))
             throw RuntimeError("Failed to get log marginal likelihood: NaN.");
        if (isinf(log_marg_like))
             throw RuntimeError("Failed to get log marginal likelihood: infinite value.");
   
        if (latent_varnames.size() > 0) {
            
            if(!p_console->DumpSampledGenTreeSmoothParticle(_sampled_value_map))
              throw RuntimeError("Failed to get sampled smooth particle.");
            
            if(_sampled_value_map.size() == 0) {
                  int rng_seed = 42;
                  _console_ptr->SampleGenTreeSmoothParticle(rng_seed);
                  if(!p_console->DumpSampledGenTreeSmoothParticle(_sampled_value_map))
                    throw RuntimeError("Failed to get sampled smooth particle.");
            }
        }
    } // fin Pmmh::pos_init
}
