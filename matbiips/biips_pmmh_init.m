function obj_pmmh = biips_pmmh_init(model, param_names, varargin)
% BIIPS_PMMH_INIT Create a PMMH object.
%   Initializes the Particle Marginal Metropolis-Hastings algorithm
%   obj_pmmh = biips_pmmh_init(model, param_names, 'PropertyName', PropertyValue, ...)
% 
%   The PMMH algorithm is a particle MCMC algorithm using SMC within a MCMC
%   algorithm. It splits the variables in the graphical model into two sets:
%   - The parameters in 'param_names' are sampled with a MH proposal
%     (multivariate Gaussian random walk). They must be continuous and must 
%     be defined as single stochastic nodes in the model.
%   - The other latent variables are sampled using a SMC algorithm. They can
%     be monitored using the 'latent_names' parameter.
%
%   INPUT 
%   - model:    Biips model as returned by the BIIPS_MODEL function
%   - param_names:  cell of strings. The names of the variables updated with
%                   MH proposal. The names can contain subset indices which 
%                   must define a valid subset of the variables of the model, 
%                   e.g.: {'var1', 'var2[1]', 'var3[1:10]', 'var4[1, 5:10, 3]'}
%   Optional Inputs:
%   - latent_names: cell of strings. The names of the variables to be
%                   updated with SMC proposal that need to be monitored.
%   - inits:        cell of numeric values of the same length as param_names.
%                   Init values for the parameters in param_names.
%                   (default = samples from the prior distribution)
%   - transform:    logical. Activate automatic parameters transformation (default = true).
%                   Transformations apply independently to each component 
%                   of the parameters depending on their support:
%                       * unbounded (-Inf, +Inf): f(x) = x
%                       * lower bounded [L, +Inf): f(x) = log(x-L)
%                       * upper bounded (-Inf, U]: f(x) = log(U-x)
%                       * lower-upper bounded [L, U]: f(x) = log((x-L)/(U-x))
%                   so that we apply a random walk on the real-valued transformed
%                   vector.
%   - rw_step:      cell of numeric values of the same length as
%                   param_names. Random walk standard deviations. If
%                   transform is true (default), the given standard deviations 
%                   apply to the transformed variables. Set transform to 
%                   false if you wish to set the random walk standard deviation
%                   for the untransformed random variables.
%   - n_rescale:    Number of iterations for rescaling (default = 400)
%   - alpha:        Tuning parameter of the rescaling (default = 0.99)
%   - beta:         Tuning parameter of the proposal (default = 0.05)
%
%   OUTPUT
%   - obj_pmmh: structure with fields meant for internal purpose only. They 
%               are used to query information on the current state of the
%               algorithm:
%               * param_names: cell with the names of the parameters to be
%                 monitored
%               * latent_names: cell with the names of the latents to be
%                 monitored
%               * sample_param: current sample value of the parameters
%               * sample_latent: current sample value of the latents
%               * sample_param_tr: current sample value of the transformed parameters
%               * log_marg_like: current value of the log marginal likelihood
%               * log_prior: log prior of the current parameters
%               * model: biips model
%               * dim: dimensions of the parameters
%               * n_iter: curent nb of iterations
%               * ar_mean: mean acceptance rate
%               * alpha: tuning parameter of the rescaling
%               * beta: tuning parameter of the proposal
%               * n_rescale: nb of iterations for rescaling
%               * n_cov: nb of iterations before starting to learn the
%               empirical covariance matrix
%               * target_prob: target acceptance probability for the
%               rescaling
%               * log_step: log random walk standard deviations for the
%               rescaling
%               * rw_mean: empirical mean of the samples
%               * rw_cov: empirical covariance matrix of the samples
%               * transform: cell of transformation function handles for each
%               parameter component
%               * transform_inv: cell of inverse transformation function handles for each
%               parameter component
%               * transform_lderiv: cell of transformation log-derivative function handles for each
%               parameter component
%
%   See also BIIPS_MODEL, BIIPS_PMMH_UPDATE, BIIPS_PMMH_SAMPLES

%--------------------------------------------------------------------------
% EXAMPLE:
% modelfile = 'hmm.bug';
% type(modelfile);
% 
% data = struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1));
% model = biips_model(modelfile, data);
% 
% n_part = 50;
% obj_pmmh = biips_pmmh_init(model, {'logtau'}, 'latent_names', {'x', 'c[2:10]'}, 'inits', {-2}); % Initialize
% [obj_pmmh, plml_pmmh_burn] = biips_pmmh_update(obj_pmmh, 100, n_part); % Burn-in
% [obj_pmmh, out_pmmh, plml_pmmh] = biips_pmmh_samples(obj_pmmh, 100, n_part, 'thin', 1); % Samples
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 21-10-2014
%--------------------------------------------------------------------------

