function [obj_pimh] = biips_pimh_init(model, variable_names)
% BIIPS_PIMH_INIT Create a PIMH object.
%   Initializes the Particle Independent Metropolis-Hastings algorithm
%   obj_pimh = biips_pimh_init(model, variable_names)
% 
%   The PIMH algorithm provides MCMC samples of the variables in 'variable_names', 
%   using a SMC algorithm as proposal distribution in an independent 
%   Metropolis-Hastings (MH) algorithm.
% 
%   INPUT
%   - model:    Biips model as returned by the BIIPS_MODEL function
%   - variable_names: cell of strings. The names of the 
%                      unobserved variables to monitor. The names can
%                      contain subset indices which must define a valid subset of 
%                      the variables of the model, e.g.: {'var1', 'var2[1]',
%                      'var3[1:10]', 'var4[1, 5:10, 3]'}
%
%   OUTPUT
%   - obj_pimh: structure with fields meant for internal purpose only. They 
%               are used to query information on the current state of the
%               algorithm:
%               * model: biips model
%               * variable_names: cell with the names of the monitored variables
%               * sample: current sample
%               * log_marg_like: current value of the log marginal likelihood
%
%   See also BIIPS_MODEL, BIIPS_PIMH_UPDATE, BIIPS_PIMH_SAMPLES

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

check_struct(model, 'biips');

% TODO check variable_names ? it is currently done at the monitoring step
matbiips('message', 'Initializing PIMH');
obj_pimh.model = model;
obj_pimh.variable_names = variable_names;
obj_pimh.sample = [];
obj_pimh.log_marg_like = - Inf;
obj_pimh.class = 'pimh';