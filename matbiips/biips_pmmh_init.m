function obj = biips_pmmh_init(model, param_names, varargin)

%
% BIIPS_PMMH_INIT creates a PMMH object
% obj = biips_pmmh_init(model, param_names, 'PropertyName', propertyvalue, ...)
%
%   INPUT 
%   - model:        structure containing the model, 
%                   returned by the 'biips_model' function
%   - param_names:  cell of strings containing the list of variables to be
%                   updated with MH proposal. Other are updated with SMC
%   Optional Inputs:
%   - inits:        cell of numeric values of the same length as param_names 
%                   containing init values for the parameters
%   - rw_step:      cell of numeric values of the same length as param_names 
%                   containing random walk variance
%   - n_rescale:    Number of iterations for rescaling
%   - beta:         Parameter of the proposal (default=0.05)
%   - alpha:        Tuning parameter of the rescaling (default=0.99)
%
%   OUTPUT
%  - obj:           PMMH structure
%
%   See also BIIPS_MODEL, BIIPS_PMMH_UPDATE, BIIPS_PMMH_SAMPLES
%--------------------------------------------------------------------------
% EXAMPLE:
% n_burn = 2000; n_iter = 2000; thin = 1; n_part = 50; 
% param_names = {'log_prec_y[1:1]';}
% latent_names = {'x'};
% obj_pmmh = biips_pmmh_object(model, param_names, 'inits', {-2});
% obj_pmmh = biips_pmmh_update(obj_pmmh, n_burn, n_part); 
% [out_pmmh, log_post, log_marg_like, stats_pmmh] = ...
%   biips_pmmh_samples(obj_pmmh, n_iter, n_part,'thin', 1, 'latent_names', latent_names); 
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

%%% Process and check optional arguments
optarg_names = {'inits', 'rw_step', 'n_rescale', 'beta', 'alpha', 'latent_names'};
optarg_default = {{}, [], 400, .05, .99, {}};
optarg_valid = {{}, [], [0,intmax], [0,1], [0,1],{}};
optarg_type = {'numeric', 'numeric', 'numeric', 'numeric', 'numeric', 'char'};
[inits, rw_step, n_rescale, beta, alpha, latent_names] = parsevar(varargin, optarg_names,...
    optarg_type, optarg_valid, optarg_default);

check_struct(model, 'biips'); % Checks if the structure model is valid

%%% TODO check param_names and latent_names are valid
  
% Check param_names
for i=1:numel(param_names)
    if ~ischar(param_names{i})
        error('Invalid parameter name %s', param_names{i})
    end
end
pnames = cellfun(@to_biips_vname, param_names, 'uniformoutput', false);
if numel(pnames)~=numel(unique(pnames))
    error('duplicated names in param_names')
end
pn_param = cellfun(@parse_varname, param_names);
n_param = numel(param_names);

% Check latent_names
lnames = cellfun(@to_biips_vname, latent_names, 'uniformoutput', false);
if numel(lnames)~=numel(unique(lnames))
    error('duplicated names in latent_names')
end
cellfun(@parse_varname, latent_names);

% Check the init values
if ~isempty(inits)
    if numel(inits)~=numel(param_names)
        error('Inits must be a cell with the same length as param_names')
    end
    for i=1:n_param
        % Check values
        if any(isnan(inits{i}(:)) | isinf(inits{i}(:)))
            error('invalid init values for variable %s', param_names{i})
        end
    end
end

% Check the rw_step values
if ~isempty(rw_step)
    if numel(rw_step)~=numel(param_names)
        error('rw_step must be a cell with the same length as param_names')
    end
    for i=1:n_param
        % Check values
        if any(isnan(rw_step{i}(:)) | isinf(rw_step{i}(:)) | rw_step{i}(:)<=0)
            error('invalid rw_step values for variable %s', param_names{i})
        end
    end
end

%% Stops biips verbosity
verb = matbiips('verbosity', 0);
cleanupObj = onCleanup(@() matbiips('verbosity', verb));% reset verbosity when function terminates

%% display message
matbiips('message', 'Initializing PMMH');

%% Clone console
obj.model = clone_model(model);
console = obj.model.id;

%% Init the parameters of the random walk
sample_param = pmmh_set_param(console, param_names, pn_param, inits);

% Parameters and latent
obj.param_names = param_names;
obj.latent_names = latent_names;
obj.sample_param = sample_param;
obj.sample_latent = [];
obj.log_marg_like = -Inf;
obj.log_prior = -Inf;

sample_dim = cellfun(@size, sample_param, 'UniformOutput', false);

obj.dim = sample_dim;
obj.n_iter = 0;
obj.ar_mean = 0;
obj.alpha = alpha;
obj.beta = beta;
obj.n_rescale = n_rescale;
obj.n_cov = n_rescale/2; % we start learning the covariance matrix after ncov iterations

obj.len = sum(cellfun(@prod, sample_dim, 'UniformOutput', true));
if obj.len==1
    obj.target_prob = 0.44;
else
    obj.target_prob = 0.234;
end

% Init random walk stepsize for the part with diagonal covariance matrix
if isempty(rw_step)
    rw_step = cell(n_param, 1);
    % default values
    for i=1:n_param
        rw_step{i} = .1/sqrt(obj.len)*ones(sample_dim{i});
    end
else
    for i=1:n_param
        % Check dimensions
        if any(size(rw_step{i})~=sample_dim{i})
            error('rw_step must be of the same dimension as the variable %s', param_names)
        end
    end
end

% Concatenate all log value in a vector
obj.log_step = cell2mat(cellfun(@(x) log(x(:)), rw_step(:), 'UniformOutput', false));

% Covariance matrix
obj.rw_mean = [];
obj.rw_cov = [];  

obj.class = 'pmmh';