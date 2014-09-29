function [obj, accept_rate, n_fail] = pmmh_one_update(obj, pn_param,...
    n_part, rs_thres, rs_type, rw_rescale, rw_learn)


%--------------------------------------------------------------------------
% PMMH_ONE_UPDATE performs one step of the PMMH algorithm
% [obj, accept_rate, n_fail] = pmmh_one_update(obj, ...
%     pn_param, n_part, rs_thres, rs_type, rw_rescale, rw_learn)
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

console = obj.model.id;
param_names = obj.param_names;
latent_names = obj.latent_names;
sample_param = obj.sample_param;
sample_param_tr = obj.sample_param_tr;
sample_latent = obj.sample_latent;
log_marg_like = obj.log_marg_like;
log_prior = obj.log_prior;

n_fail = 0;
n_param = numel(param_names);
n_latent = numel(latent_names);

% Increment iterations counter
obj.n_iter = obj.n_iter + 1;

% Random walk proposal
prop_tr = pmmh_rw_proposal(obj);
prop = pmmh_rw_transform(prop_tr, obj, 'inverse');
lderiv = pmmh_rw_transform(prop, obj, 'lderiv');

% Compute log prior density
log_prior_prop = 0;
for i=1:n_param
    ok = matbiips('change_data', console, pn_param(i).name, ...
        pn_param(i).lower, pn_param(i).upper, prop{i}, true);
    if ~ok
        % DATA CHANGE FAILED: proposed parameter value must be out of
        % bounds
        log_prior_prop = -Inf;
%         n_fail = n_fail + 1;
%         warning('Data change failed: %s = %.1f', param_names{i}, prop{i});
        break;
    end
    log_p = matbiips('get_log_prior_density', console, pn_param(i).name, ...
        pn_param(i).lower, pn_param(i).upper);
    
    log_p = log_p - sum(abs(lderiv{i}(:)));
    
    %%% FIXME check node is not stochastic: log_p = NA in R
    
    log_prior_prop = log_prior_prop + log_p;
end

if isnan(log_prior_prop)
%     accept_rate = 0;
    error('Failed to compute log prior density : %g', log_prior_prop);
end
if log_prior_prop==-Inf  % If proposal is not in the support of the prior
    accept_rate = 0;
else 
    % Compute the marginal likelihood: Run SMC sampler
    ok = smc_forward_algo(console, n_part, rs_thres, rs_type, get_seed());
    if ~ok
        log_marg_like_prop = -Inf;
        n_fail = n_fail + 1;
        warning('Failure running SMC forward sampler')
    else
        log_marg_like_prop = matbiips('get_log_norm_const', console);
        if isnan(log_marg_like_prop) || log_marg_like_prop==Inf
            %%% TODO error or n_fail increment ?
            error('Failed to compute log marginal likelihood : %g', log_marg_like_prop);
        end
    end
    
    % Acceptance rate
    accept_rate = min(1, exp(log_marg_like_prop - log_marg_like + ...
        log_prior_prop - log_prior));
    if isnan(accept_rate)
        error('Failed to compute acceptance rate: %g', accept_rate)
    end

    % Accept-reject step
    if rand < accept_rate
        sample_param = prop;
        sample_param_tr = prop_tr;
        log_prior = log_prior_prop;
        log_marg_like = log_marg_like_prop;
        
        if n_latent>0
            % Sample one realization of the monitored latent variables
            sampled_value = matbiips('sample_gen_tree_smooth_particle', console, get_seed());
            for i=1:n_latent
                %%% FIXME transform variable name. eg: x[1,] => x[1,1:100]
                var = to_biips_vname(latent_names{i});
                sample_latent{i} = getfield(sampled_value, var);
            end   
        end
    end
end

%% Update PMMH object with current state 
obj.sample_param = sample_param;
obj.sample_param_tr = sample_param_tr;
obj.sample_latent = sample_latent;
obj.log_prior = log_prior;
obj.log_marg_like = log_marg_like;

% rescale random walk stepsize
if rw_rescale
    obj = pmmh_rw_rescale(obj, accept_rate);
end

% Update empirical mean and covariance matrix
if rw_learn
    obj = pmmh_rw_learn_cov(obj);
end


