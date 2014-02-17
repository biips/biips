function biips_pmmh_update(console, param_names, n_iter, n_part, varargin)

%% TODO: DOC


%% NOTE: For the time being, optional arg rw_step needs to be a cell with the same number of arguments as param_names
% TODO: change this to allow for numeric (and then duplicate values) - need to update sparsevar)

%% PROCESS AND CHECK INPUTS
% TODO: add some checks on nonoptional inputs

%%% Process and check optional arguments
optarg_names = {'latent_names', 'max_fail', 'inits', 'rw_step', 'rw_rescale',...
    'rw_learn', 'rw_rescale_type', 'n_rescale', 'rs_thres', 'rs_type', 'seed'};
optarg_default = {{}, 0, {}, [], true, true, 'dureau', n_iter/4,...
    .5, 'stratified', get_seed()};
optarg_valid = {{}, [0, intmax], {}, {}, {true, false}, {true, false}, ...
    {'dureau','plummer'}, [0,n_iter], [0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'},[0, intmax]};
optarg_type = {'char', 'numeric', 'numeric', 'numeric', 'logical', 'logical',...
    'char', 'numeric', 'numeric', 'char', 'numeric'};
[latent_names, max_fail, inits, rw_step, rw_rescale, rw_learn, rw_rescale_type,...
    n_rescale, rs_thres, rs_type, seed] = parsevar(varargin, optarg_names,...
    optarg_type, optarg_valid, optarg_default);

n_param = length(param_names);

% Check rw_step
rw_step_values = {};
if ~isempty(rw_step)    
    if isnumeric(rw_step) % NEED TO MODIFY SPARSEVAR TO ALLOW THIS
        for i=1:n_param
            rw_step_values{i} = rw_step; 
        end
    elseif iscell(rw_step)
        if length(rw_step) ~= length(param_names)
            warning('Dimension of rw_step does not match the dimension of param_names - skipped argument');
        else
            rw_step_values = rw_step;
        end
    end
end

%% Stops biips verbosity
inter_biips('verbosity', 0);
cleanupObj = onCleanup(@() inter_biips('verbosity', 1));% set verbosity on again when function terminates

latent_names
% Initialize
[sample_param, sample_latent, log_prior, log_marg_like, rw] = init_pmmh(console, param_names, n_part, varargin{:});
param_names
pn_param =  cellfun(@parse_varname, param_names);

% Update rw structure
if ~isempty(rw_step_values)
    rw = pmmh_rw_step(rw, rw_step_values);
end
rw.rescale = rw_rescale;
rw.learn = rw_learn;
rw.rescale_type = rw_rescale_type;


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
    [sample_param, sample_latent, log_prior, log_marg_like, ...
    accept_rate, accepted, n_fail, rw] = one_update_pmmh(console, param_names, pn_param, sample_param,...
        sample_latent, latent_names, log_prior, log_marg_like,  n_part, rs_thres,...
        rs_type,seed, rw);
    
end