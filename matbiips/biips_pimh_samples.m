function [obj_pimh, samples_pimh, varargout] = biips_pimh_samples(obj_pimh, n_iter, n_part, varargin)
% BIIPS_PIMH_SAMPLES Perform iterations for the PIMH algorithm and returns samples.
% [obj_pimh, samples_pimh, log_marg_like] = biips_pimh_samples(obj_pimh,...
%                           n_iter, n_part, 'PropertyName', PropertyValue, ...)
%
%   INPUT:
%   - obj_pimh:     structure. PIMH object as returned by BIIPS_PIMH_INIT
%   - n_iter:       integer. Number of iterations
%   - n_part:       integer. Number of particles used in SMC algorithms
%   Optional Inputs:
%   - thin :        integer. Returns samples every 'thin' iterations
%                   (default = 1)
%   - rs_thres :    real. Threshold for the adaptive SMC resampling.
%                   (default = 0.5)
%                   * if 'rs_thres' is in [0,1], resampling occurs when
%                     (ESS < rs_thres * n_part)
%                   * if 'rs_thres' is in [2,nb_part], resampling occurs when
%                     (ESS < rs_thres)
%   - rs_type :     string. The type of algorithm used for the SMC resampling.
%                   Possible values are 'stratified', 'systematic',
%                   'residual', 'multinomial'. (default = 'stratified')
%
%   OUTPUT:
%   - obj_pimh:     structure. updated PIMH object
%   - samples_pimh: structure. PIMH samples for each monitored variable
%   Optional Output:
%   - log_marg_like: vector of log marginal likelihood over iterations
%
%   See also BIIPS_MODEL, BIIPS_PIMH_INIT, BIIPS_PIMH_UPDATE
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

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 21-10-2014
%--------------------------------------------------------------------------

return_samples = true;
varargout = cell(nargout-2,1);
[obj_pimh, samples_pimh, varargout{:}] = pimh_algo(obj_pimh, n_iter, n_part, return_samples, varargin{:});
