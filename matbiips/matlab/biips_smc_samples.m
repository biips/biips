function [variables_out] = biips_smc_samples(console, variables_names, nb_part, varargin)
% BIIPS_SMC_SAMPLES main routine implementing SMC algorithms
% [variables_out] = biips_smc_samples(console, variables_names, nb_part, type, rs_thres, rs_type)
% INPUT : 
% - console : id of the console containing the model
% - variables_names : cell containing the names of variables to monitor
% - nb_part : the number of particules used in smc algorithms (integer)
% - type : cell of strings among "filtering", "smoothing" and "backward.smoothing" indicationg
%           the processings to do*
% - rs_thres : resampling threshold ( if real in  [0,1] ---> percentage of nb_particules
%                                        integer in [1,nb_part] --> number of particules
% - rs_type : string belonging to {'stratified', 'systematic', 'residual', 'multinomial' } indicating 
%             the kind of algorithm used for esampling
% check for optional options
opt_argin = length(varargin);
% defauts values
type={'filtering', 'smoothing', 'backward_smoothing'};

res_type = 'stratified';

if opt_argin >= 1
   type = varargin{1};
end

if opt_argin >=2 
   res_thres = varargin{2};
end   

if opt_argin >=3
   res_ttype = varargin{3};
end    

backward = any(cellfun(@(x) strcmp(x,'backward_smoothing'),type));
if (backward)
    inter_biips('set_default_monitors', p);
end

mon = inter_biips('get_filter_monitors', p);


