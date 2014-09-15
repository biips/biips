function [obj_pmmh, samples, varargout] = ... 
    biips_pmmh_samples(obj_pmmh, n_iter, n_part, varargin)

%
% BIIPS_PMMH_SAMPLES performs iterations for the PMMH algorithm and returns samples
% [obj_pmmh, samples, log_marg_like_pen, log_marg_like, info_pmmh] = ... 
%     biips_pmmh_samples(obj_pmmh, n_iter, n_part, 'PropertyName', PropertyValue)
%
%   INPUT 
%   - obj_pmmh:          PMMH structure (returned by BIIPS_PMMH_OBJECT)
%   - n_iter:       positive integer. Number of iterations
%   - n_part:       positive integer. Number of particles used in SMC algorithms
%   Optional Inputs:
%   - thin:         positive integer. Returns samples every thin iterations
%                   (default=1)
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
%   - obj_pmmh:          PMMH structure
%   - samples:           Structure with the PIMH samples for each variable
%   Optional Output:
%   - log_marg_like_pen: vector with penalized log marginal likelihood over iterations
%   - log_marg_like:     vector with log marginal likelihood over iterations
%   - info_pmmh:         Structure with additional information on the MCMC run
%
%   See also BIIPS_MODEL, BIIPS_PMMH_OBJECT, BIIPS_PMMH_UPDATE
%--------------------------------------------------------------------------
% EXAMPLE:
% n_burn = 2000; n_iter = 2000; thin = 1; n_part = 50; 
% param_names = {'log_prec_y[1:1]';}
% latent_names = {'x'};
% obj_pmmh = biips_pmmh_init(model, param_names, 'latent_names', latent_names, 'inits', {-2});
% obj_pmmh = biips_pmmh_update(obj_pmmh, n_burn, n_part); 
% [obj_pmmh, samples, log_marg_like_pen, log_marg_like, out_pmmh] = biips_pmmh_samples(obj_pmmh, n_iter, n_part,...
%     'thin', 1); 
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

%% PROCESS AND CHECK INPUTS
optarg_names = {'thin', 'max_fail', 'rs_thres', 'rs_type'};
optarg_default = {1, 0, .5, 'stratified'};
optarg_valid = {[0, n_iter], [0, n_part],...
    {'stratified', 'systematic', 'residual', 'multinomial'}};
optarg_type = {'numeric', 'numeric', 'numeric', 'char'};
[thin, max_fail, rs_thres, rs_type] = parsevar(varargin, optarg_names,...
    optarg_type, optarg_valid, optarg_default);

%% Call pmmh_algo internal routine
return_samples = true;

[obj_pmmh, samples, varargout{:}] = pmmh_algo(obj_pmmh, n_iter, n_part,...
    return_samples, 'thin', thin, 'max_fail',...
    max_fail, 'rs_thres', rs_thres, 'rs_type', rs_type);
