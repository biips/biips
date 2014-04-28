function [sample_param, sample_latent, log_prior, log_marg_like]...
    = pmmh_init(console, param_names, n_part, first_init, varargin)

%--------------------------------------------------------------------------
% PMMH_INIT initializes the PMMH algorithm
% [sample_param, sample_latent, log_prior, log_marg_like]...
%     = pmmh_init(console, param_names, n_part, first_init, varargin)
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------


%% PROCESS AND CHECK INPUTS
for i=1:numel(param_names)
    if ~ischar(param_names{i})
        warning('Invalid parameter name')
        param_names(i) = [];
    end
end
% % Remove duplicate entries
pn_param = cellfun(@parse_varname, param_names);
%%% Process and check optional arguments
optarg_names = {'latent_names', 'rs_thres', 'rs_type'};
optarg_default = {{}, .5, 'stratified'};
optarg_valid = {{}, [0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'}};
optarg_type = {'char', 'numeric', 'char'};
[latent_names, rs_thres, rs_type] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);
% Check latent_names
for i=1:numel(latent_names)
    if ~ischar(latent_names{i})
        warning('Invalid latent name')
        latent_names(i) = [];
    end
end
sample_param = cell(numel(param_names), 1);
%% Set init values
data = matbiips('get_data', console);
for i=1:numel(param_names)
    clear var_name;
    if isempty(pn_param(i).lower)
        var_name = param_names{i};
    else % Get the name of the variable without bracketts
        k=1;
        while ~strcmp(param_names{i}(k),'[')
            var_name(k) = param_names{i}(k);
            k=k+1;
        end  

    end    
    if isempty(pn_param(i).lower)
        sample_param{i} = getfield(data, var_name);
    else
        samp_full = getfield(data, var_name);
        switch(numel(pn_param(i).lower))
            case 1
                sample_param{i} = samp_full(pn_param(i).lower(1):pn_param(i).upper(1));
            case 2
                sample_param{i} = samp_full(pn_param(i).lower(1):pn_param(i).upper(1),...
                    pn_param(i).lower(2):pn_param(i).upper(2));
            case 3
                sample_param{i} = samp_full(pn_param(i).lower(1):pn_param(i).upper(1),...
                    pn_param(i).lower(2):pn_param(i).upper(2),...
                    pn_param(i).lower(3):pn_param(i).upper(3));
        end
    end
end

% log-prior density
log_prior = 0;
for i=1:numel(param_names)
    log_p = matbiips('get_log_prior_density', console, pn_param(i).name, ...
        pn_param(i).lower, pn_param(i).upper);
    if isnan(log_p) || log_p==Inf
        error('Failed to evaluate log-prior density');
    end
    log_prior = log_prior + log_p;
end

latent_monitored = true;
if ~isempty(latent_names)
    monitored = is_monitored(console, latent_names, 's', false);
    if ~monitored
        % monitor variables
        monitor(console, latent_names, 's'); 
        latent_monitored = false;
    end
end

% build SMC sampler
if ~matbiips('is_sampler_built', console)
    matbiips('build_smc_sampler', console, false)
end


if first_init
    matbiips('message', 'Initializing PMMH');   
    ok = run_smc_forward(console, n_part, rs_thres, rs_type, get_seed());
    if (~ok)
        error('Run SMC sampler: invalid values');        
    end    
elseif ~latent_monitored
    matbiips('message', 'Initializing PMMH - latent variables');
    ok = run_smc_forward(console, n_part, rs_thres, rs_type, get_seed());
    if (~ok)
        error('Run SMC sampler: invalid values');        
    end    
end
    
log_marg_like = matbiips('get_log_norm_const', console);
if isnan(log_marg_like) || isinf(log_marg_like)
    error('Failed to evaluate the log marginal likelihood (Inf or NaN)');
end


%% Get latent variables sampled value
sample_latent = cell(numel(latent_names), 1);
if (~isempty(latent_names))
    sampled_value = matbiips('get_sampled_gen_tree_smooth_particle', console);
    if isempty(fieldnames(sampled_value))
        % Sample one particle
        rng_seed = get_seed();
        matbiips('sample_gen_tree_smooth_particle', console, rng_seed);
        sampled_value = matbiips('get_sampled_gen_tree_smooth_particle', console);
    end
    for i=1:numel(latent_names)
        var_name = latent_names{i};
        sample_latent{i} = getfield(sampled_value, var_name);
    end    
end