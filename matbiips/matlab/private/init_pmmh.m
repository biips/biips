function [sample, log_prior, log_marg_like] = init_pmmh(console, param_names, n_part, varargin)

%% ADD DOC
% param_names: parameters updated by MCMC
% latent_names: parameters updated by SMC that you want to monitor
varargin
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
pn_param = cellfun(@parse_varname, param_names)
%%% Process and check optional arguments
optarg_names = {'latent_names', 'inits', 'inits_rng_seed', 'rs_thres', 'rs_type'};
optarg_default = {{}, {}, [], .5, 'stratified'};
optarg_valid = {{}, {}, get_seed(), [0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'}};
optarg_type = {'char', 'cell', 'numeric', 'numeric', 'char'};
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

inits
% pause
% Set init values
if ~sample_init_values
    for i=1:length(param_names)
        var = param_names{i};
        % Take init value in inits parameters
        fprintf('START CHANGE_DATA\n')
        tag = inter_biips('change_data', console, pn_param(i).name, ...
            pn_param(i).lower, pn_param(i).upper, inits{i}, true);
        fprintf('END CHANGE_DATA\n')        
        if ~tag
            error('Data change failed: invalid initial value for variable %s', var);
        end
    end
else
    data = biips_get_data(console);
    for i=1:length(param_names)
        data
        var = param_names{i}
        pn_param(i)
        if isfield(data, var)
            sample = setfield(sample, var, getfield(data, var));
        else
            fprintf('START SAMPLE_DATA\n')
            samp = inter_biips('sample_data', console, pn_param(i).name,...
                pn_param(i).lower, pn_param(i).upper, inits_rng_seed);
            fprintf('END SAMPLE_DATA\n')
            sample = setfield(sample, var, samp);
        end    
    end
end

% Check NA
% TODO??

% Initialize rw
% TODO: check with Adrien

% log-prior density
for i=1:length(param_names)
    log_prior = inter_biips('get_log_prior_density', console, pn_param{i}.names, ...
        pn_param{i}.lower, pn_param{i}.upper);
    if isnan(log_prior) || isinf(log_prior)
        error('Failed to evaluate log-prior density');
    end
end

latent_monitored = true;
if ~isempty(latentvariables)
    if ~is_monitored(console, latent_names, 's', false)
        % monitor variables
        monitor_biips(console, latent_names, 's');
        latent_monitored = false;
    end
end

% build SMC sampler
if ~interbiips('is_sampler_built', console)
    interbiips('build_smc_sampler', console, false)
end

sampler_atend = interbiips('is_smc_at_end', console);

%% Get log-normalizing constant
if (~sampler_atend || ~latent_monitored)
    % Run SMC
    if (~sampler_atend)
        interbiips('message', 'Initializing PMMH');
    else
        if (~latent_monitored)
            inter_biips('message', 'Initializing PMMH latent variables')        
        end
        ok = run_smc_forward(console, n_part, rs_thres, rs_type, seed);
        if (~ok)
            error('Run SMC sampler: invalid initial values');
        end
    end    
end
log_marg_like = inter_biips('get_log_norm_const', console);
if isnan(log_marg_like) || isinf(log_marg_like)
    error('Failed to evaluate the log marginal likelihood (Inf or NaN)');
end

%% Get latent variables sampled value
if (~isempty(latent_names))
    sampled_value = inter_biips('get_sampled_gen_tree_smooth_particle', console);
    if isempty(sampled_value)
        % Sample one particle
        rng_seed = get_seed();
        inter_biips('sample_gen_tree_smooth_particle', console, uint32(rng_seed));
        sampled_value = inter_biips('get_sampled_gen_tree_smooth_particle', console);
    end
    for i=1:length(latent_names)
        sample.(var) = sampled_value.(var); % NOTE: CHANGE THAT FOR OCTAVE
    end    
end