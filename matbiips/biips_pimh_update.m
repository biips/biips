function [obj_pimh, varargout] = biips_pimh_update(obj_pimh, n_iter, n_part, varargin)
% BIIPS_PIMH_UPDATE performs burn-in iterations for the PIMH algorithm.
% [obj_pimh, log_marg_like, accept_rate] = biips_pimh_update(obj_pimh, n_iter, n_part, 'PropertyName', PropertyValue, ...)
%
%   INPUT: 
%   - obj_pimh:     structure. PIMH object as returned by BIIPS_PIMH_INIT.
%   - n_iter:       integer. Number of burn-in iterations.
%   - n_part:       integer. Number of particles used in SMC algorithms.
%   Optional Inputs:
%   - thin :        integer. Thinning interval. Returns samples every 'thin' iterations
%                   (default = 1
%   - rs_thres, rs_type, ... : Additional arguments to be passed to the SMC
%      algorithm. See BIIPS_SMC_SAMPLES for for details.
% 
%   OUTPUT
%   - obj_pimh:     structure. updated PIMH object
%   Optional Output:
%   - log_marg_like: vector of log marginal likelihood estimates over iterations
%   - accept_rate: vector of acceptance rates over iterations
%
%   See also BIIPS_MODEL, BIIPS_PIMH_INIT, BIIPS_PIMH_SAMPLES

%--------------------------------------------------------------------------
% EXAMPLE:
% modelfile = 'hmm.bug';
% type(modelfile);
% 
% data = struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1), 'logtau', log(1));
% model = biips_model(modelfile, data);
% 
% n_part = 50;
% obj_pimh = biips_pimh_init(model, {'x', 'c[2:10]'}); % Initialize
% [obj_pimh, lml_pimh_burn] = biips_pimh_update(obj_pimh, 100, n_part); % Burn-in
% [obj_pimh, out_pimh, lml_pimh] = biips_pimh_samples(obj_pimh, 100, n_part); % Samples
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 21-10-2014
%--------------------------------------------------------------------------

return_samples = false;

varargout = cell(nargout-1,1);
[obj_pimh, varargout{:}] = pimh_algo(obj_pimh, n_iter, n_part, return_samples, varargin{:});
    
