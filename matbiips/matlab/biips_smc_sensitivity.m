function out = biips_smc_sensitivity(console, param_names, param_values, ...
    n_part, varargin)

%
% BIIPS_SMC_SENSITIVITY performs sensitivity analysis of parameters with SMC
%  out = biips_smc_sensitivity(console, param_names, param_values, ...
%    n_part, 'PropertyName', propertyvalue, ...)
%
%   INPUT 
%   - console:      integer. Id of the console containing the model, 
%                   returned by the 'biips_model' function
%   - param_names:  cell of strings. Contains the names of the 
%                   unobserved variables for sensitivity analysis
%   - param_values: cell of numeric values, of the same length as
%                   param_names.
%   Optional Inputs:
%   - rs_thres :    positive real (default = 0.5).
%                   Threshold for the resampling step (adaptive SMC).
%                   if rs_thres is in [0,1] --> resampling occurs when 
%                                           (ESS > rs_thres * nb_part)
%                   if rs_thres is in [2,nb_part] --> resampling occurs when 
%                                               (ESS > rs_thres)
%   - rs_type :     string (default = 'stratified')
%                   Possible values are 'stratified', 'systematic', 'residual', 'multinomial'
%                   Indicates the type of algorithm used for the resampling step.
%
%   OUTPUT
%   - out:          Structure with the following fields:
%                   * log_marg_like
%                   * log_marg_like_pen
%                   * max_param
%                   * max_log_marg_like
%                   * max_param_pen
%                   * max_log_marg_like_pen
%
%   See also BIIPS_MODEL
%--------------------------------------------------------------------------
% EXAMPLE:
% model_id = 1; n_part = 100; 
% param_names = {'log_prec_y[1:1]'};
% param_values = {-5:.2:3}; 
% out = biips_smc_sensitivity(model_id, param_names, param_values, n_part); 
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, François Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------


n_params = length(param_names);
if n_params~=length(param_values)
    error('param_names and param_values must be cells of the same size')
end
pn_param =  cellfun(@parse_varname, param_names);

if ~prod(cellfun(@isnumeric, param_values))
    error('param_values must be a cell of numerics');
end


% Get the number of parameter values to evaluate
dimension = size(param_values{1});
if (length(dimension)==2 && dimension(2)==1) % scalar parameter
    n_values = dimension(1);
else
    n_values = dimension(end);
end
% Check if number of parameter values match
for i=2:n_params
    dimension = size(param_values{i});
    if (length(dimension)==2 && dimension(2)==1) % scalar parameter#
        if n_values ~= dimension(1)
            error('Invalid param_values argument: number of values to evaluate do not match')
        end
    else
        if n_values ~= dimension(end)
            error('Invalid param_values argument: number of values to evaluate do not match')
        end
    end
end


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

% initialize
log_marg_like = zeros(n_values, 1);
log_marg_like_pen = zeros(n_values, 1);
max_log_marg_like = -Inf;
max_log_marg_like_pen = -Inf;
max_param = [];
max_param_pen = [];

inter_biips('message', ['Analyzing sensitivity with ' num2str(n_part) ' particles']);
% Progress bar
bar = inter_biips('make_progress_bar', n_values, '*', 'iterations');
 % Iterate over different values
 for k=1:n_values
     % Update value of each parameter
     for i=1:n_params
         var = param_names{i};
         size_i = size(param_values{i});
         switch(length(size_i))
             case 2
                 if size_i(2)==1
                     value{i} = param_values{i}(k);
                 else
                     value{i} = param_values{i}(:, k);
                 end
             case 3
                 value{i} = param_values{i}(:, :, k);
             case 4
                 value{i} = param_values{i}(:, :, :, k);
         end
         tag = inter_biips('change_data', console, pn_param(i).name, ...
             pn_param(i).lower, pn_param(i).upper, value{i}, false);
         if ~tag
             error('Data change failed: invalid parameter %s = %.f.', var, value{i});
         end              
     end
     
     log_prior = 0;
     for i=1:n_params
         log_p = inter_biips('get_log_prior_density', console, pn_param(i).name, ...
             pn_param(i).lower, pn_param(i).upper);
         log_prior = log_prior + log_p;
     end
     ok = run_smc_forward(console, n_part, rs_thres, rs_type, get_seed());
     if ~ok
         error('Failure running SMC forward sampler')        
     end
     
     % log marginal likelihood
     log_marg_like(k) = inter_biips('get_log_norm_const', console);
     if log_marg_like(k)>max_log_marg_like
         max_log_marg_like = log_marg_like(k);
         max_param = value;
     end
     log_marg_like_pen(k) = log_marg_like(k) + log_prior;
     if log_marg_like_pen(k)>max_log_marg_like_pen
         max_log_marg_like_pen = log_marg_like_pen(k);
         max_param_pen = value;
     end
     
     % Advance progress bar
     inter_biips('advance_progress_bar', bar, 1);
 end
out.log_marg_like = log_marg_like;
out.log_marg_like_pen = log_marg_like_pen;
out.max_param = max_param;
out.max_log_marg_like = max_log_marg_like;
out.max_param_pen = max_param_pen;
out.max_log_marg_like_pen = max_log_marg_like_pen; 