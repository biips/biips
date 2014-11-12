function ok = smc_forward_algo(id, n_part, rs_thres, rs_type, seed)

%--------------------------------------------------------------------------
% SMC_FORWARD_algo runs a SMC algorithm
% ok = smc_forward_algo(id, nb_part, rs_thres, rs_type, seed)
%   INPUT 
%   - id:        id (integer) of the console
%   - n_part:    number (integer) of particules
%   - rs_thres:  resampling threshold ( if real in  [0,1] ---> percentage of n_part
%                                       integer in ]1,nb_part] --> number of particules
%   - rs_type:   string belonging to {'stratified', 'systematic', 'residual', 'multinomial'} indicating 
%                the algorithm used for resampling
%   - seed:      seed for the random number generator
%
%   OUTPUT
%   - ok: logical 
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

if nargin<5 % if seed not specified
    seed = get_seed();
end

if (~matbiips('is_sampler_built', id))
   matbiips('build_smc_sampler', id, false);
end
ok = matbiips('run_smc_sampler', id, n_part, seed, rs_thres, rs_type);
