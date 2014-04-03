function [obj, out, log_post, log_marg_like, stats_pmmh] = ... 
    biips_pmmh_samples(obj, n_iter, n_part, varargin)

%
% BIIPS_PMMH_SAMPLES performs iterations for the PMMH algorithm and returns samples
% [out, log_post, log_marg_like, stats_pmmh] = ... 
%     biips_pmmh_samples(obj, n_iter, n_part, 'PropertyName', PropertyValue)
%
%   INPUT 
%   - obj:          PMMH structure (returned by BIIPS_PMMH_OBJECT)
%   - n_iter:       positive integer. Number of iterations
%   - n_part:       positive integer. Number of particles used in SMC algorithms
%   Optional Inputs:
%   - thin:         positive integer. Returns samples every thin iterations
%                   (default=1)
%   - latent_names: cell of strings. Names of the variables, sampled with
%                   SMC, to be monitored
%   - rs_thres:     positive real (default = 0.5).
%                   Threshold for the resampling step (adaptive SMC).
%                   if rs_thres is in [0,1] --> resampling occurs when 
%                                           (ESS > rs_thres * nb_part)
%                   if rs_thres is in [2,nb_part] --> resampling occurs when 
%                                               (ESS > rs_thres)
%   - rs_type:      string (default = 'stratified')
%                   Possible values are 'stratified', 'systematic', 'residual', 'multinomial'
%                   Indicates the type of algorithm used for the resampling step.
%
%   OUTPUT
%   - obj:          PMMH structure
%   - out:          Structure with the PIMH samples for each variable
%   - log_marg_like_st: vector with log marginal likelihood over iterations
%
%   See also BIIPS_MODEL, BIIPS_PMMH_OBJECT, BIIPS_PMMH_UPDATE
%--------------------------------------------------------------------------
% EXAMPLE:
% n_burn = 2000; n_iter = 2000; thin = 1; n_part = 50; 
% param_names = {'log_prec_y[1:1]';}
% latent_names = {'x'};
% obj_pmmh = biips_pmmh_init(model, param_names, 'inits', {-2});
% obj_pmmh = biips_pmmh_update(obj_pmmh, n_burn, n_part); 
% [obj_pmmh, out_pmmh, log_post, log_marg_like, stats_pmmh] = biips_pmmh_samples(obj_pmmh, n_iter, n_part,...
%     'thin', 1); 
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, François Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

%% PROCESS AND CHECK INPUTS
optarg_names = {'thin', 'max_fail', 'rs_thres', 'rs_type'};
optarg_default = {1, 0, .5, 'stratified'};
optarg_valid = {[0, n_iter], [0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'}};
optarg_type = {'numeric', 'numeric', 'numeric', 'char'};
[thin, max_fail, rs_thres, rs_type] = parsevar(varargin, optarg_names,...
    optarg_type, optarg_valid, optarg_default);

return_samples = true;
[obj, out, log_post, log_marg_like, stats_pmmh] = biips_pmmh(obj, n_iter, n_part,...
    return_samples, 'thin', thin, 'max_fail',...
    max_fail, 'rs_thres', rs_thres, 'rs_type', rs_type);