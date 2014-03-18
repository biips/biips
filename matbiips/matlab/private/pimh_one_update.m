function [sample, log_marg_like, accepted] = pimh_one_update(console,...
    variable_names, n_part, rs_thres, rs_type, sample, log_marg_like)

%--------------------------------------------------------------------------
% PIMH_ONE_UPDATE performs one iteration of the Particle independent
% Metropolis-Hastings algorithm
% [sample, log_marg_like, accepted] = pimh_one_update(console,...
%    variable_names, n_part, rs_thres, rs_type, sample, log_marg_like)

%   INPUT 
%   - console:  integer. Id of the console containing the model, 
%               returned by the 'biips_model' function
%   - variable_names: cell of strings. Contains the names of the 
%               unobserved variables to monitor.
%               Possible value: {'var1', 'var2[1]', 'var3[1:10]',
%                                                    'var4[1, 5:10, 3]'}
%               Dimensions and indices must be a valid subset of 
%               the variables of the model.
%   - n_part:   positive integer. Number of particles used in SMC algorithms
%
%   - rs_thres: positive real 
%               Threshold for the resampling step (adaptive SMC).
%               if rs_thres is in [0,1] --> resampling occurs when 
%                                           (ESS > rs_thres * nb_part)
%               if rs_thres is in [2,nb_part] --> resampling occurs when 
%                                               (ESS > rs_thres)
%   - rs_type:  string 
%               Possible values are 'stratified', 'systematic', 'residual', 'multinomial'
%               Indicates the type of algorithm used for the resampling step.           
%   - sample:   cell of the same length as variable_names containing the
%               values of the variables at the current iteration
%   - log_marg_like: log marginal likelihood at the current iteration
%   OUTPUT:
%   - sample:     cell containing the new sample
%   - log_marg_like: log marginal likelihood
%   - accepted:   boolean if proposal accepted in the MH algorithm
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, François Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------



%% SMC
run_smc_forward(console, n_part, rs_thres, rs_type);

%% Acceptance rate
log_marg_like_prop = inter_biips('get_log_norm_const', console);
log_ar = log_marg_like_prop - log_marg_like;

%% Metropolis-Hastings step
accepted = (rand<exp(log_ar));
if accepted
    log_marg_like = log_marg_like_prop;
    
    % Sample one particle
    rng_seed = get_seed();
    inter_biips('sample_gen_tree_smooth_particle', console, rng_seed);
    
    % Get sampled value
    sampled_value = inter_biips('get_sampled_gen_tree_smooth_particle', console);
    cell_struct = cell(length(variable_names), 1);
    for i=1:length(variable_names)
        cell_struct{i} = getfield(sampled_value, variable_names{i});
    end
    sample = cell2struct_weaknames(cell_struct, variable_names);
end
    
