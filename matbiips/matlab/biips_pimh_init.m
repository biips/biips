function [obj_pimh] = biips_pimh_init(model, variable_names)

%--------------------------------------------------------------------------
% BIIPS_PIMH_INIT initializes the Particle Independent Metropolis-Hastings algorithm
% [sample, log_marg_like] = biips_pimh_init(model, variable_names, n_part, rs_thres, rs_type)
%
%   INPUT
%   - model:    structure. Biips model, returned by the 'biips_model' function
%   - variable_names: cell of strings. Contains the names of the 
%               unobserved variables to monitor.
%               Possible value: {'var1', 'var2[1]', 'var3[1:10]',
%                                                'var4[1, 5:10, 3]'}
%               Dimensions and indices must be a valid subset of 
%               the variables of the model.
%
%   OUTPUT
%   - obj_pimh: structure containing
%               * sample: current sample
%               * log_marg_lik: current value of the log marginal likelihood
%               * variable_names: cell with names of the variables to be
%                 monitored
%               * model: structure, biips model
%
%   See also BIIPS_MODEL, BIIPS_PIMH_UPDATE, BIIPS_PIMH_SAMPLES
%--------------------------------------------------------------------------
% EXAMPLE:
% data = struct('var1', 0, 'var2', 1.2);
% model = biips_model('model.bug', data)
% variables = {'x'}; 
% nburn = 1000; niter = 1000; npart = 100; 
% obj_pimh = biips_pimh_init(model, variables); 
% obj_pimh = biips_pimh_update(obj_pimh, nburn, npart); 
% [obj_pimh, samples_pimh] = biips_pimh_samples(obj_pimh, niter, npart); 
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 03-04-2014
%--------------------------------------------------------------------------

matbiips('message', 'Initializing PIMH');
obj_pimh.log_marg_like = - Inf;
obj_pimh.sample = [];
obj_pimh.variable_names = variable_names;
obj_pimh.model = model;