function [sample_param, sample_latent, log_prior, log_marg_like, pmmh_rw]...
    = init_pmmh(console, param_names, n_part, varargin)

%% ADD DOC
% param_names: parameters updated by MCMC
% latent_names: parameters updated by SMC that you want to monitor
% varargin
%% PROCESS AND CHECK INPUTS
% Check param_names
for i=1:length(param_names)
    if ~ischar(param_names{i})
        warning('Invalid parameter name')
        param_names(i) = [];
    end
end
% Remove duplicate entries
param_names = unique(param_names);
pn_param = cellfun(@parse_varname, param_names);
%%% Process and check optional arguments
optarg_names = {'latent_names', 'inits', 'inits_rng_seed', 'rs_thres', 'rs_type'};
optarg_default = {{}, {}, get_seed(), .5, 'stratified'};
optarg_valid = {{}, {}, get_seed(), [0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'}};
optarg_type = {'char', 'numeric', 'numeric', 'numeric', 'char'};
[latent_names, inits, inits_rng_seed, rs_thres, rs_type] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);
% Check latent_names
for i=1:length(latent_names)
    if ~ischar(latent_names{i})
        warning('Invalid latent name')
        latent_names(i) = [];
    end
end
% Remove duplicate entries
latent_names = unique(latent_names);


% Check inits
if ~isempty(inits)
    if length(inits)~=length(param_names)
        error('variables_names and inits should be of same length')
    end
    sample_init_values = false;
else
    sample_init_values = true;
end
% MORE TODO: R code not clear

% Stop Biips verbosity
verb = inter_biips('verbosity', 0);
cleanupObj = onCleanup(@() inter_biips('verbosity', 1));% set verbosity on again when function terminates

% inits
% pn_param(1).name
% iscell(pn_param(1).name)
% iscell(pn_param(1).lower)

sample_param = cell(length(param_names), 1);
% Set init values
if ~sample_init_values
    for i=1:length(param_names)
        var = param_names{i};
        % Take init value in inits parameters
        tag = inter_biips('change_data', console, pn_param(i).name, ...
            pn_param(i).lower, pn_param(i).upper, inits{i}, true);
        if ~tag
            error('Data change failed: invalid initial value for variable %s', var);
        end
        sample_param{i} = inits{i};
%         sample = setfield(sample, var, inits{i}); % PB: will not work with [] TBD
    end
else
    data = biips_get_data(console);
    for i=1:length(param_names)
        var = param_names{i};
        if isfield(data, var)
%             sample = setfield(sample, var, getfield(data, var));
            sample_param{i} = getfield(data, var);
        else
            samp = inter_biips('sample_data', console, pn_param(i).name,...
                pn_param(i).lower, pn_param(i).upper, inits_rng_seed);
            sample_param{i} = samp;
%             sample = setfield(sample, var, samp);
        end    
    end
end

sample_param

% Check NA
% TODO??

% Initialize rw
pmmh_rw = pmmh_rw_init(sample_param);

% log-prior density
log_prior = 0;
for i=1:length(param_names)
    log_p = inter_biips('get_log_prior_density', console, pn_param(i).name, ...
        pn_param(i).lower, pn_param(i).upper);
    if isnan(log_p) || isinf(log_p)
        error('Failed to evaluate log-prior density');
    end
    log_prior = log_prior + log_p;
end

latent_monitored = true;
if ~isempty(latent_names)
    monitored = is_monitored(console, latent_names, 's', false);
    if ~monitored
        % monitor variables
        monitor_biips(console, latent_names, 's'); 
        latent_monitored = false
    end
end

% build SMC sampler
if ~inter_biips('is_sampler_built', console)
    inter_biips('build_smc_sampler', console, false)
end

sampler_atend = inter_biips('is_smc_sampler_at_end', console);
% pause

%% Get log-normalizing constant
if (~sampler_atend || ~latent_monitored)
    % Run SMC
    if (~sampler_atend)
        inter_biips('message', 'Initializing PMMH');
    else
        if (~latent_monitored)
            inter_biips('message', 'Initializing PMMH latent variables')        
        end
    end
    ok = run_smc_forward(console, n_part, rs_thres, rs_type, inits_rng_seed);
    if (~ok)
        error('Run SMC sampler: invalid initial values');
    end    
end
log_marg_like = inter_biips('get_log_norm_const', console);
if isnan(log_marg_like) || isinf(log_marg_like)
    error('Failed to evaluate the log marginal likelihood (Inf or NaN)');
end

%% Get latent variables sampled value
sample_latent = cell(length(latent_names), 1);
if (~isempty(latent_names))
    sampled_value = inter_biips('get_sampled_gen_tree_smooth_particle', console)
    if isempty(fieldnames(sampled_value))
        % Sample one particle
        rng_seed = get_seed();
        inter_biips('sample_gen_tree_smooth_particle', console, uint32(rng_seed));
        sampled_value = inter_biips('get_sampled_gen_tree_smooth_particle', console)
    end
    for i=1:length(latent_names)
        var = latent_names{i};
        sample_latent{i} = getfield(sampled_value, var);
%         sample.(var) = sampled_value.(var); % NOTE: CHANGE THAT FOR OCTAVE
    end    
end


