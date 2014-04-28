function [obj_pimh, samples, log_marg_like] = biips_pimh_samples(obj_pimh, n_iter, n_part, varargin)

%
% BIIPS_PIMH_SAMPLES performs iterations for the PIMH algorithm and returns samples
% [obj_pimh, samples, log_marg_like] = biips_pimh_samples(obj_pimh,...
%                           variable_names, n_iter, n_part, varargin)
%
%   INPUT
%   - obj_pimh:     structure. PIMH object
%   - variable_names: cell of strings. Contains the names of the
%                   unobserved variables to monitor.
%                   Examples: {'var1', 'var2[1]', 'var3[1:10]',
%                                                       'var4[1, 5:10, 3]'}
%                   Dimensions and indices must be a valid subset of
%                   the variables of the model.
%   - n_iter:       positive integer. Number of iterations
%   - n_part:       positive integer. Number of particles used in SMC algorithms
%   Optional Inputs:
%   - thin :        positive integer. Returns samples every thin iterations
%                   (default=1)
%   - rs_thres :    positive real (default = 0.5).
%                   Threshold for the resampling step (adaptive SMC).
%                   if rs_thres is in [0,1] --> resampling occurs when
%                                           (ESS > rs_thres * nb_part)
%                   if rs_thres is in [2,nb_part] --> resampling occurs when
%                                               (ESS > rs_thres)
%   - rs_type :     string (default = 'stratified')
%                   Possible values are 'stratified', 'systematic', 'residual', 'multinomial'
%                   Indicates the type of algorithm used for the resampling step.
%
%   OUTPUT
%   - obj_pimh:     structure. PIMH object modified
%   - samples:       Structure with the PIMH samples for each variable
%   - log_marg_like: vector with log marginal likelihood over iterations
%
%   See also BIIPS_MODEL, BIIPS_PIMH_UPDATE
%--------------------------------------------------------------------------
% EXAMPLE:
% data = struct('var1', 0, 'var2', 1.2);
% model = biips_model('model.bug', data)
% variables = {'x'};
% n_burn = 1000; n_iter = 1000; n_part = 100;
% obj_pimh = biips_pimh_init(model, variables); %Initialize
% obj_pimh = biips_pimh_update(obj_pimh, n_burn, n_part); % Burn-in
% [obj_pimh, samples_pimh] = biips_pimh_samples(obj_pimh, n_iter, n_part); % Samples
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

check_pimh(obj_pimh);

[obj_pimh, samples, log_marg_like] = pimh_algo(obj_pimh, n_iter, n_part, varargin{:});