function biips_pimh_update(console, variable_names, n_iter, n_part, varargin)

%% NEED HEADER




%% PROCESS AND CHECK INPUTS
% CAN WE ADD A CHECK ON THE VARIABLES??
%%% Process and check optional arguments
optarg_names = {'rs_thres', 'rs_type', 'seed'};
optarg_default = {.5, 'stratified', get_seed()};
optarg_valid = {[0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'},...
    [0, intmax]};
optarg_type = {'numeric', 'char', 'numeric'};
[rs_thres, rs_type, seed] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

% %% Process options
% rs_type = 'stratified';
% rs_thres = 0.5;
% seed = get_seed();
% parsevar; % Process options

%% Stops biips verbosity
inter_biips('verbosity', 0);
cleanupObj = onCleanup(@() inter_biips('verbosity', 1));% set verbosity on again when function terminates

%% Initialization
[sample, log_marg_like] = init_pimh(console, variable_names, n_part, rs_thres, rs_type, seed);
inter_biips('message', ['Updating PIMH with ' num2str(n_part) ...
    ' particles and ' num2str(n_iter) ' iterations']);

% !!! NOTE FRANCOIS: LA FONCTION PROGRESS_BAR, utilise dans R, n'existe pas
% dans inter_biips - remplace par fonction MAKE_PROGRESS_BAR
bar = inter_biips('make_progress_bar', n_iter, '*', 'iterations');

% Independent Metropolis-Hastings iterations
for i=1:n_iter
    [sample, log_marg_like, accepted] = one_update_pimh(console, ...
        variable_names, n_part, rs_thres, rs_type, sample, log_marg_like);
    
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
