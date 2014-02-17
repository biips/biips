function biips_pmmh_update(console, param_names, n_iter, n_part, varargin)

%% TODO: DOC



%% PROCESS AND CHECK INPUTS
% TODO: add some checks on nonoptional inputs

%%% Process and check optional arguments
optarg_names = {'max_fail', 'inits', 'rw_step', 'rw_rescale',...
    'rw_learn', 'rw_rescale_type', 'n_rescale', 'rs_thres', 'rs_type', 'seed'};
optarg_default = {0, {}, [], true, true, 'p', n_iter/4,...
    .5, 'stratified', get_seed()};
optarg_valid = {[0, intmax], {}, [], {true, false}, {true, false}, ...
    {}, [0,n_iter], [0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'},[0, intmax]};
optarg_type = {'numeric', 'cell', 'numeric', 'logical', 'logical',...
    'char', 'numeric', 'numeric', 'char', 'numeric'};
[max_fail, inits, rw_step, rw_rescale, rw_learn, rw_rescale_type,...
    n_rescale, rs_thres, rs_type, seed] = parsevar(varargin, optarg_names,...
    optarg_type, optarg_valid, optarg_default);

% TODO: check rw_step


%% Stops biips verbosity
inter_biips('verbosity', 0);
cleanupObj = onCleanup(@() inter_biips('verbosity', 1));% set verbosity on again when function terminates

% Initialize
[sample_param, sample_latent, log_prior, log_marg_like, rw] = init_pmmh(console, param_names, n_part, varargin{:});
param_names
pn_param =  cellfun(@parse_varname, param_names);

% TODO: assign rw step


% TODO: stop adaptation if necessary


% display message and progress bar
% is_adapt = 1;
if rw_learn % CHECK IF THIS IS CORRECT
    inter_biips('message', ['Adapting PMMH with ', num2str(n_part) ' particles'])    
    inter_biips('make_progress_bar', n_iter, '+', 'iterations')
else
    inter_biips('message', ['Updating PMMH with ', num2str(n_part) ' particles'])
    inter_biips('make progress_bar', n_iter, '*', 'iterations')
end

% Initialize counters
n_fail = 0;
n_accept = 0;
accept_rate = zeros(n_iter, 1);
%   step=list()

% PMMH iterations
for i=1:n_iter
    out = one_update_pmmh();
    
end