function [variables_out] = biips_smc_samples(console, variables_names, nb_part, varargin)
% BIIPS_SMC_SAMPLES main routine implementing SMC algorithms
% [variables_out] = biips_smc_samples(console, variables_names, nb_part, type, rs_thres, rs_type, seed)
% INPUT : 
% - console : id of the console containing the model
% - variables_names : cell containing the names of variables to monitor
% - nb_part : the number of particules used in smc algorithms (integer)
% - type : cell of strings belonging to {"filtering", "smoothing", "backward.smoothing"} 
% - rs_thres : resampling threshold ( if real in  [0,1] ---> percentage of nb_particules
%                                        integer in [1,nb_part] --> number of particules
% - rs_type : string belonging to {'stratified', 'systematic', 'residual', 'multinomial' } indicating 
%             the kind of algorithm used for esampling
% - seed : seed for random number generator
% check for optional options
opt_argin = length(varargin);
% default values
type='fsb';
res_type = 'stratified';
s=rng('shuffle');
seed=randi(intmax);
rng(s);

if opt_argin >= 1
   type = varargin{1};
end

if opt_argin >=2 
   res_thres = varargin{2};
end   

if opt_argin >=3
   res_type = varargin{3};
end    

if opt_argin >=4
    seed = varargin{4};
end    

indices = arrayfun(@(x) strfind(type,x), 'fsb', 'UniformOutput', 0); 
backward = ~isempty(indices{3});
if (backward)
    inter_biips('set_default_monitors', console);
end
if (~isempty(variables_names))
    monitor_biips(console, variables_names, type); 
end
log_marg_like = inter_biips('get_log_norm_const', console);
mon = inter_biips('get_filter_monitors', console);
