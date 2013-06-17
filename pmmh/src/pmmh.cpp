#include "pmmh.hpp"
#include "parse_varname.hpp"

#include "boost/typeof/typeof.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/range/numeric.hpp"

#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/range/algorithm/find_if.hpp>


#include "iostream/ProgressBar.hpp"
#include "common/ValArray.hpp"
#include "common/IndexRange.hpp"
#include <cmath>
#include <functional>
#include <map>
using namespace boost;
using namespace Biips;

namespace Biips {

    void Pmmh::post_init(void) {
  
        _VERBOSITY = 2;

        parse_varnames(_param_names, _param_varnames, _param_lower, _param_upper); 
     
        CheckRuntime(_init_values.size() != _param_varnames.size(), "init_values and param_names must have the same size!");
        
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

            IndexRange range = IndexRange(_param_lower[i], _param_upper[i]);
            
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
            
            _console.ChangeData(var_name, range , marray, mcmc, _VERBOSITY);
            _sampled_value_map[var_name] = marray;
            
            // a verifier si pas trop long
            _proposal[var_name] = marray.Clone();
            _l_step[var_name] = marray.Clone();
            _l_step[var_name] = marray.Clone();
            _l_step[var_name].Values().assign(marray.Length(), log(0.1));


            // log prior density 
            double log_p;
            
            CheckRuntime(!_console.GetLogPriorDensity(log_p, var_name, range),
                 (string("for variable ") + var_name + string("cannot compute log prior")).c_str());
            
            CheckRuntime(std::isnan(log_p), (string("variable ") + var_name + string(" has a NaN log_prior")).c_str());
            
            _log_prior += log_p;
        
        }
        // latent_names


        parse_varnames(_latent_names, _latent_varnames, _latent_lower, _latent_upper); 
       
