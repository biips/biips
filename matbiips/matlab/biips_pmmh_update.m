function [rw, out] = biips_pmmh_update(console, param_names, n_iter, n_part, varargin)

%% TODO: DOC


%% NOTE: For the time being, optional arg rw_step needs to be a cell with the same number of arguments as param_names
% TODO: change this to allow for numeric (and then duplicate values) - need to update sparsevar)

%% PROCESS AND CHECK INPUTS
% TODO: add some checks on nonoptional inputs

%%% Process and check optional arguments
optarg_names = {'latent_names', 'max_fail', 'inits', 'rw_step', 'rw_rescale',...
    'rw_learn', 'rw_rescale_type', 'n_rescale', 'rs_thres', 'rs_type'};
optarg_default = {{}, 0, {}, [], true, true, 'dureau', n_iter/4,...
    .5, 'stratified'};
optarg_valid = {{}, [0, intmax], {}, {}, {true, false}, {true, false}, ...
    {'dureau','plummer'}, [0,n_iter], [0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'}};
optarg_type = {'char', 'numeric', 'numeric', 'numeric', 'logical', 'logical',...
    'char', 'numeric', 'numeric', 'char'};
[latent_names, max_fail, inits, rw_step, rw_rescale, rw_learn, rw_rescale_type,...
    n_rescale, rs_thres, rs_type] = parsevar(varargin, optarg_names,...
    optarg_type, optarg_valid, optarg_default);

return_samples = false;
[rw, out] = biips_pmmh(console, param_names, n_iter, n_part,...
    return_samples, varargin{:});