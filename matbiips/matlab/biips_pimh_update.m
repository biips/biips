function obj_pimh = biips_pimh_update(obj_pimh, n_iter, n_part, varargin)

%
% BIIPS_PIMH_UPDATE performs burn-in iterations for the PIMH algorithm
% biips_pimh_update(console, variable_names, n_iter, n_part, varargin)
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
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------


%% PROCESS AND CHECK INPUTS
optarg_names = {'rs_thres', 'rs_type'};
optarg_default = {.5, 'stratified'};
optarg_valid = {[0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'}};
optarg_type = {'numeric', 'char'};
[rs_thres, rs_type] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

% console = model.id;
check_struct_model(obj_pimh.model);

%% Stops biips verbosity
matbiips('verbosity', 0);
cleanupObj = onCleanup(@() matbiips('verbosity', 1));% set verbosity on again when function terminates


%% Create a clone model for the PIMH
variable_names = obj_pimh.variable_names;
% model2 = clone_model(obj_pimh.model);
console = obj_pimh.model.id;
monitor_biips(console, variable_names, 's'); 
if (~matbiips('is_sampler_built', console))
   matbiips('build_smc_sampler', console, false);
end

%% Get sample and log likelihood from PIMH object
sample = obj_pimh.sample;
variable_names = obj_pimh.variable_names;
log_marg_like = obj_pimh.log_marg_like;

% %% Initialization
% [sample, log_marg_like] = pimh_init(console, variable_names, n_part, rs_thres, rs_type);
matbiips('message', ['Updating PIMH with ' num2str(n_part) ...
    ' particles and ' num2str(n_iter) ' iterations']);

bar = matbiips('make_progress_bar', n_iter, '*', 'iterations');

% Independent Metropolis-Hastings iterations
for i=1:n_iter
    [sample, log_marg_like, accepted] = pimh_one_update(console, ...
        variable_names, n_part, rs_thres, rs_type, sample, log_marg_like);
    
    % Progress bar
    matbiips('advance_progress_bar', bar, 1);    
end

% Release monitor memory
clear_monitors(console, 's', true);

% % Delete clone console
% matbiips('clear_console', console);

obj_pimh.sample = sample;
obj_pimh.log_marg_like = log_marg_like;
