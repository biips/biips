function [x_est, x_res, w_res, x, w, ESS, log_norm_const, n_child, ind_parent, varargout] = run_particle_filter(dimx, t_final, y_obs, initiate_particles, mutation, N, ESS_thres, varargin)

% Step 1 : Initialization
%========================

% Particle filter initialization
%-------------------------------
[x_0] = initiate_particles(N);

log_w_0 = -log(N) * ones(1,N);
w_0     = 1/N * ones(1,N);
x_0_est = mean(x_0,2);
ESS_0   = N;

x          = zeros(dimx,t_final+1,N);
x_res      = zeros(dimx,t_final+1,N);
log_w_t    = zeros(1,1,N);
w          = zeros(1,t_final+1,N);
w_res      = zeros(1,t_final+1,N);
x_est      = zeros(dimx,t_final+1);
ESS        = zeros(1,t_final+1);
log_norm_const = zeros(1,t_final+1);
n_child    = zeros(1,t_final+1,N);
ind_parent = zeros(1,t_final,N);

if (nargin == 8)
    var_z_0 = varargin{1};
    dimz = size(var_z_0, 1);    
    var_z = zeros(dimz, dimz, t_final+1, N);
    var_z(:,:,1,:) = var_z_0;
end

x(:,1,:)       = x_0;
x_res(:,1,:)   = x_0;
log_w_t(1,1,:) = log_w_0;
w(1,1,:)       = w_0;
w_res(1,1,:)   = w_0;
x_est(:,1)     = x_0_est;
ESS(1)         = ESS_0;
n_child(1,1,:) = ones(N,1);

% Step 2 : Iterations
%====================
for t = 1 : t_final
    % Mutation
    if (nargin == 7)
        [x(:,t+1,:), log_w_t(1,1,:), w(1,t+1,:), x_est(:,t+1), ESS(1,t+1), log_norm_const(1,t+1)] = mutate_particles(x_res(:,t,:), log_w_t(1,1,:), y_obs(:,t), mutation, t, log_norm_const(1,t));
    elseif (nargin == 8)
        [x(:,t+1,:), log_w_t(1,1,:), w(1,t+1,:), x_est(:,t+1), ESS(1,t+1), log_norm_const(1,t+1), var_z(:,:,t+1,:)] = mutate_particles(x_res(:,t,:), log_w_t(1,1,:), y_obs(:,t), mutation, t, log_norm_const(1,t), var_z(:,:,t,:));
    end
    % Resampling/Selection
    if ( ESS(1,t+1) / N < ESS_thres )
        [x_res(:,t+1,:), log_w_t(1,1,:), w_res(1,t+1,:), n_child(1,t+1,:), ind_parent(1,t,:)] = resample_particles(x(:,t+1,:), w(1,t+1,:), N);
    else
        x_res(:,t+1,:)     = x(:,t+1,:);
        w_res(1,t+1,:)     = w(1,t+1,:);
        n_child(1,t+1,:)   = ones(1,1,N);
        ind_parent(1,t,:)  = 1:N;
    end
end

if (nargin == 8)
    varargout = {var_z};
end

end
