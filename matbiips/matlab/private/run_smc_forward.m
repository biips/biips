function ok=run_smc_forward(p, n_part, rs_thres, rs_type, seed)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% RUN_SMC_FORWARD 
% ok = run_smc_forward(p, nb_part, rs_thres, rs_type, seed)
% INPUT 
% - p :         id (integer) of the console
% - n_part :    number (integer) of particules
% - rs_thres :  resampling threshold ( if real in  [0,1] ---> percentage of nb_particules
%                                        integer in [1,nb_part] --> number of particules
% - rs_type :   string belonging to {'stratified', 'systematic', 'residual', 'multinomial' } indicating 
%               the kind of algorithm used for esampling
% - seed :      seed for random number generator
% OUTPUT
% ok : boolean 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% BiiPS Project - Bayesian Inference with interacting Particle Systems
%
% Reference: A. Todeschini, M. Fuentes, F. Caron, P. Legrand, P. Del Moral.
% BiiPS: a software for Bayesian inference with interacting particle
% systems. Technical Report, INRIA. February 2014.
%
% Authors: Adrien Todeschini, Marc Fuentes
% INRIA Bordeaux, France
% email: biips-project@lists.gforge.inria.fr
% Website: https://alea.bordeaux.inria.fr/biips
% Jan 2014; Last revision: 30-01-2014
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

if nargin<5 % if seed not specified
    seed = get_seed();
end

if (~inter_biips('is_sampler_built', p))
   inter_biips('build_smc_sampler', p, false);
end
ok = inter_biips('run_smc_sampler', p, n_part, seed, rs_thres, rs_type);