        bool latent_monitored = true;
        for(int i = 0; i < _latent_varnames.size(); ++i){
            
            IndexRange latent_range = IndexRange(_latent_lower[i], _latent_upper[i]);
            
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
                CheckRuntime(!_console.RunForwardSampler(_nb_particles, _init_rng_seed, 
                                                         _resample_type, _resample_threshold, _VERBOSITY, false), "Failed to run forward sampler");
            }
        }
       
        _log_marg_like = 0;
        
        CheckRuntime(!_console.GetLogNormConst(_log_marg_like), "Failed to get log normalizing constant.");
         
        CheckRuntime(std::isnan(_log_marg_like), "Failed to get log marginal likelihood: NaN.");
        
        CheckRuntime(std::isinf(_log_marg_like), "Failed to get log marginal likelihood: infinite value.");
   
        if (_latent_varnames.size() > 0) {
            
            CheckRuntime(!_console.DumpSampledGenTreeSmoothParticle(_sampled_value_map), 
               "Failed to get sampled smooth particle.");
            

            if(_sampled_value_map.size() == 0) {
                  int rng_seed = 42;
                  _console.SampleGenTreeSmoothParticle(rng_seed);
                  CheckRuntime(!_console.DumpSampledGenTreeSmoothParticle(_sampled_value_map), 
                               "Failed to get sampled smooth particle.");
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
                 CheckRuntime(std::isnan(prop[j]), "proposal PMMH as NaN value:");
             }
             
           
      }           
         
      double log_prior_prop = 0;
      
      for (int i  = 0; i <  _param_varnames.size() ; ++i) {
             
              auto range_i = IndexRange(_param_lower[i], _param_upper[i]);
             
             // on injecte la proposal en  entree du smc
             CheckRuntime(!_console.ChangeData(_param_varnames[i], range_i, _proposal[_param_varnames[i]], true, 0),
                "Failed to change data in proposal");
     
             double log_p;
             _console.GetLogPriorDensity(log_p, _param_varnames[i], range_i);

             CheckRuntime(std::isnan(log_p), 
                (string("Failed to get log prior density node ")+ _param_varnames[i] + string("is not stochastic.")).c_str());

             log_prior_prop += log_p;
       } // fin loop param


       // compute log marginal likelihood: run smc sampler
 
       CheckRuntime(!_console.RunForwardSampler(_nb_particles, _init_rng_seed, _resample_type, _resample_threshold, 0, false),
          "Unable to run smc on proposal");
       
       double log_marg_like_prop  = 0;
       CheckRuntime(!_console.GetLogNormConst(log_marg_like_prop),
          "Unable to retrieve log marginal likelihood on proposal");
       
       CheckRuntime(std::isnan(log_marg_like_prop) || std::isinf(log_marg_like_prop),
         "log marginal likelihood is infinite or NaN for a proposal");

       _acceptance_rate = exp(log_marg_like_prop - _log_marg_like + log_prior_prop - _log_prior);

       variate_generator<random::mt19937 & , uniform_real<> > unif_gen(mt, uniform_real<>());

       bool accept = unif_gen() < _acceptance_rate;
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
       double p_modif = std::min(_acceptance_rate, 1.); 
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

       // penser a rajouter le code concernant la covariance avec la proposal
       // 
      return accept;
   } // fin one_step _update

   int Pmmh::update(size_t nb_iter) {

      if (nb_iter == 0) return 0;
      
      int accept_count = 0;
      ProgressBar barre(nb_iter);
      bool drapeau = false;
      for(int i = 0; i < nb_iter; ++i) {
         drapeau = one_step_update();
         if (drapeau) 
            accept_count++;
         ++barre; 
         cout << flush; 
      }     

      // no more need to adapt, just burn!
      if (abs(log(_pmean / (1. - _pmean)) - log(_target_prob / (1. - _target_prob))) < 0.5)
          _adapt = false;

      // before to return we need to check if last update was accepted
      // if not, we need store manually the loglikelihood in the biips model
      // using set_log_norm const because the SMC updated to a false value
      if (!drapeau) {
          for (int i  = 0; i <  _param_varnames.size() ; ++i) {
             auto range_i =  IndexRange(_param_lower[i], _param_upper[i]);
             CheckRuntime(!_console.ChangeData(_param_varnames[i], range_i, 
                                               _sampled_value_map[_param_varnames[i]], true, 0), 
                           "Failed to change data in sample");

          }

      }

      return accept_count;
   }

   int Pmmh::sample(size_t nb_iter, size_t thin) {

      if (nb_iter == 0) 
           return 0;

      if (_adapt) {
         std::cout << "WARNING : adaptation not finish before to start sampling" << endl
                   << "so we force to stop adaptation" << endl;
         _adapt = false;
      }
      
      int n_samples = 0;
      int accept_count = 0;
      ProgressBar barre(nb_iter);
      bool drapeau = false;
      vector<double> accept_rate(nb_iter);
      for(int i = 0; i < nb_iter; ++i) {
         drapeau = one_step_update();
         if (drapeau) 
            accept_count++;
         accept_rate[i] = _acceptance_rate;   
         barre += 1; 
         if ((i - 1) % thin == 0) {
              n_samples++;
         }
      }     
    
      _console.ClearGenTreeSmoothMonitors(false);

      if (!drapeau) {
          for (int i  = 0; i <  _param_varnames.size() ; ++i) {
              auto range_i =  IndexRange(_param_lower[i], _param_upper[i]);
              CheckRuntime(!_console.ChangeData(_param_varnames[i], range_i, 
                                                _sampled_value_map[_param_varnames[i]], true, 0), 
                            "Failed to change data in sample");

          }
      
          _console.SetLogNormConst(_log_marg_like);
      
          if (_latent_names.size() > 0) {
             std::map<string, MultiArray> a_map;
             for(auto & s :  _latent_names) {
                     a_map[s] = _sampled_value_map[s];
             }
             _console.SetSampledGenTreeSmoothParticle(a_map);
          }
      
      
      }
      return accept_count; 
   }
} // fin namespace Biips

