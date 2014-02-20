function [out, log_post, log_marg_like, stats_pmmh] = ... 
    biips_pmmh_samples(obj, n_iter, n_part, varargin)

%% TODO: DOC


%% NOTE: For the time being, optional arg rw_step needs to be a cell with the same number of arguments as param_names
% TODO: change this to allow for numeric (and then duplicate values) - need to update sparsevar)

%% PROCESS AND CHECK INPUTS
% TODO: add some checks on nonoptional inputs

%%% Process and check optional arguments
optarg_names = {'thin', 'latent_names', 'max_fail', 'rs_thres', 'rs_type'};
optarg_default = {1, {}, 0, .5, 'stratified'};
optarg_valid = {[0, n_iter], {}, [0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'}};
optarg_type = {'numeric', 'char', 'numeric', 'numeric', 'char'};
[thin, latent_names, max_fail, rs_thres, rs_type] = parsevar(varargin, optarg_names,...
    optarg_type, optarg_valid, optarg_default);

return_samples = true;
[out, log_post, log_marg_like, stats_pmmh] = biips_pmmh(obj, n_iter, n_part,...
    return_samples, 'thin', thin, 'latent_names', latent_names, 'max_fail',...
    max_fail, 'rs_thres', rs_thres, 'rs_type', rs_type);