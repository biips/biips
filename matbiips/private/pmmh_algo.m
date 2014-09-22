function [obj, varargout] = pmmh_algo(obj, n_iter, n_part, return_samples, varargin)

%
% PMMH_ALGO performs iterations for the PMMH algorithm
%
%   It is recommended to use the two wrapper functions below instead:
%
%   See also  BIIPS_PMMH_UPDATE, BIIPS_PMMH_SAMPLES
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------


%% PROCESS AND CHECK INPUTS
optarg_names = {'thin', 'max_fail', 'rw_adapt',...
    'rs_thres', 'rs_type'};
optarg_default = {1, 0, false, .5, 'stratified'};
optarg_valid = {[1, n_iter], [0, intmax], {true, false},...
    [0, n_part], {'stratified', 'systematic', 'residual', 'multinomial'}};
optarg_type = {'numeric', 'numeric', 'logical', 'numeric', 'char'};
[thin, max_fail, rw_adapt, rs_thres, rs_type] = parsevar(varargin, optarg_names,...
    optarg_type, optarg_valid, optarg_default);

check_struct(obj, 'pmmh');

%% Stops biips verbosity
verb = matbiips('verbosity', 0);
cleanupObj = onCleanup(@() matbiips('verbosity', verb));% reset verbosity when function terminates


%% Initialization
console = obj.model.id;

% monitor variables
param_names = obj.param_names;
latent_names = obj.latent_names;
n_param = numel(param_names);
n_latent = numel(latent_names);

if n_latent>0
    monitor(console, latent_names, 's');
end

% build smc sampler
if (~matbiips('is_sampler_built', console))
    matbiips('build_smc_sampler', console, false);
end

% toggle rescaling adaptation
rw_rescale = rw_adapt && obj.n_iter<obj.n_rescale;

% set current param value to the model
sample_param = obj.sample_param;
pn_param = cellfun(@parse_varname, param_names);
pmmh_set_param(console, param_names, pn_param, sample_param);

% Initialize counters
n_samples = floor(n_iter/thin);
ind_sample = 0;
n_fail = 0;

% Output structure with MCMC samples
accept_rate = zeros(1, n_samples);
rw_step = zeros(numel(obj.log_step), n_samples);
log_marg_like_st = zeros(1, n_samples);
log_marg_like_pen_st = zeros(1, n_samples);

if return_samples
    samples_st = cell(n_param+n_latent, 1);
    
    for k=1:n_param
        samples_st{k} = zeros([size(sample_param{k}), n_samples]);
    end
end

% display message and progress bar
if ~return_samples
    if rw_adapt
        matbiips('message', ['Adapting PMMH with ', num2str(n_part) ' particles']);
        bar = matbiips('make_progress_bar', n_iter, '+', 'iterations');
    else
        matbiips('message', ['Updating PMMH with ', num2str(n_part) ' particles']);
        bar = matbiips('make_progress_bar', n_iter, '*', 'iterations');
    end
else
    matbiips('message', ['Generating ' num2str(n_samples) ' PMMH samples with ', num2str(n_part) ' particles']);
    bar = matbiips('make_progress_bar', n_iter, '*', 'iterations');
end


% PMMH iterations
for i=1:n_iter
    % MH step
    [obj, accept_rate_step, n_fail_step] = pmmh_one_update(obj, pn_param, ...
        n_part, rs_thres, rs_type, rw_rescale, rw_adapt);
    
    n_fail = n_fail + n_fail_step;
    
    % Check nb of failures FC: MODIFY THIS EVENTUALLY
    if (n_fail>max_fail)
        error('Number of failure exceeds max_fails: %d failures', n_fail)
    end
    
    % Stop rescale
    if rw_rescale && (obj.n_iter>=obj.n_rescale)
        rw_rescale = false;
    end
    
    % Store output
    if mod(i, thin)==0
        ind_sample = ind_sample + 1;
        
        log_prior = obj.log_prior;
        log_marg_like = obj.log_marg_like;
        
        accept_rate(ind_sample) = accept_rate_step;
        rw_step(:, ind_sample) = exp(obj.log_step(:));
        log_marg_like_st(ind_sample) = log_marg_like;
        log_marg_like_pen_st(ind_sample) = log_marg_like + log_prior;
        
        if return_samples
            
            sample_param = obj.sample_param;
            sample_latent = obj.sample_latent;
            
            for k=1:n_param
                len = numel(sample_param{k});
                from = (ind_sample-1)*len+1;
                to = (ind_sample-1)*len+len;
                samples_st{k}(from:to) = sample_param{k};
            end
            
            if ind_sample==1
                % pre-allocation here to be sure that sample_latent is not empty
                for k=1:n_latent
                    samples_st{n_param+k} = zeros([size(sample_latent{k}), n_samples]);
                end
            end
            
            for k=1:n_latent
                len = numel(sample_latent{k});
                from = (ind_sample-1)*len+1;
                to = (ind_sample-1)*len+len;
                samples_st{n_param+k}(from:to) = sample_latent{k};
            end
        end
    end
    
    % Print progress bar
    matbiips('advance_progress_bar', bar, 1);
end


%% Set output structure
nout = nargout-1;
if return_samples
    if nout>=1
        n_var = n_param+n_latent;
        for k=1:n_var % Remove singleton dimensions for vectors
            samples_st{k} = squeeze(samples_st{k});
            if size(samples_st{k}, ndims(samples_st{k}))==1 % needed because weird behavior of squeeze with [1,1,n]
                samples_st{k} = samples_st{k}';
            end
        end
        variable_names = [param_names latent_names];
        samples_st = cell2struct_weaknames(samples_st, variable_names);
        varargout{1} = samples_st;
    end
    if nout>=2
        varargout{2} = log_marg_like_pen_st;
    end
    if nout>=3
        varargout{3} = log_marg_like_st;
    end
    if nout>=4
        out.accept_rate = accept_rate;
        out.n_fail = n_fail;
        out.rw_step = rw_step;
        
        varargout{4} = out;
    end
else
    if nout>=1
        varargout{1} = log_marg_like_pen_st;
    end
    if nout>=2
        varargout{2} = log_marg_like_st;
    end
    if nout>=3
        out.accept_rate = accept_rate;
        out.n_fail = n_fail;
        out.rw_step = rw_step;
        
        varargout{3} = out;
    end
end
