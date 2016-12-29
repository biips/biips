function [x_tplus1, log_w_tplus1, w_tplus1, x_t_est, ESS_t, log_norm_const_tplus1, varargout] = mutate_particles(x_t, log_w_t, y_tplus1, mutation, t, log_norm_const_t, varargin)

dimx = size(x_t, 1);

% Mutation
if nargin == 6
    [x_tplus1, log_w_update] = mutation(x_t, y_tplus1, t);
elseif nargin == 7
    var_z_t = varargin{1};
    
    [x_tplus1, log_w_update, var_z_tplus1] = mutation(x_t, y_tplus1, var_z_t, t);
    
    varargout = {var_z_tplus1};
end

log_w_tplus1 = log_w_t + log_w_update;

w_tplus1 = exp(log_w_tplus1);
W = sum(w_tplus1);
w_tplus1 = w_tplus1 / W;
log_w_tplus1 = log_w_tplus1 - log(W);

x_t_est = zeros(dimx,1);
% State estimation
for i = 1 : dimx
    x_t_est(i) = sum(w_tplus1 .* x_tplus1(i,1,:));
end
% Quality of the approximation : Effective Sample Size (ESS)
ESS_t = 1 / sum(w_tplus1.^2);

% normalizing constant
log_norm_const_tplus1 = log_norm_const_t + log(W);

end
