function [obj_pmmh, samples_pmmh, varargout] = biips_pmmh_samples(obj_pmmh, n_iter, n_part, varargin)
% BIIPS_PMMH_SAMPLES Perform iterations for the PMMH algorithm and returns samples.
% [obj_pmmh, samples_pmmh, log_marg_like_pen, log_marg_like, info_pmmh] = ... 
%     biips_pmmh_samples(obj_pmmh, n_iter, n_part, 'PropertyName', PropertyValue, ...)
%
%   INPUT 
%   - obj_pmmh:     PMMH structure as returned by BIIPS_PMMH_INIT
%   - n_iter:       integer. Number of iterations
%   - n_part:       integer. Number of particles used in SMC algorithms
%   Optional Inputs:
%   - thin:         integer. Returns samples every 'thin' iterations
%                   (default = 1).
%   - rs_thres:    real. Threshold for the adaptive SMC resampling. (default = 0.5)
%                   * if 'rs_thres' is in [0,1], resampling occurs when
%                     (ESS < rs_thres * n_part)
%                   * if 'rs_thres' is in [2,nb_part], resampling occurs when
%                     (ESS < rs_thres)
%   - rs_type:     string. The type of algorithm used for the SMC resampling.
%                   Possible values are 'stratified', 'systematic',
%                   'residual', 'multinomial'. (default = 'stratified')
%
%   OUTPUT
%   - obj_pmmh:          structure. updated PMMH object
%   - samples_pmmh:      structure. PMMH samples for each monitored variable
%   Optional Output:
%   - log_marg_like_pen: vector of penalized log marginal likelihood estimates over iterations
%   - log_marg_like:     vector of log marginal likelihood estimates over iterations
%   - info_pmmh:          structure. Additional information on the MCMC run
%                         with the fields:
%                         * accept_rate: vector of acceptance rates over
%                         iterations
%                         * n_fail: number of failed SMC algorithms
%                         * rw_step: standard deviations of the random walk
%                         over iterations.
%
%   See also BIIPS_MODEL, BIIPS_PMMH_INIT, BIIPS_PMMH_UPDATE
%--------------------------------------------------------------------------
% EXAMPLE:
% n_burn = 2000; n_iter = 2000; thin = 1; n_part = 50; 
% param_names = {'log_prec_y';}
% latent_names = {'x'};
% obj_pmmh = biips_pmmh_init(model, param_names, 'latent_names', latent_names, 'inits', {-2});
% obj_pmmh = biips_pmmh_update(obj_pmmh, n_burn, n_part); 
% [obj_pmmh, samples_pmmh, log_marg_like_pen, log_marg_like, info_pmmh] = biips_pmmh_samples(obj_pmmh, n_iter, n_part,...
%     'thin', thin); 
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 21-10-2014
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

varargout = cell(nargout-2,1);
[obj_pmmh, samples_pmmh, varargout{:}] = pmmh_algo(obj_pmmh, n_iter, n_part,...
    return_samples, 'thin', thin, 'max_fail',...
    max_fail, 'rs_thres', rs_thres, 'rs_type', rs_type);
