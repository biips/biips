function out = biips_smc_sensitivity(model, param_names, param_values, ...
    n_part, varargin)

%
% BIIPS_SMC_SENSITIVITY performs sensitivity analysis of parameters with SMC
%  out = biips_smc_sensitivity(console, param_names, param_values, ...
%    n_part, 'PropertyName', propertyvalue, ...)
%
%   INPUT
%   - model:        structure containing the model,
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
%   See also BIIPS_MODEL, BIIPS_SMC_SAMPLES
%--------------------------------------------------------------------------
% EXAMPLE:
% model = biips_model('file.bug', data);
% n_part = 100;
% param_names = {'log_prec_y[1:1]'};
% param_values = {-5:.2:3};
% out = biips_smc_sensitivity(model, param_names, param_values, n_part);
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

n_param = numel(param_names);
if n_param~=numel(param_values)
    error('param_names and param_values must be cells of the same size')
end
pn_param =  cellfun(@parse_varname, param_names);

if ~all(cellfun(@isnumeric, param_values))
    error('param_values must be a cell of numerics');
end

% Get the sizes of parameter values
sizes = cellfun(@size, param_values, 'uniformoutput', false);

% inline if, elseif, else function.
% usage: ret = ifelse(cond_1, return_1, cond_2, return_2, ..., true, return_default)
ifelse  = @(varargin) varargin{2*find([varargin{1:2:end}], 1, 'first')}(); 

% Get the number of parameter values to evaluate
nval_fun = @(s) ifelse((numel(s)==2 && s(2)==1), s(1), true, s(end));
nval = cellfun(nval_fun, sizes, 'uniformoutput', false);

n_values = nval{1};

% Check if number of parameter values match
if any(cellfun(@(x) x~= n_values, nval))
    error('Invalid param_values argument: number of values to evaluate does not match')
end

% Get the dimensions of parameters
dim_fun = @(s) ifelse((numel(s)==2 && s(2)==1), [1, 1], (numel(s)==2), [s(1), 1], true, s(1:end-1));
dimen = cellfun(dim_fun, sizes, 'uniformoutput', false);

len = cellfun(@prod, dimen, 'uniformoutput', false);


%% PROCESS AND CHECK INPUTS
optarg_names = {'rs_thres', 'rs_type'};
optarg_default = {.5, 'stratified'};
optarg_valid = {[0, n_part], ...
    {'multinomial', 'stratified', 'residual', 'systematic'}};
optarg_type = {'numeric', 'char'};
[rs_thres, rs_type] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

check_struct(model, 'biips');

%% Stops biips verbosity
verb = matbiips('verbosity', 0);
cleanupObj = onCleanup(@() matbiips('verbosity', verb));% set verbosity on again when function terminates

%% Create a clone model
model2 = clone_model(model);
console = model2.id;
%monitor(console, variable_names, 's');
if (~matbiips('is_sampler_built', console))
    matbiips('build_smc_sampler', console, false);
end


%% initialize
log_marg_like = zeros(n_values, 1);
log_marg_like_pen = zeros(n_values, 1);
max_log_marg_like = -Inf;
max_log_marg_like_pen = -Inf;
max_param = [];
max_param_pen = [];

matbiips('message', ['Analyzing sensitivity with ' num2str(n_part) ' particles']);
% Progress bar
bar = matbiips('make_progress_bar', n_values, '*', 'iterations');

%% Iterate over different values
for k=1:n_values
    % Update value of each parameter
    value = cell(n_param, 1);
    for i=1:n_param
        var = param_names{i};
        ind = ((k - 1) * len{i} + 1) : (k * len{i});
        value{i} = reshape(param_values{i}(ind), dimen{i});
        ok = matbiips('change_data', console, pn_param(i).name, ...
            pn_param(i).lower, pn_param(i).upper, value{i}, false);
        if ~ok
            error('Data change failed: invalid parameter %s = %s.\n', var, sprintf('%g ', value{i}));
        end
    end
    
    % get log prior
    log_prior = 0;
    for i=1:n_param
        log_p = matbiips('get_log_prior_density', console, pn_param(i).name, ...
            pn_param(i).lower, pn_param(i).upper);
        log_prior = log_prior + log_p;
    end
    
    % run smc sampler
    ok = smc_forward_algo(console, n_part, rs_thres, rs_type, get_seed());
    if ~ok
        error('Failure running SMC forward sampler')
    end
    
    % log marginal likelihood
    log_marg_like(k) = matbiips('get_log_norm_const', console);
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
    matbiips('advance_progress_bar', bar, 1);
end

%% Delete clone console
matbiips('clear_console', console);

out.log_marg_like = log_marg_like;
out.log_marg_like_pen = log_marg_like_pen;
out.max_param = max_param;
out.max_log_marg_like = max_log_marg_like;
out.max_param_pen = max_param_pen;
out.max_log_marg_like_pen = max_log_marg_like_pen;
