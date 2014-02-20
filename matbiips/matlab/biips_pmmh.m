function varargout = biips_pmmh(obj, n_iter, n_part, return_samples, varargin)

%% TODO: DOC


%% NOTE: For the time being, optional arg rw_step needs to be a cell with the same number of arguments as param_names
% TODO: change this to allow for numeric (and then duplicate values) - need to update sparsevar)

%% PROCESS AND CHECK INPUTS
% TODO: add some checks on nonoptional inputs

%%% Process and check optional arguments
optarg_names = {'thin', 'latent_names', 'max_fail', 'rw_learn',...
    'rs_thres', 'rs_type'};
optarg_default = {1, {}, 0, false, .5, 'stratified'};
optarg_valid = {[0, n_part], {}, [0, intmax], {true, false},...
    [0, n_part], {'multinomial', 'stratified', 'residual', 'systematic'}};
optarg_type = {'numeric', 'char', 'numeric', 'logical', 'numeric', 'char'};
[thin, latent_names, max_fail, rw_learn, rs_thres, rs_type] = parsevar(varargin, optarg_names,...
    optarg_type, optarg_valid, optarg_default);

console = obj.console;
param_names = obj.param_names;
n_param = length(param_names);

if obj.niter<obj.n_rescale
    rw_rescale = true;
else
    rw_rescale = false;    
end

% % Check rw_step
% rw_step_values = {};
% if ~isempty(rw_step)    
%     if isnumeric(rw_step)
%         for i=1:n_param
%             rw_step_values{i} = rw_step; 
%         end
%     elseif iscell(rw_step)
%         if length(rw_step) ~= length(param_names)
%             warning('Dimension of rw_step does not match the dimension of param_names - skipped argument');
%         else
%             rw_step_values = rw_step;
%         end
%     end
% end



%% Stops biips verbosity
inter_biips('verbosity', 0);
cleanupObj = onCleanup(@() inter_biips('verbosity', 1));% set verbosity on again when function terminates

% Initialize
[sample_param, sample_latent, log_prior, log_marg_like] =...
    init_pmmh(console, param_names, n_part, (obj.niter==0),  'latent_names', latent_names,...
    'rs_thres', rs_thres, 'rs_type', rs_type);
pn_param =  cellfun(@parse_varname, param_names);

% if ~return_samples || isempty(fieldnames(rw)) % if update of if the details of the rw proposal have not been provided
%     % Initialize rw
%     rw = pmmh_rw_init(sample_param);
%     % Update rw structure
%     if ~isempty(rw_step_values)
%         rw = pmmh_rw_step(rw, rw_step_values);
%     end    
%     rw.rescale = rw_rescale;
%     rw.learn = rw_learn;
%     rw.rescale_type = rw_rescale_type;  
% else
% %     % Update rw structure
% %     if ~isempty(rw_step_values)
% %         rw = pmmh_rw_step(rw, rw_step_values);
% %     end
% %     rw.cov = rw_cov;
%     rw.rescale = false;
%     rw.learn = false;
% end
    

% display message and progress bar
% is_adapt = 1;
if ~return_samples
    if rw_learn % CHECK IF THIS IS CORRECT
        inter_biips('message', ['Adapting PMMH with ', num2str(n_part) ' particles']);   
        bar = inter_biips('make_progress_bar', n_iter, '+', 'iterations');
    else
        inter_biips('message', ['Updating PMMH with ', num2str(n_part) ' particles']);
        bar = inter_biips('make_progress_bar', n_iter, '*', 'iterations');
    end
else
    inter_biips('message', ['Generating ' num2str(n_iter) ' PMMH samples with ', num2str(n_part) ' particles']);
    bar = inter_biips('make_progress_bar', n_iter, '*', 'iterations');
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
    n_dim_param = zeros(length(param_names), 1);
    for k=1:length(param_names)
        n_dim_param(k) = ndims(sample_param{k});
        samples_param_st{k} = zeros([size(sample_param{k}), n_samples]);    
    end
    n_dim_latent = zeros(length(latent_names), 1);
    for k=1:length(latent_names)
        n_dim_latent(k) = ndims(sample_latent{k});
        samples_latent_st{k} = zeros([size(sample_latent{k}), n_samples]);    
    end
end


% PMMH iterations
for i=1:n_iter
    % MH step
    [sample_param, sample_latent, log_prior, log_marg_like, ...
    accept_rate_step, accepted, n_fail_step, obj] = one_update_pmmh(console, param_names, pn_param, sample_param,...
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
    
%     rw.cov
    
    % Store output
    if mod(i-1, thin)==0
        ind_sample = (i-1)/thin + 1;
        log_marg_like_st(ind_sample) = log_marg_like;
        log_post_st(ind_sample) = log_marg_like + log_prior;
        if return_samples
            for k=1:length(param_names)
                switch(n_dim_param(k)) % !!!FC: NOT VERY ELEGANT PIECE OF CODE!!!
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
            for k=1:length(latent_names)
                switch(n_dim_latent(k)) % !!!FC: NOT VERY ELEGANT PIECE OF CODE!!!
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

if ~isempty(latent_names)
    clear_monitors(console, 's')
end


% Reset log-norm constant and sampled values if not accepted to store the
% last of the loglikelihood in the biips model
if (n_iter>0 && ~accepted)
    for i=1:length(param_names)
        var = param_names{i};
        tag = inter_biips('change_data', console, pn_param(i).name, ...
            pn_param(i).lower, pn_param(i).upper, sample_param{i}, true);
        if ~tag
            error('Cannot reset previous data: %s=%.f', var, sample_param{i});
        end        
    end
    inter_biips('set_log_norm_const', console, log_marg_like)
end

if return_samples
    %% Set output structure
    for k=1:length(param_names) % Remove singleton dimensions for vectors
        size(samples_param_st{k})
        samples_all{k} = squeeze(samples_param_st{k});
        if size(samples_all{k}, ndims(samples_all{k}))==1 % needed because weird behavior of squeeze with [1,1,n]
            samples_all{k} = samples_all{k}';
        end
        variable_names{k} = param_names{k};
    end
    for k=1:length(latent_names) % Remove singleton dimensions for vectors
        samples_all{k+length(param_names)} = squeeze(samples_latent_st{k});
        if size(samples_all{k+length(param_names)}, ndims(samples_all{k+length(param_names)}))==1 % needed because weird behavior of squeeze with [1,1,n]
            samples_all{k+length(param_names)} = samples_all{k+length(param_names)}';
        end
        variable_names{k+length(param_names)} = latent_names{k};
    end
    varargout{1} = cell2struct_weaknames(samples_all, variable_names);
    varargout{2} = log_post_st;
    varargout{3} = log_marg_like_st;    
    out.accept_rate = accept_rate;
    out.n_fail = n_fail;
    out.step_rw = step_rw;
    varargout{4} = out;
else    
    out.accept_rate = accept_rate;
    out.n_fail = n_fail;
    out.step_rw = step_rw;
    out.log_marg_like = log_marg_like_st;
    out.log_post = log_post_st;
    varargout{1} = obj;
    varargout{2} = out;
end