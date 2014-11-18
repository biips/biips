function out_sens = biips_smc_sensitivity(model, param_names, param_values, ...
    n_part, varargin)
% BIIPS_SMC_SENSITIVITY Perform sensitivity analysis of parameters with SMC.
%  out_sens = biips_smc_sensitivity(model, param_names, param_values, ...
%    n_part, 'PropertyName', propertyvalue, ...)
%
%   INPUT:
%   - model:        Biips model as returned by the BIIPS_MODEL function
%   - param_names:  cell of strings. Names of the parameters for which we 
%                   want to study sensitivity.
%   - param_values: cell of numeric values, of the same length as
%                   param_names. Each element is an array whose last dimension is the number of
%                   values for which we want to study sensitivity.
%   - n_part:       integer. Number of particles.
% 
%   Optional Inputs:
%   - rs_thres, rs_type, ... : Additional arguments to be passed to the SMC
%      algorithm. See BIIPS_SMC_SAMPLES for for details.
%
%   OUTPUT:
%   - out_sens:  Structure with the following fields:
%                   * log_marg_like: vector of log marginal likelihood
%                   estimates at the different values of the parameters.
%                   * log_marg_like_pen: vector of penalized log marginal likelihood
%                   estimates at the different values of the parameters.
%                   * max_param: parameters value with maximum log_marg_like.
%                   * max_log_marg_like: maximum log_marg_like value.
%                   * max_param_pen: parameters value with maximum
%                   log_marg_like_pen.
%                   * max_log_marg_like_pen: maximum log_marg_like_pen
%                   value.
%
%   See also BIIPS_MODEL, BIIPS_SMC_SAMPLES

%--------------------------------------------------------------------------
% EXAMPLE:
% modelfile = 'hmm.bug';
% type(modelfile);
% 
% data = struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1), 'logtau', log(1));
% model = biips_model(modelfile, data);
% 
% n_part = 50;
% logtau_val = -10:10;
% out_sens = biips_smc_sensitivity(model, {'logtau'}, {logtau_val}, n_part);
% 
% figure
% subplot(2,1,1); hold on
% title('SMC sensitivity')
% plot(logtau_val, out_sens.log_marg_like)
% plot(data.logtau, min(out_sens.log_marg_like), 'g^', 'markerfacecolor', 'g')
% xlabel('logtau')
% ylabel('log p(y|logtau)')
% 
% subplot(2,1,2); hold on
% plot(logtau_val, out_sens.log_marg_like_pen)
% yl = ylim;
% plot(data.logtau, yl(1), 'g^', 'markerfacecolor', 'g')
% xlabel('logtau')
% ylabel('log p(y|logtau) + log p(logtau)')
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 21-10-2014
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
            error('Data change failed: invalid parameter %s = %s\n', var, sprintf('%g ', value{i}));
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

out_sens.log_marg_like = log_marg_like;
out_sens.log_marg_like_pen = log_marg_like_pen;
out_sens.max_param = max_param;
out_sens.max_log_marg_like = max_log_marg_like;
out_sens.max_param_pen = max_param_pen;
out_sens.max_log_marg_like_pen = max_log_marg_like_pen;
