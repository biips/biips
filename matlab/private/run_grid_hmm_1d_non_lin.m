function [mean_filter, var_filter, mean_smooth, var_smooth, log_norm_const] = run_grid_hmm_1d_non_lin(t_final, y_obs, xinf, xsup, log_init_prior_pdf, log_prior_pdf, log_like_pdf, n_cell)

% make grid
xstep = (xsup-xinf) / n_cell;
xgrid = zeros(t_final+1,n_cell+1);
for t=1:t_final+1
    xgrid(t,:) = xinf(t):xstep(t):xsup(t);
end

% filter
%-------
% initiate
log_post_filter = zeros(t_final+1, n_cell+1);
log_post_filter(1,:) = log_init_prior_pdf(xgrid(1,:));

% mean estimate t = 0
mean_filter = zeros(1, t_final+1);
integrand = xgrid(1,:) .* exp(log_post_filter(1,:));
mean_filter(1) = sum(xstep(1) * (integrand(1:end-1) + integrand(2:end))/2);

% variance estimate t = 0
var_filter = zeros(1, t_final+1);
integrand = integrand .* xgrid(1,:);
var_filter(1) = sum(xstep(1) * (integrand(1:end-1) + integrand(2:end))/2) - mean_filter(1)^2;

log_norm_const = zeros(1, t_final+1);

log_incr_prior = zeros(t_final,n_cell+1, n_cell+1);
log_prior = zeros(t_final,n_cell+1);

% iterate
for t=1:t_final
    for i=1:n_cell+1
        log_incr_prior(t,i,:) = log_prior_pdf(xgrid(t+1,:), xgrid(t,i), t);
    end
    integrand = repmat(log_post_filter(t,:)',1,n_cell+1) + squeeze(log_incr_prior(t,:,:));
    integrand = exp(integrand);
    
    prior = sum(xstep(t) * (integrand(1:end-1,:) + integrand(2:end,:))/2, 1);
    log_prior(t,:) = log(prior);
    
    log_like = log_like_pdf(y_obs(t), xgrid(t+1,:), t);
    
    integrand = prior .* exp(log_like);
    norm_const_incr = sum(xstep(t+1) * (integrand(1:end-1) + integrand(2:end))/2);
    
    log_norm_const(t+1) = log_norm_const(t) + log(norm_const_incr);
    
    log_post_filter(t+1,:) = log_prior(t,:) + log_like - log(norm_const_incr);
    post = exp(log_post_filter(t+1,:));
    
    % mean estimate
    integrand = xgrid(t+1,:) .* post;
    mean_filter(t+1) = sum(xstep(t+1) * (integrand(1:end-1) + integrand(2:end))/2);
    
    % variance estimate
    integrand = integrand .* xgrid(t+1,:);
    var_filter(t+1) = sum(xstep(t+1) * (integrand(1:end-1) + integrand(2:end))/2) - mean_filter(t+1)^2;
end

% smoother
%---------
% initiate
log_post_smooth = zeros(t_final+1, n_cell+1);
log_post_smooth(end,:) = log_post_filter(end,:);

% mean estimate t = t_final
mean_smooth = zeros(1, t_final+1);
integrand = xgrid(end,:) .* exp(log_post_smooth(end,:));
mean_smooth(end) = sum(xstep(end) * (integrand(1:end-1) + integrand(2:end))/2);

% variance estimate t = t_final
var_smooth = zeros(1, t_final+1);
integrand = integrand .* xgrid(end,:);
var_smooth(end) = sum(xstep(end) * (integrand(1:end-1) + integrand(2:end))/2) - mean_smooth(end)^2;

% iterate
for t=t_final-1:-1:0
    integrand = exp(squeeze(log_incr_prior(t+1,:,:)) - repmat(log_prior(t+1,:),n_cell+1,1) + repmat(log_post_smooth(t+2,:),n_cell+1,1));
    back_incr = sum(xstep(t+2) * (integrand(:,1:end-1) + integrand(:,2:end))/2, 2);
    log_post_smooth(t+1,:) = log_post_filter(t+1,:) + log(back_incr)';
    
    post_smooth = exp(log_post_smooth(t+1,:));
    
    % mean estimate
    integrand = xgrid(t+1,:) .* post_smooth;
    mean_smooth(t+1) = sum(xstep(t+1) * (integrand(1:end-1) + integrand(2:end))/2);
    
    % variance estimate
    integrand = integrand .* xgrid(t+1,:);
    var_smooth(t+1) = sum(xstep(t+1) * (integrand(1:end-1) + integrand(2:end))/2) - mean_smooth(t+1)^2;
end
end
