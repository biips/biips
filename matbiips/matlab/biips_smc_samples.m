function [var_out] = biips_smc_samples(console, variable_names, nb_part, varargin)
% BIIPS_SMC_SAMPLES main routine implementing SMC algorithms
% [variables_out] = biips_smc_samples(console, variable_names, nb_part, [ type, rs_thres, rs_type, seed ])
% INPUT : 
% - console : integer. Id of the console containing the model, returned by the 'biips_model' function
% - variable_names : cell of strings. Contains the names of the unobserved variables to monitor.
%                    Possible value: {'x', 'foo', 'bar[1]', 'x.state[1:10]', 'var[1, 5:10, 3]'}
%                    Dimensions and indices must be a valid subset of the variables of the model.
% - nb_part : positive integer. The number of particules used in smc algorithms
% - type : string (default = 'fs').
%          Its characters must be in the set {'f', 's', 'b'} for respectively 'filtering', 'smoothing' and 'backward smoothing'.
%          Controls the desired types of SMC algorithms, hence the types of values returned in the output structure 'variables_out'.
%          Can contain several characters, eg: 'fsb' for the three algorithms.
%          Note that backward smoothing algorithm ('b') is costly: O(n_part²) vs filtering and smoothing O(n_part)
% - rs_thres : positive real (default = 0.5).
%              Threshold for controling the activation of the resampling step (adaptive SMC).
%              if rs_thres is in [0,1] --> resampling occurs when (ESS > rs_thres * nb_part)
%              if rs_thres is in [2,nb_part] --> resampling occurs when (ESS > rs_thres)
% - rs_type : string (default = 'stratified')
%             Possible values are 'stratified', 'systematic', 'residual', 'multinomial'
%             Indicates the type of algorithm used for the resampling step.           
% - seed : integer (default is set randomly). Seed for random number generator.

% check for optional options
opt_argin = length(varargin);
% default values
type='fsb';
rs_type = 'stratified';
rs_thres = 0.5;
if (~exist('OCTAVE_VERSION')) % check MATLAB/OCTAVE run
  s=rng('shuffle');
  seed=randi(intmax);
  rng(s);
else
  s=rand('state');
  rand('state',time);
  seed=randi(intmax);
  rand('state',s);
end

if opt_argin >= 1
   type = varargin{1};
end

if opt_argin >=2 
   rs_thres = varargin{2};
end   

if opt_argin >=3
   rs_type = varargin{3};
end    

if opt_argin >=4
    seed = varargin{4};
end    

indices = arrayfun(@(x) strfind(type,x), 'fsb', 'UniformOutput', 0); 
backward = ~isempty(indices{3});
% monitor
if (backward)
    inter_biips('set_default_monitors', console);
end
if (~isempty(variable_names))
    monitor_biips(console, variable_names, type); 
end

% run smc_sample
ok = run_smc_forward(console, nb_part, rs_thres, rs_type, seed);

log_marg_like = inter_biips('get_log_norm_const', console);
mon1 = inter_biips('get_filter_monitors', console);
noms = fieldnames(mon1);
cz1 = struct2cell(mon1);
cz = { cz1{:} };
clear cz1{:} ;
if (~backward)
   biips_clear_monitors(console, 'f');
end   

mon2 = inter_biips('get_smooth_tree_monitors', console);
biips_clear_monitors(console, 's');
cz2 = struct2cell(mon2);
cz = { cz{:} ; cz2 };
clear cz2;

if (backward)
   inter_biips('run_backward_smoother', console);
   biips_clear_monitors(console, 'f'); 
   mon3 = inter_biips('get_smooth_tree_monitors', console);
   cz3 = struct2cell(mon3);
   biips_clear_monitors(console, 'b'); 
   cz = { cz{:} , cz3{:} };
   clear cz3;
end

fsb = {'f', 's', 'b' };
fsb = {fsb{1:size(cz,1)} };

nb_noms = length(noms);
cell_noms = cell(nb_noms,1);
for i=1:nb_noms
   cell_noms{i} = cell2struct({cz{:,i}}, fsb, 2);
end   
var_out = inter_biips('cell2struct_weak_names', cell_noms, noms);
