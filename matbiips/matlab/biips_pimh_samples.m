function [out, log_marg_like_st] = biips_pimh_samples(console, variable_names, n_iter, n_part, varargin)

%% FC: NEED HEADER

%% PROCESS AND CHECK INPUTS
% CAN WE ADD A CHECK ON THE VARIABLES??
%%% Process and check optional arguments
optarg_names = {'rs_thres', 'rs_type', 'seed', 'thin'};
optarg_default = {.5, 'stratified', get_seed(), 1};
optarg_valid = {[0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'},...
    [0, intmax], [1, n_iter]};
optarg_type = {'numeric', 'char', 'numeric', 'numeric'};
[rs_thres, rs_type, seed, thin] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);
% 
% %% Process options
% rs_type = 'stratified';
% rs_thres = 0.5;
% thin = 1;
% seed = get_seed();
% parsevar; % Process options

%% Stops biips verbosity
inter_biips('verbosity', 0);
cleanupObj = onCleanup(@() inter_biips('verbosity', 1));% set verbosity on again when function terminates

%% Initialization
[sample, log_marg_like] = init_pimh(console, variable_names, n_part, rs_thres, rs_type, seed);
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
    [sample, log_marg_like, accepted] = one_update_pimh(console, ...
        variable_names, n_part, rs_thres, rs_type, sample, log_marg_like);
    
    % Store output
    if mod(i-1, thin)==0
        ind_sample = (i-1)/thin + 1;
        log_marg_like_st(ind_sample) = log_marg_like;
        for k=1:length(variable_names)
            switch(n_dim(k)) % !!!FC: NOT VERY ELEGANT PIECE OF CODE!!!
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
% out.log_marg_like = log_marg_like_st;
