function [particles, log_marg_like] = biips_smc_samples(model, variable_names, n_part, varargin)

%
% BIIPS_SMC_SAMPLES runs a sequential Monte Carlo algorithm
%   [particles, log_marg_like] = biips_smc_samples(model, variable_names, 
%        nb_part, 'PropertyName', PropertyValue, ...)
%   INPUT: 
%   - model:        structure containing the model, 
%                   returned by the 'biips_model' function
%    - variable_names: cell of strings. Contains the names of the 
%                      unobserved variables to monitor.
%                      Possible value: {'var1', 'var2[1]', 'var3[1:10]',
%                                                       'var4[1, 5:10, 3]'}
%                      Dimensions and indices must be a valid subset of 
%                      the variables of the model.
%    - n_part:         positive integer. Number of particles used in SMC algorithms
%
%   Optional Inputs:
%   - type:     string (default = 'fs').
%               Its characters must be in the set {'f', 's', 'b'} for 
%               respectively 'filtering', 'smoothing' and 'backward smoothing'.
%               can use multiple letters, eg: 'fsb' for the three algorithms.
%   - rs_thres: positive real (default = 0.5).
%               Threshold for the resampling step (adaptive SMC).
%               if rs_thres is in [0,1] --> resampling occurs when 
%                                           (ESS > rs_thres * nb_part)
%               if rs_thres is in [2,nb_part] --> resampling occurs when 
%                                               (ESS > rs_thres)
%   - rs_type : string (default = 'stratified')
%               Possible values are 'stratified', 'systematic', 'residual', 'multinomial'
%               Indicates the type of algorithm used for the resampling step.
%
%   OUTPUT:
%   - particles:    output structure containing all the SMC information
%                   with following fields: 
%                   * names of the variables monitored with subfields
%                       * f, s and/or b for filtering, smoothing or backward
%                       smoothing with subfields
%                           * values: array of size [dim, n_part] with values of the particles
%                           * weights: array of size [dim, n_part] with weights of the particles
%                           * ess: array of size dim with effective sample size 
%                           * discrete: array of size dim with boolean indicating discreteness
%                           * conditionals: lists of the contitioning variables (observations).
%                               - filtering: cell array of size dim. each element is a cell of strings
%                                 with the respective contitioning variables of the node array element.
%                               - smoothing/backward_smoothing: cell of strings, the same for all the 
%                                 elements of the node array.
%                           * name: string with the name of the variable
%                           * lower: vector with the lower bounds of the variable
%                           * upper: vector with the upper bounds of the variable
%                           * type: string ('filtering', 'smoothing' or 'backward_smoothing')
%                   Example to access values of the particles for smoothing for variable x: 
%                   particles.x.s.values
%   - log_marg_like:log marginal likelihood
%
%   See also BIIPS_MODEL
%--------------------------------------------------------------------------
% EXAMPLE:
% data = struct('var1', 0, 'var2', 1.2);
% model = biips_model('model.bug', data)
% n_part = 100; variables = {'x'}; 
% type = 'fs'; rs_type = 'multinomial'; rs_thres = 0.5;
% out_smc = biips_smc_samples(model, variables, n_part, 'type', type,...
%               'rs_type', rs_type, 'rs_thres', rs_thres);
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 13-03-2014
%--------------------------------------------------------------------------

%% PROCESS AND CHECK INPUTS
optarg_names = {'type', 'rs_thres', 'rs_type'};
optarg_default = {'fs', .5, 'stratified'};
optarg_valid = {{'f', 's', 'b', 'fs', 'fb', 'sb', 'fsb'}, [0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'}};
optarg_type = {'char', 'numeric', 'char'};
[type, rs_thres, rs_type] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

indices = arrayfun(@(x) strfind(type,x), 'fsb', 'UniformOutput', 0);
filtering = ~isempty(indices{1});
smoothing = ~isempty(indices{2}); 
backward = ~isempty(indices{3});

check_struct(model, 'biips');
console = model.id; % Get the id of the biips console

%% Monitor
if (backward)
    matbiips('set_default_monitors', console);
end
if (~isempty(variable_names))
    monitor(console, variable_names, type); 
end

%% Run smc_sample
smc_forward_algo(console, n_part, rs_thres, rs_type, get_seed());

log_marg_like = matbiips('get_log_norm_const', console);


cz=cell(0,1);
if filtering % Get filtering output
    mon1 = matbiips('get_filter_monitors', console);
    names = fieldnames(mon1);
    cz = struct2cell(mon1);
    if (~backward)
       clear_monitors(console, 'f');
    end   
end

if smoothing % Get smoothing output
    mon2 = matbiips('get_gen_tree_smooth_monitors', console);
    names = fieldnames(mon2);
    cz = horzcat(cz, struct2cell(mon2));
    clear_monitors(console, 's');
end

if (backward) % Get backward smoothing output
   matbiips('run_backward_smoother', console);
   clear_monitors(console, 'f'); 
   mon3 = matbiips('get_backward_smooth_monitors', console);
   names = fieldnames(mon3);    
   clear_monitors(console, 'b'); 
   cz = horzcat(cz, struct2cell(mon3));
end

for i=1:numel(type)
    fsb{i} = type(i);
end

nb_names = numel(names);
cell_names = cell(nb_names, 1);
for i=1:nb_names
   cell_names{i} = cell2struct({cz{i, :}}, fsb, 2);
end
particles = cell2struct_weaknames(cell_names, names); % Allows to add fields with brackets in the structure
