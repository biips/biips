function [sample, log_marg_like, accepted] = one_update_pimh(console, variable_names, npart, rs_thres, rs_type, sample, log_marg_like)

%% SMC
run_smc_forward(console, npart, rs_thres, rs_type);

%% Acceptance rate
log_marg_like_prop = inter_biips('get_log_norm_const', console);
log_ar = log_marg_like_prop - log_marg_like;

%% Metropolis-Hastings step
accepted = (rand<exp(log_ar));
if accepted
    log_marg_like = log_marg_like_prop;
    
    % Sample one particle
    rng_seed = get_seed();
    inter_biips('sample_gen_tree_smooth_particle', console, rng_seed);
    
    % Get sampled value
    sampled_value = inter_biips('get_sampled_gen_tree_smooth_particle', console);
    cell_struct = cell(length(variable_names), 1);
    for i=1:length(variable_names)
        cell_struct{i} = getfield(sampled_value, variable_names{i});
    end
    sample = cell2struct_weaknames(cell_struct, variable_names);
end
    
