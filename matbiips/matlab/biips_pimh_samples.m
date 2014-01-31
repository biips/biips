function out = biips_pimh_samples(console, variable_names, n_iter, n_part, varargin)

%% NEED HEADER


%%
% ADD SOME CHECKS ON THE INPUTS

%% Process options
rs_type = 'stratified';
rs_thres = 0.5;
thin = 1;
seed = get_seed();
parsevar; % Process options

%% Stops biips verbosity
inter_biips('verbosity', 0);
cleanupObj = onCleanup(@() inter_biips('verbosity', 1));% set verbosity on again when function terminates

%% Initialization
[sample, log_marg_like] = init_pimh(console, variable_names, n_part, rs_thres, rs_type, seed);
inter_biips('message', ['Generating PIMH samples with ' num2str(n_part) ' particles']);
bar = inter_biips('progress_bar', n_iter, '*', 'iterations');

n_samples = 0;
% Independent Metropolis-Hastings iterations
for i=1:n_iter
    [sample, log_marg_like, accepted] = one_update_pimh(console, ...
        variable_names, n_part, rs_thres, rs_type, sample, log_marg_like);
    
    % Store output
    if mod(i-1, thin)==0
        n_samples = n_samples + 1;
        out.log_marg.like(n_samples) = log_marg_like;
        for i=1:length(variable_names)
%             out.(variable_names{i}) = 
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
