function [obj_pimh, log_marg_like] = biips_pimh_update(obj_pimh, n_iter, n_part, varargin)

%
% BIIPS_PIMH_UPDATE performs burn-in iterations for the PIMH algorithm
% obj_pimh = biips_pimh_update(obj_pimh, n_iter, n_part, varargin)
%
%   INPUT: 
%   - obj_pimh:     structure containing information about the PIMH
%   - n_iter:       positive integer. Number of burn-in iterations
%   - n_part:       positive integer. Number of particles used in SMC algorithms
%   Optional Inputs:
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
%   - obj_pimh:     structure. PIMH object modified
%   - log_marg_like: vector with log marginal likelihood over iterations
%
%   See also BIIPS_MODEL, BIIPS_PIMH_INIT, BIIPS_PIMH_SAMPLES
%--------------------------------------------------------------------------
% EXAMPLE:
% data = struct('var1', 0, 'var2', 1.2);
% model = biips_model('model.bug', data)
% variables = {'x'}; 
% nburn = 1000; niter = 1000; npart = 100; 
% obj_pimh = biips_pimh_init(model, variables); %Initialize
% obj_pimh = biips_pimh_update(obj_pimh, nburn, npart); % Burn-in
% [obj_pimh, samples_pimh] = biips_pimh_samples(obj_pimh, niter, npart); % Samples
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

if nargout>=2
    [obj_pimh, ~, log_marg_like] = pimh_algo(obj_pimh, n_iter, n_part, varargin{:});
else
    obj_pimh = pimh_algo(obj_pimh, n_iter, n_part, varargin{:});
end
    
