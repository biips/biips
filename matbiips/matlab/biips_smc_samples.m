function [variables_out] = biips_smc_samples(console, variables_names, varargin)
% BIIPS_SMC_SAMPLES main routine implementing SMC algorithms
% [variables_out] = biips_smc_samples(console, variables_names, type, nb_part, rs_thres, rs_type)
% INPUT : 
% - console : id of the console containing the model
% - variables_names : cell containing the names of variables to monitor
% - type : cell of strings among "filtering", "smoothing" and "backward.smoothing" indicationg
%           the processings to do*
% - nb_part : the number of particules used in smc algorithms (integer)
% - rs_thres : resampling threshold ( if real in  [0,1] ---> percentage of nb_particules
%                                        integer in [1,nb_part] --> number of particules
% - rs.type : string belonging to {'stratified', 'systematic', 'residual', 'multinomial' } indicating 
%             the kind of algorithm used for esampling
% check for optional options
opt_argin = length(varargin);
% defauts values
type={'filtering', 'smoothing', 'backward.smoothing'};

if opt_argin >= 1
   sample_data = varargin{1};
end
if opt_argin >=2 
   data_rng_seed = varargin{2};
end   

if opt_argin >=3
    quiet = varargin{3};
end    


