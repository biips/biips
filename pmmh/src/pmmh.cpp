#include "pmmh.hpp"
#include "parse_varname.hpp"
#include "boost/typeof/typeof.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/range/numeric.hpp"
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
        params_total_size = 0;
        for(int i = 0; i < param_varnames.size() ;  ++i) {
              
            // a remplacer
            int VERBOSITY = 2;
            Bool mcmc = false;

            // calcul de la taille totale du vecteur de parametres
            params_total_size += _init_values[i].Length();

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
            _console.ChangeData(param_varnames[i], range, marray, mcmc, VERBOSITY);
        
            // log prior density 
            double log_p;
            string message1 = string("for variable ") + param_varnames[i] + string("cannot compute log prior");
            if (!_console.GetLogPriorDensity(log_p, param_varnames[i], range))
               throw LogicError(message1.c_str());
            string message2 = string("variable ") + param_varnames[i] + string(" has a NaN log_prior");
            if (isnan(log_p))
               throw LogicError(message2.c_str());
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
         
        if (isnan(log_marg_like))
             throw RuntimeError("Failed to get log marginal likelihood: NaN.");
        if (isinf(log_marg_like))
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
    } // fin Pmmh::pos_init

   void one_update(const vector<double> & sample) {

      //int fails = 0;
      //size_t & d = params_total_size;
      //vector<double> param_vec(d), prop(d);
      //double coef = 2.38 / sqrt(d) * exp( 
      //for (auto i : irange(0,d))
      //       prop[i] = sample[i] + 2.
      //
      // version avec composantes independantes
      

   }



} // fin namespace Biips




