function [out, log_marg_like_st] = biips_pimh_samples(console, variable_names, n_iter, n_part, varargin)

%--------------------------------------------------------------------------
% BIIPS_PIMH_SAMPLES Performs iterations for the particle
% independent Metropolis-Hastings algorithm and returns samples
% [out, log_marg_like_st] = biips_pimh_samples(console,...
%                           variable_names, n_iter, n_part, varargin)
%
%   INPUT 
%   - console:      integer. Id of the console containing the model, 
%                   returned by the 'biips_model' function
%   - variable_names: cell of strings. Contains the names of the 
%                   unobserved variables to monitor.
%                   Examples: {'var1', 'var2[1]', 'var3[1:10]',
%                                                       'var4[1, 5:10, 3]'}
%                   Dimensions and indices must be a valid subset of 
%                   the variables of the model.
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
%   - out:          Structure with the PIMH samples for each variable
%   - log_marg_like_st: vector with log marginal likelihood over iterations
%
%   See also BIIPS_MODEL, BIIPS_PIMH_UPDATE
%--------------------------------------------------------------------------
% EXAMPLE:
% data = struct('var1', 0, 'var2', 1.2);
% model_id = biips_model('model.bug', data)
% npart = 100; variables = {'x'}; 
% thin = 2;
% out_smc = biips_pimh_samples(model_id, variables, npart, 'thin', thin);
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, François Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

%% PROCESS AND CHECK INPUTS
optarg_names = {'rs_thres', 'rs_type', 'thin'};
optarg_default = {.5, 'stratified', 1};
optarg_valid = {[0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'},[1, n_iter]};
optarg_type = {'numeric', 'char', 'numeric'};
[rs_thres, rs_type, thin] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

%% Stops biips verbosity
inter_biips('verbosity', 0);
cleanupObj = onCleanup(@() inter_biips('verbosity', 1));% set verbosity on again when function terminates

%% Initialization
[sample, log_marg_like] = pimh_init(console, variable_names, n_part, rs_thres, rs_type);
inter_biips('message', ['Generating PIMH samples with ' num2str(n_part)...
    ' particles and ' num2str(n_iter) ' iterations']);
bar = inter_biips('make_progress_bar', n_iter, '*', 'iterations');

% Output structure with MCMC samples
n_samples = ceil((n_iter)/thin);
n_dim = zeros(length(variable_names), 1);
for k=1:length(variable_names)
    n_dim(k) = ndims(getfield(sample, variable_names{k}));
    samples_st{k} = zeros([size(getfield(sample, variable_names{k})), n_samples]);    
end
log_marg_like_st = zeros(n_samples, 1);


% Independent Metropolis-Hastings iterations
for i=1:n_iter
    [sample, log_marg_like, accepted] = pimh_one_update(console, ...
        variable_names, n_part, rs_thres, rs_type, sample, log_marg_like);
    
    % Store output
    if mod(i-1, thin)==0
        ind_sample = (i-1)/thin + 1;
        log_marg_like_st(ind_sample) = log_marg_like;
        for k=1:length(variable_names)
            switch(n_dim(k))
                case 1
                    samples_st{k}(:, ind_sample) = getfield(sample, variable_names{k});
                case 2
                    samples_st{k}(:, :, ind_sample) = getfield(sample, variable_names{k});
                case 3
                    samples_st{k}(:, :, :, ind_sample) = getfield(sample, variable_names{k});                    
                otherwise
                    error(['Variable ' variable_names{k} 'of dimension >3'])                    
            end
        end
    end
    % Progress bar
    inter_biips('advance_progress_bar', bar, 1);    
end

% Release monitor memory
clear_monitors(console, 's', true);

% Reset lognormalizing constant and sampled value
if (n_iter>0 && ~accepted)
    inter_biips('set_log_norm_const', console, log_marg_like);
    inter_biips('set_sampled_gen_tree_smooth_particle', console, sample);        
end

%% Set output structure
for k=1:length(variable_names) % Remove singleton dimensions for vectors
    samples_st{k} = squeeze(samples_st{k});
    if size(samples_st{k}, ndims(samples_st{k}))==1 % needed because weird behavior of squeeze with [1,1,n]
            samples_st{k} = samples_st{k}';
    end
end
out = cell2struct_weaknames(samples_st, variable_names);