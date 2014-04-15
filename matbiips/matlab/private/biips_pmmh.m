function varargout = biips_pmmh(obj, n_iter, n_part, return_samples, varargin)

%
% BIIPS_PMMH performs iterations for the PMMH algorithm
%
%   It is recommended to use the two wrapper functions below instead:
%
%   See also  BIIPS_PMMH_UPDATE, BIIPS_PMMH_SAMPLES
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------


%% PROCESS AND CHECK INPUTS
optarg_names = {'thin', 'max_fail', 'rw_learn',...
    'rs_thres', 'rs_type'};
optarg_default = {1, 0, false, .5, 'stratified'};
optarg_valid = {[0, n_part], [0, intmax], {true, false},...
    [0, n_part], {'multinomial', 'stratified', 'residual', 'systematic'}};
optarg_type = {'numeric', 'numeric', 'logical', 'numeric', 'char'};
[thin, max_fail, rw_learn, rs_thres, rs_type] = parsevar(varargin, optarg_names,...
    optarg_type, optarg_valid, optarg_default);

%% Stops biips verbosity
old_verb = inter_biips('verbosity', 0);
cleanupObj = onCleanup(@() inter_biips('verbosity', old_verb));% reset verbosity when function terminates


%% Create a clone console
model2 = clone_model(obj.model);
console = model2.id;
if (~inter_biips('is_sampler_built', console))
   inter_biips('build_smc_sampler', console, false);
end

param_names = obj.param_names;
latent_names = obj.latent_names;
n_param = length(param_names);
n_latent = length(latent_names);

% Get values of current iteration from PMMH object
sample_param = obj.param_val;
sample_latent = obj.latent_val;
log_prior = obj.log_prior;
log_marg_like = obj.log_marg_like;

if obj.niter<obj.n_rescale
    rw_rescale = true;
else
    rw_rescale = false;    
end


% % Initialize
% [sample_param, sample_latent, log_prior, log_marg_like] =...
%     pmmh_init(console, param_names, n_part, (obj.niter==0),  'latent_names', latent_names,...
%     'rs_thres', rs_thres, 'rs_type', rs_type);
pn_param =  cellfun(@parse_varname, param_names);
pmmh_get_param(console, pn_param, sample_param, true);

if n_latent>0
    monitor_biips(console, latent_names, 's'); 
end

    
% Initialize counters
n_fail = 0;
n_accept = 0;

% Set output
n_samples = ceil((n_iter)/thin);
accept_rate = zeros(n_samples, 1);
step_rw = zeros(n_samples, length(obj.lstep));
log_marg_like_st = zeros(n_samples, 1);
log_post_st = zeros(n_samples, 1);
if return_samples
    n_dim_param = zeros(n_param, 1);
    for k=1:length(param_names)
        n_dim_param(k) = ndims(sample_param{k});
        samples_param_st{k} = zeros([size(sample_param{k}), n_samples]);    
    end
    n_dim_latent = zeros(n_latent, 1);
    if isempty(sample_latent)% in case one does not call PMMH_update before
        error('Please call biips_pmmh_update before biips_pmmh_samples')
        % TODO: if empty, then need to change the data with current value
        % and sample a particle
    end
    for k=1:length(latent_names)
        n_dim_latent(k) = ndims(sample_latent{k});
        samples_latent_st{k} = zeros([size(sample_latent{k}), n_samples]);    
    end
end

% display message and progress bar
if ~return_samples
    if rw_learn 
        inter_biips('message', ['Adapting PMMH with ', num2str(n_part) ' particles']);   
        bar = inter_biips('make_progress_bar', n_iter, '+', 'iterations');
    else
        inter_biips('message', ['Updating PMMH with ', num2str(n_part) ' particles']);
        bar = inter_biips('make_progress_bar', n_iter, '*', 'iterations');
    end
else
    inter_biips('message', ['Generating ' num2str(n_samples) ' PMMH samples with ', num2str(n_part) ' particles']);
    bar = inter_biips('make_progress_bar', n_iter, '*', 'iterations');
end


