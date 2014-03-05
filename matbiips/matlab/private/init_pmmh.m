function [sample_param, sample_latent, log_prior, log_marg_like]...
    = init_pmmh(console, param_names, n_part, first_init, varargin)

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
optarg_names = {'latent_names', 'rs_thres', 'rs_type'};
optarg_default = {{}, .5, 'stratified'};
optarg_valid = {{}, [0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'}};
optarg_type = {'char', 'numeric', 'char'};
[latent_names, rs_thres, rs_type] = parsevar(varargin, optarg_names, optarg_type,...
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


% % Check inits
% if ~isempty(inits)
%     if length(inits)~=length(param_names)
%         error('variables_names and inits should be of same length')
%     end
%     sample_init_values = false;
% else
%     sample_init_values = true;
% end

% MORE TODO: R code not clear
% % Stop Biips verbosity
% verb = inter_biips('verbosity', 0);
% cleanupObj = onCleanup(@() inter_biips('verbosity', 1));% set verbosity on again when function terminates

sample_param = cell(length(param_names), 1);
% % Set init values
% if ~sample_init_values
%     for i=1:length(param_names)
%         var_name = param_names{i};
%         % Take init value in inits parameters
%         tag = inter_biips('change_data', console, pn_param(i).name, ...
%             pn_param(i).lower, pn_param(i).upper, inits{i}, true);
%         if ~tag
%             error('Data change failed: invalid initial value for variable %s', var_name);
%         end
%         sample_param{i} = inits{i};
% %         sample = setfield(sample, var, inits{i}); % PB: will not work with [] TBD
%     end
% else
    data = biips_get_data(console);    
    for i=1:length(param_names)
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
%         if isfield(data, var_name) 
            if isempty(pn_param(i).lower)
                sample_param{i} = getfield(data, var_name);
            else
                samp_full = getfield(data, var_name);
                switch(length(pn_param(i).lower))
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
%         else
%             samp = inter_biips('sample_data', console, pn_param(i).name,...
%                 pn_param(i).lower, pn_param(i).upper, inits_rng_seed);
%             sample_param{i} = samp;
%         end    
    end
% end

% Check NA
% TODO??



% log-prior density
log_prior = 0;
for i=1:length(param_names)
    log_p = inter_biips('get_log_prior_density', console, pn_param(i).name, ...
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
        monitor_biips(console, latent_names, 's'); 
        latent_monitored = false;
    end
end

% build SMC sampler
if ~inter_biips('is_sampler_built', console)
    inter_biips('build_smc_sampler', console, false)
end

% sampler_atend = inter_biips('is_smc_sampler_at_end', console);
% pause

%% Get log-normalizing constant
% if (~sampler_atend || ~latent_monitored || ~sample_init_values)
    % Run SMC
%     if (~sampler_atend || ~sample_init_values)
%         inter_biips('message', 'Initializing PMMH');
%     else
%         if (~latent_monitored)
%             inter_biips('message', 'Initializing PMMH latent variables')        
%         end
%     end

if first_init
    inter_biips('message', 'Initializing PMMH');
    ok = run_smc_forward(console, n_part, rs_thres, rs_type, get_seed());
    if (~ok)
        error('Run SMC sampler: invalid values');        
    end    
elseif ~latent_monitored
    inter_biips('message', 'Initializing PMMH - latent variables');
    ok = run_smc_forward(console, n_part, rs_thres, rs_type, get_seed());
    if (~ok)
        error('Run SMC sampler: invalid values');        
    end    
end
    
log_marg_like = inter_biips('get_log_norm_const', console);
if isnan(log_marg_like) || isinf(log_marg_like)
    error('Failed to evaluate the log marginal likelihood (Inf or NaN)');
end


%% Get latent variables sampled value
sample_latent = cell(length(latent_names), 1);
if (~isempty(latent_names))
    sampled_value = inter_biips('get_sampled_gen_tree_smooth_particle', console);
    if isempty(fieldnames(sampled_value))
        % Sample one particle
        rng_seed = get_seed();
        inter_biips('sample_gen_tree_smooth_particle', console, rng_seed);
        sampled_value = inter_biips('get_sampled_gen_tree_smooth_particle', console);
    end
    for i=1:length(latent_names)
        var_name = latent_names{i};
        sample_latent{i} = getfield(sampled_value, var_name);
    end    
end