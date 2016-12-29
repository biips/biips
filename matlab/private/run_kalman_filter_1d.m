function [x_est_KF,var, x_pred_KF, var_pred, log_norm_const] = run_kalman_filter_1d(x_0, var_0, t_final, u, y, f_t, c_t, q_t, h_t, r_t)

%% Initialization
x_est_KF = zeros(1,t_final+1);
x_pred_KF = zeros(1,t_final+1);
log_norm_const = zeros(1,t_final+1);

x_est_KF(1) = x_0;
x_pred_KF(1) = x_0;
var = zeros(1,t_final+1);
var_pred = zeros(1,t_final+1);
var(:,1) = var_0;
var_pred(:,1) = var_0;

%% Iterations
for t = 1 : t_final
    [x_est_KF(t+1), var(t+1), x_pred_KF(t+1), var_pred(t+1), y_pred_tplus1, s_tplus1] = kalman_filter_1d(x_est_KF(t), var(t), y(t), u(t), f_t, c_t, q_t, h_t, r_t);
    log_norm_const(t+1) = log_norm_const(t) - 0.5*( log(2*pi*s_tplus1) + (y(t)-y_pred_tplus1)^2/s_tplus1 );
end
