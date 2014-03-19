function biips_pimh_update(console, variable_names, n_iter, n_part, varargin)

%
% BIIPS_PIMH_UPDATE performs burn-in iterations for the PIMH algorithm
% biips_pimh_update(console, variable_names, n_iter, n_part, varargin)
%
%   INPUT: 
%   - console:      integer. Id of the console containing the model, 
%                   returned by the 'biips_model' function
%   - variable_names: cell of strings. Contains the names of the 
%                   unobserved variables to monitor.
%                   Possible value: {'var1', 'var2[1]', 'var3[1:10]',
%                                                       'var4[1, 5:10, 3]'}
%                   Dimensions and indices must be a valid subset of 
%                   the variables of the model.
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
%   See also BIIPS_MODEL, BIIPS_PIMH_SAMPLES
%--------------------------------------------------------------------------
% EXAMPLE:
% data = struct('var1', 0, 'var2', 1.2);
% model_id = biips_model('model.bug', data)
% npart = 100; variables = {'x'}; 
% biips_pimh_update(model_id, variables, npart);
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, François Caron
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

%% Stops biips verbosity
inter_biips('verbosity', 0);
cleanupObj = onCleanup(@() inter_biips('verbosity', 1));% set verbosity on again when function terminates

%% Initialization
[sample, log_marg_like] = pimh_init(console, variable_names, n_part, rs_thres, rs_type);
inter_biips('message', ['Updating PIMH with ' num2str(n_part) ...
    ' particles and ' num2str(n_iter) ' iterations']);

bar = inter_biips('make_progress_bar', n_iter, '*', 'iterations');

% Independent Metropolis-Hastings iterations
for i=1:n_iter
    [sample, log_marg_like, accepted] = pimh_one_update(console, ...
        variable_names, n_part, rs_thres, rs_type, sample, log_marg_like);
    
    % Progress bar
    inter_biips('advance_progress_bar', bar, 1);    
end

% Release monitor memory
clear_monitors(console, 's', true);


% Reset lognormalizing constant and sampled value
if (n_iter>0 && ~accepted)
    inter_biips('set_log_norm_const', console, log_marg_like);
    inter_biips('set_sampled_gen_tree_smooth_particle', console, sample); 
end