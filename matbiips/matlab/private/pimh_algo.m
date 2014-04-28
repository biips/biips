function [obj, samples_st, log_marg_like_st] = pimh_algo(obj, n_iter, n_part, varargin)

%
% PIMH_ALGO performs iterations for the PIMH algorithm
% [obj, samples_st, log_marg_like_st] = pimh_algo(obj,...
%                           n_iter, n_part, varargin)
%
%   INPUT
%   - obj:     structure. PIMH object
%   - n_iter:       positive integer. Number of iterations
%   - n_part:       positive integer. Number of particles used in SMC algorithms
%   Optional Inputs:
%   - thin :        positive integer. Returns samples every thin iterations
%                   (default=1)
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
%   - obj:     structure. PIMH object modified
%   Optional Outputs:
%   - samples_st:       Structure with the PIMH samples for each variable
%   - log_marg_like_st: vector with log marginal likelihood over iterations
%
%   See also BIIPS_MODEL, BIIPS_PIMH_INIT, BIIPS_PIMH_UPDATE, BIIPS_PIMH_SAMPLES
%--------------------------------------------------------------------------
% EXAMPLE:
% data = struct('var1', 0, 'var2', 1.2);
% model = biips_model('model.bug', data)
% variables = {'x'};
% nburn = 1000; niter = 1000; npart = 100;
% obj_pimh = biips_pimh_init(model, variables); %Initialize
% obj_pimh = biips_pimh_update(obj_pimh, nburn, npart); % Burn-in
% [obj_pimh, samples_pimh] = biips_pimh_samples(obj_pimh, niter, npart); % Samples
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

%% PROCESS AND CHECK INPUTS
optarg_names = {'thin', 'rs_thres', 'rs_type'};
optarg_default = {1, .5, 'stratified'};
optarg_valid = {[1, n_iter], [0, n_part],...
    {'stratified', 'systematic', 'residual', 'multinomial'}};
optarg_type = {'numeric', 'numeric', 'char'};
[thin, rs_thres, rs_type] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

check_struct_model(obj.model);
%%% TODO check pimh_obj structure

%% Stops biips verbosity
verb = matbiips('verbosity', 0);
cleanupObj = onCleanup(@() matbiips('verbosity', verb));% set verbosity on again when function terminates


%% Initialization
console = obj.model.id;

% monitor variables
variable_names = obj.variable_names;
monitor(console, variable_names, 's');

% build smc sampler
if (~matbiips('is_sampler_built', console))
    matbiips('build_smc_sampler', console, false);
end

% Get sample and log likelihood from PIMH object
sample = obj.sample;
log_marg_like = obj.log_marg_like;

% Output structure with MCMC samples
n_var = length(variable_names);
    
if nargout>=2
    n_samples = ceil(n_iter/thin);
    samples_st = cell(n_var, 1);
    if nargout>=3
        log_marg_like_st = zeros(n_samples, 1);
    end
    ind_sample = 0;
end

% display message and progress bar
if nargout>=2
    mess = 'Generating PIMH samples with ';
else
    mess = 'Updating PIMH with ';
end
matbiips('message', [mess, num2str(n_part), ...
    ' particles and ', num2str(n_iter), ' iterations']);
bar = matbiips('make_progress_bar', n_iter, '*', 'iterations');
%%% TODO: display expected time of run

%% Independent Metropolis-Hastings iterations
for i=1:n_iter
    
    % SMC
    smc_forward_algo(console, n_part, rs_thres, rs_type);
    
    % Acceptance rate
    log_marg_like_prop = matbiips('get_log_norm_const', console);
    log_ar = log_marg_like_prop - log_marg_like;
    
    % Metropolis-Hastings step
    if rand<exp(log_ar)
        log_marg_like = log_marg_like_prop;
        
        % Sample one particle
        sampled_value = matbiips('sample_gen_tree_smooth_particle', console, get_seed());
        
        sample = cell(n_var, 1);
        for k=1:n_var
            %%% FIXME transfrom variable name. eg: x[1,] => x[1,1:100]
            var = strjoin(strsplit(variable_names{k}, ' '), '');
            sample{k} = getfield(sampled_value, var);
        end
    end
    
    % Store output
    if nargout>=2 && mod(i-1, thin)==0
        ind_sample = ind_sample + 1;
        
        if ind_sample==1
            % pre-allocation here to be sure that sample is not empty
            for k=1:n_var
                samples_st{k} = zeros([size(sample{k}), n_samples]);
            end
        end
        
        for k=1:n_var
            len = numel(sample{k});
            from = (ind_sample-1)*len+1;
            to = (ind_sample-1)*len+len;
            samples_st{k}(from:to) = sample{k};
        end
        
        if nargout >=3
            log_marg_like_st(ind_sample) = log_marg_like;
        end
    end

    % Progress bar
    matbiips('advance_progress_bar', bar, 1);
end

% Release monitor memory
clear_monitors(console, 's', true);


%% Output PIMH object with current sample and log marginal likelihood
obj.sample = sample;
obj.log_marg_like = log_marg_like;

%% Set output structure
if nargout>=2
    for k=1:n_var % Remove singleton dimensions for vectors
        samples_st{k} = squeeze(samples_st{k});
        if size(samples_st{k}, ndims(samples_st{k}))==1 % needed because weird behavior of squeeze with [1,1,n]
            samples_st{k} = samples_st{k}';
        end
    end
    samples_st = cell2struct_weaknames(samples_st, variable_names);
end
