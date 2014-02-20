function [obj, out] = biips_pmmh_update(obj, n_iter, n_part, varargin)

%% TODO: DOC


%% NOTE: For the time being, optional arg rw_step needs to be a cell with the same number of arguments as param_names
% TODO: change this to allow for numeric (and then duplicate values) - need to update sparsevar)

%% PROCESS AND CHECK INPUTS
% TODO: add some checks on nonoptional inputs

%%% Process and check optional arguments
optarg_names = {'max_fail', 'rw_learn', 'rs_thres', 'rs_type'};
optarg_default = {0, true, .5, 'stratified'};
optarg_valid = {[0, intmax], {true, false}, [0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'}};
optarg_type = {'numeric', 'logical', 'numeric', 'char'};
[max_fail, rw_learn, rs_thres, rs_type] = parsevar(varargin, optarg_names,...
    optarg_type, optarg_valid, optarg_default);

return_samples = false;
[obj, out] = biips_pmmh(obj, n_iter, n_part,...
    return_samples, 'max_fail', max_fail, 'rw_learn', rw_learn,...
    'rs_thres', rs_thres, 'rs_type', rs_type);