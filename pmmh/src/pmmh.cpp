#include "pmmh.hpp"
#include "parse_varname.hpp"

#include "boost/typeof/typeof.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/range/numeric.hpp"

#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/range/algorithm/find_if.hpp>


#include "common/ValArray.hpp"
#include <cmath>
#include <functional>
#include <map>
using namespace boost;
namespace Biips {

    void Pmmh::post_init(void) {
  

        _VERBOSITY = 2;

        parse_varnames(_param_names, _param_varnames, _param_lower, _param_upper); 
     
        if (_init_values.size() != _param_varnames.size())
           throw LogicError("init_values and param_names must have the same size!");
        
        // loop on parameters
        // we copy the init_values into the smc
        _log_prior = 0.;
        _params_total_size = 0;
        
        for(int i = 0; i < _param_varnames.size() ;  ++i) {
              
            // a remplacer
            int _VERBOSITY = 2;
            Bool mcmc = false;
            string & var_name = _param_varnames[i];
           
            // calcul de la taille totale du vecteur de parametres
            _params_total_size += _init_values[i].Length();

            IndexRange::Indices lind(_param_lower[i].begin(), _param_lower[i].end());
            IndexRange::Indices uind(_param_upper[i].begin(), _param_upper[i].end());
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
            
            _console.ChangeData(var_name, range, marray, mcmc, _VERBOSITY);
            _sampled_value_map[var_name] = marray;
            
            // a verifier si pas trop long
            _proposal[var_name] = marray.Clone();
            _l_step[var_name] = marray.Clone();
            _l_step[var_name] = marray.Clone();
            _l_step[var_name].Values().assign(marray.Length(), log(0.1));


            // log prior density 
            double log_p;
           
            
            if (!_console.GetLogPriorDensity(log_p, var_name, range)) 
                 throw LogicError((string("for variable ") + var_name + string("cannot compute log prior")).c_str());
            
            if (std::isnan(log_p)) 
                  throw LogicError((string("variable ") + var_name + string(" has a NaN log_prior")).c_str());
            
            _log_prior += log_p;
        
        }
        // latent_names


        parse_varnames(_latent_names, _latent_varnames, _latent_lower, _latent_upper); 
       
        bool latent_monitored = true;
        for(int i = 0; i < _latent_varnames.size(); ++i){
            
            IndexRange::Indices lind(_latent_lower[i].begin(), _latent_lower[i].end());
            IndexRange::Indices uind(_latent_upper[i].begin(), _latent_upper[i].end());
            BOOST_AUTO(latent_range, IndexRange(lind, uind));
            
            if (!_console.IsGenTreeSmoothMonitored(_latent_varnames[i], latent_range, false)) {
                _console.SetGenTreeSmoothMonitor(_latent_varnames[i], latent_range);
                latent_monitored = false; 
            }
        }

        if (!_console.SamplerBuilt()) {
           _console.BuildSampler(false, _VERBOSITY);
        }

        bool sampler_at_end = _console.ForwardSamplerAtEnd();

        // run smc
        if (!sampler_at_end || !latent_monitored) {
            if (!sampler_at_end) {
                if (!_console.RunForwardSampler(_nb_particles, _init_rng_seed, 
                                                    _resample_type, _resample_threshold, _VERBOSITY, false))
                   throw RuntimeError("Failed to run forward sampler");
            }
        }
       
        _log_marg_like = 0;
        if (!_console.GetLogNormConst(_log_marg_like))
             throw RuntimeError("Failed to get log normalizing constant.");
         
        if (std::isnan(_log_marg_like))
             throw RuntimeError("Failed to get log marginal likelihood: NaN.");
        if (std::isinf(_log_marg_like))
             throw RuntimeError("Failed to get log marginal likelihood: infinite value.");
   
        if (_latent_varnames.size() > 0) {
            
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
   
   bool Pmmh::one_step_update() {

      size_t & d = _params_total_size;
      double coef = 2.38 / sqrt(d);  
      
      random::mt19937 mt;
      normal_distribution<double> norm_dist;
      variate_generator<random::mt19937 & , normal_distribution<double> > norm_gen(mt, norm_dist);
      
      // version avec composantes independantes
      // on ne touche qu'aux parametres
      for (auto & s : _param_varnames) {
             
             ValArray & sample = _sampled_value_map[s].Values();
             ValArray & step = _l_step[s].Values();
             ValArray & prop = _proposal[s].Values();
             
             for(int j = 0; j < _proposal[s].Length() ; ++j) {
                 prop[j] = sample[j] + coef * exp(step[j]) * norm_gen();
                 if (std::isnan(prop[j]))
                    throw LogicError("proposal PMMH as NaN value:");
             }
             
           
      }           
         
      double log_prior_prop = 0;
      
      for (int i  = 0; i <  _param_varnames.size() ; ++i) {
             
              IndexRange::Indices lind(_param_lower[i].begin(), _param_lower[i].end());
              IndexRange::Indices uind(_param_upper[i].begin(), _param_upper[i].end());
              IndexRange range_i = IndexRange(lind, uind);
             
             
             // on injecte la proposal en  entree du smc
             if (!_console.ChangeData(_param_varnames[i], range_i, _proposal[_param_varnames[i]], true, 0))
                throw RuntimeError("Failed to change data in proposal");
     
             double log_p;
             _console.GetLogPriorDensity(log_p, _param_varnames[i], range_i);

             if (std::isnan(log_p)) 
                throw RuntimeError((string("Failed to get log prior density node ")+ _param_varnames[i] + string("is not stochastic.")).c_str());

             log_prior_prop += log_p;
   }


   // compute log marginal likelihood: run smc sampler
  
   if (!_console.RunForwardSampler(_nb_particles, _init_rng_seed, _resample_type, _resample_threshold, 0, false))
      throw RuntimeError("Unable to run smc on proposal");
   
   double log_marg_like_prop  = 0;
   if (!_console.GetLogNormConst(log_marg_like_prop))
      throw RuntimeError("Unable to retrieve log marginal likelyhood on proposal");
   
   if (std::isnan(log_marg_like_prop) || (std::isinf(log_marg_like_prop)))
      throw RuntimeError("log marginal likelyhood is infinite or NaN for a proposal");

   double acceptance_rate = exp(log_marg_like_prop - _log_marg_like + log_prior_prop - _log_prior);

   variate_generator<random::mt19937 & , uniform_real<> > unif_gen(mt, uniform_real<>());

   bool accept = unif_gen() < acceptance_rate;
   if (accept) {

       for (auto & m : _sampled_value_map) {
               // sample <- prop
               m.second.Values() =  _proposal[m.first].Values();
       }
       
       _log_prior = log_prior_prop;
       _log_marg_like = log_marg_like_prop;
   
       if ( _latent_names.size() > 0) {
           // FIXME
           _console.SampleGenTreeSmoothParticle(42);
           std::map<string, MultiArray> sampled_val;
           _console.DumpSampledGenTreeSmoothParticle(sampled_val);
           for(int i = 0 ; i < _latent_names.size(); ++i) {
               _sampled_value_map[_latent_names[i]].Values() = sampled_val[_latent_names[i]].Values();
           }
       }  
   
   } // fin proposal accepte

   // rescale step
   double p_modif = std::min(acceptance_rate, 1.); 
   _pmean += 2 * (p_modif - _pmean) / _n_iter;
   _n_iter++;

   if (_adapt) {
      double modif = (p_modif - _target_prob)/ _cross_target;
      for(auto & s : _param_varnames) {
          ValArray & step = _l_step[s].Values();
          step += modif;
      }
      if ( (p_modif > _target_prob) != _pover_target) {
           _pover_target = ! _pover_target; // a expliquer ca fait un peu magie noire
           _cross_target++;
      }
   }

   // penser a rajouter le code concernant la covariance avec le proposal
   // 
   return accept;
} // fin one_update

   void Pmmh::update(size_t nb_iter) {

      // think to count acceptations
      size_t accept_count = 0;
      for(int i = 0; i < nb_iter; ++i) {
         if (one_step_update()) accept_count++;
      }     

      // no more need to adapt, just burn!
      if (abs(log(_pmean / (1. - _pmean)) - log( _target_prob/ (1. - _target_prob))) < 0.5)
          _adapt = false;


   }



} // fin namespace Biips