% PMMH iterations
for i=1:n_iter
    % MH step
    [sample_param, sample_latent, log_prior, log_marg_like, ...
    accept_rate_step, accepted, n_fail_step, obj] = pmmh_one_update(console, param_names, pn_param, sample_param,...
        sample_latent, latent_names, log_prior, log_marg_like,  n_part, rs_thres,...
        rs_type, obj, rw_rescale, rw_learn);

    % Print progress bar
    inter_biips('advance_progress_bar', bar, 1);
    
    n_fail = n_fail + n_fail_step;
    n_accept = n_accept + accepted;
    accept_rate(i) = accept_rate_step;
    step_rw(i,:) = exp(obj.lstep);
    log_marg_like_st(i) = log_marg_like;
    log_post_st(i) = log_marg_like + log_prior;
    
    % Check nb of failures FC: MODIFY THIS EVENTUALLY
    if (n_fail>max_fail)
        error('Number of failure exceeds max_fails: %d failures', n_fail)
    end
    
    % Stop rescale
    if (rw_rescale && (i==obj.n_rescale))
        rw_rescale = false;  
    end  
        
    % Store output
    if mod(i-1, thin)==0
        ind_sample = (i-1)/thin + 1;
        log_marg_like_st(ind_sample) = log_marg_like;
        log_post_st(ind_sample) = log_marg_like + log_prior;
        if return_samples
            for k=1:n_param
                switch(n_dim_param(k))
                    case 1
                        samples_param_st{k}(:, ind_sample) = sample_param{k};
                    case 2
                        samples_param_st{k}(:, :, ind_sample) = sample_param{k};
                    case 3
                        samples_param_st{k}(:, :, :, ind_sample) = sample_param{k};                    
                    otherwise
                        error(['Variable ' param_names{k} 'of dimension >3'])                    
                end
            end
            for k=1:n_latent
               switch(n_dim_latent(k))
                    case 1
                        samples_latent_st{k}(:, ind_sample) = sample_latent{k};
                    case 2
                        samples_latent_st{k}(:, :, ind_sample) = sample_latent{k};
                    case 3
                        samples_latent_st{k}(:, :, :, ind_sample) = sample_latent{k};                    
                    otherwise
                        error(['Variable ' latent_names{k} 'of dimension >3'])                    
                end
            end
        end
    end
    
    
end

%% Delete clone console
inter_biips('clear_console', console); 

% if ~isempty(latent_names)
%     clear_monitors(console, 's')
% end


% % Reset log-norm constant and sampled values if not accepted to store the
% % last of the loglikelihood in the biips model
% if (n_iter>0 && ~accepted)
%     for i=1:n_param
%         var = param_names{i};
%         tag = inter_biips('change_data', console, pn_param(i).name, ...
%             pn_param(i).lower, pn_param(i).upper, sample_param{i}, true);
%         if ~tag
%             error('Cannot reset previous data: %s=%.f', var, sample_param{i});
%         end        
%     end
%     inter_biips('set_log_norm_const', console, log_marg_like)
% end

obj.param_val = sample_param;
obj.latent_val = sample_latent;
obj.log_prior = log_prior;
obj.log_marg_like = log_marg_like;

if return_samples
    %% Set output structure
    for k=1:n_param % Remove singleton dimensions for vectors
        samples_all{k} = squeeze(samples_param_st{k});
        if size(samples_all{k}, ndims(samples_all{k}))==1 % needed because weird behavior of squeeze with [1,1,n]
            samples_all{k} = samples_all{k}';
        end
        variable_names{k} = param_names{k};
    end
    for k=1:n_latent % Remove singleton dimensions for vectors
        samples_all{k+length(param_names)} = squeeze(samples_latent_st{k});
        if size(samples_all{k+length(param_names)}, ndims(samples_all{k+length(param_names)}))==1 % needed because weird behavior of squeeze with [1,1,n]
            samples_all{k+n_param} = samples_all{k+n_param}';
        end
        variable_names{k+n_param} = latent_names{k};
    end
    varargout{1} = obj;
    varargout{2} = cell2struct_weaknames(samples_all, variable_names);
    varargout{3} = log_post_st;
    varargout{4} = log_marg_like_st;    
    out.accept_rate = accept_rate;
    out.n_fail = n_fail;
    out.step_rw = step_rw;
    varargout{5} = out;
else    
    out.accept_rate = accept_rate;
    out.n_fail = n_fail;
    out.step_rw = step_rw;
    out.log_marg_like = log_marg_like_st;
    out.log_post = log_post_st;
    varargout{1} = obj;
    varargout{2} = out;
end