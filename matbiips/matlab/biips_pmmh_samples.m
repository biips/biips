function [out, log_post, log_marg_like, stats_pmmh] = biips_pmmh_samples(console, param_names, n_iter, n_part, varargin)

%% TODO: DOC


%% NOTE: For the time being, optional arg rw_step needs to be a cell with the same number of arguments as param_names
% TODO: change this to allow for numeric (and then duplicate values) - need to update sparsevar)

%% PROCESS AND CHECK INPUTS
% TODO: add some checks on nonoptional inputs

%%% Process and check optional arguments
optarg_names = {'rw', 'thin', 'latent_names', 'max_fail', 'rw_cov', 'rw_step', 'rs_thres', 'rs_type', 'seed'};
optarg_default = {struct(), 1, {}, 0, {}, [],...
    .5, 'stratified', get_seed()};
optarg_valid = {{}, [0, n_iter], {}, [0, intmax], {}, {}, [0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'},[0, intmax]};
optarg_type = {'struct', 'numeric', 'char', 'numeric', 'numeric', 'numeric', 'char', 'numeric'};
[rw, thin, latent_names, max_fail, rw_cov, rw_step, rs_thres, rs_type, seed] = parsevar(varargin, optarg_names,...
    optarg_type, optarg_valid, optarg_default);
return_samples = true;
[out, log_post, log_marg_like, stats_pmmh] = biips_pmmh(console, param_names, n_iter, n_part,...
    return_samples, 'rw', rw, 'thin', thin, 'latent_names', latent_names, 'max_fail',...
    max_fail, 'rw_cov', rw_cov, 'rw_step', rw_step, 'rs_thres', rs_thres,...
    'rs_type', rs_type, 'seed', seed);