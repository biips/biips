function one_update_pmmh(console, param_names, latent_names, pn_param,...
    sample, log_prior, log_marg_like, n_part, rw_rescale, rw_learn,rw_rescale_type)

n_fail = 0;

% Remove duplicate entries
pn_param = cellfun(@parse_varname, param_names);

% TODO: random walk proposal

% TODO: Check NA

log_prior_prop = 0;

for i=1:length(param_names)
    var = param_names{i};
    tag = inter_biips('change_data', console, pn_param(i).name, ...
        pn_param(i).lower, pn_param(i).upper, inits{i}, true);
    if ~tag
        error('Data change failed: invalid value for variable %s', var);
    end
    sample = setfield(sample, var, inits{i}); % PB: will not work with [] TBD
end