%%% Process and check optional arguments
optarg_names = {'inits', 'rw_step', 'transform', 'n_rescale', 'beta', 'alpha', 'latent_names'};
optarg_default = {{}, [], true, 400, .05, .99, {}};
optarg_valid = {{}, [], {true, false}, [0,intmax], [0,1], [0,1],{}};
optarg_type = {'numeric', 'numeric', 'logical', 'numeric', 'numeric', 'numeric', 'char'};
[inits, rw_step, transform, n_rescale, beta, alpha, latent_names] = parsevar(varargin, optarg_names,...
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
obj_pmmh.model = clone_model(model);
console = obj_pmmh.model.id;



%% Init the parameters of the random walk
sample_param = pmmh_set_param(console, param_names, pn_param, inits);

% Parameters and latent
obj_pmmh.param_names = param_names;
obj_pmmh.latent_names = latent_names;
obj_pmmh.sample_param = sample_param;
obj_pmmh.sample_latent = [];
obj_pmmh.log_marg_like = -Inf;
obj_pmmh.log_prior = -Inf;

sample_dim = cellfun(@size, sample_param, 'UniformOutput', false);

obj_pmmh.dim = sample_dim;
obj_pmmh.n_iter = 0;
obj_pmmh.ar_mean = 0;
obj_pmmh.alpha = alpha;
obj_pmmh.beta = beta;
obj_pmmh.n_rescale = n_rescale;
obj_pmmh.n_cov = n_rescale/2; % we start learning the covariance matrix after n_cov iterations

obj_pmmh.len = sum(cellfun(@prod, sample_dim, 'UniformOutput', true));
if obj_pmmh.len==1
    obj_pmmh.target_prob = 0.44;
else
    obj_pmmh.target_prob = 0.234;
end

% Init random walk stepsize for the part with diagonal covariance matrix
if isempty(rw_step)
    rw_step = cell(n_param, 1);
    % default values
    for i=1:n_param
        rw_step{i} = 0.1/sqrt(obj_pmmh.len)*ones(sample_dim{i});
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
obj_pmmh.log_step = cell2mat(cellfun(@(x) log(x(:)), rw_step(:), 'UniformOutput', false));

% Covariance matrix
obj_pmmh.rw_mean = [];
obj_pmmh.rw_cov = [];  

%% Define variable transformations
obj_pmmh.transform = cell(obj_pmmh.len, 1);
obj_pmmh.transform_inv= cell(obj_pmmh.len, 1);
obj_pmmh.transform_lderiv = cell(obj_pmmh.len, 1);

k = 0;
for i=1:n_param
    len = prod(obj_pmmh.dim{i});
    if transform
%         try
            support = matbiips('get_fixed_support', console, pn_param(i).name, ...
                pn_param(i).lower, pn_param(i).upper);
%         catch
%             error('CANNOT GET FIXED SUPPORT OF VARIABLE %s: BUG TO BE FIXED', param_names{i})
%         end
        if size(support,1) ~= len
            error('support size does not match')
        end
    else
        support = [-Inf*ones(len,1), +Inf*ones(len,1)];
    end
    for j=1:len
        k = k+1;
        L = support(j,1);
        U = support(j,2);
        if isfinite(L)
            if isfinite(U) % lower-upper bounded support: logit transform
                obj_pmmh.transform{k} = @(x) log((x-L)/(U-x));
                obj_pmmh.transform_inv{k} = @(y) L+(U-L)/(1+exp(-y));
                obj_pmmh.transform_lderiv{k} = @(x) log(U-L)-log(x-L)-log(U-x);
            else
                if (U<0)
                    error('upper can not be -Inf')
                end
                % lower bounded support: log transform
                obj_pmmh.transform{k} = @(x) log(x-L);
                obj_pmmh.transform_inv{k} = @(y) L+exp(y);
                obj_pmmh.transform_lderiv{k} = @(x) -log(x-L);
            end
        else
            if (L>0)
                error('lower can not be +Inf')
            end
            if isfinite(U) % upper bounded support: -log transform
                obj_pmmh.transform{k} = @(x) log(U-x);
                obj_pmmh.transform_inv{k} = @(y) U-exp(y);
                obj_pmmh.transform_lderiv{k} = @(x) -log(U-x);
            else
                if (U<0)
                    error('upper can not be -Inf')
                end
                % unbounded support: identity
                obj_pmmh.transform{k} = @(x) x;
                obj_pmmh.transform_inv{k} = @(y) y;
                obj_pmmh.transform_lderiv{k} = @(x) 0;
            end
        end
    end
end

obj_pmmh.sample_param_tr = pmmh_rw_transform(sample_param, obj_pmmh);

obj_pmmh.class = 'pmmh';
