function [variables_out] = biips_smc_samples(console, variable_names, nb_part, varargin)
% BIIPS_SMC_SAMPLES main routine implementing SMC algorithms
% [variables_out] = biips_smc_samples(console, variable_names, nb_part, type, rs_thres, rs_type, seed)
% INPUT : 
% - console : id of the console containing the model
% - variable_names : cell containing the names of variables to monitor
% - nb_part : the number of particules used in smc algorithms (integer)
% - type : strings with characters in {'f', 's', 'b'} 
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
mon = inter_biips('get_filter_monitors', console);

if (~backward)
   biips_clear_monitors(console, 'f');
end   


