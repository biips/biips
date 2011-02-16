function [x_est_KS, var_KS] = run_kalman_smoother_1D(x_est_KF, var_KF, x_pred_KF, var_pred_KF, t_final, a)

x_est_KS = zeros(size(x_est_KF));
var_KS = zeros(size(var_KF));

x_est_KS(:,t_final+1) = x_est_KF(:,t_final+1);
var_KS(:,t_final+1) = var_KF(:,t_final+1);

for t = t_final : -1 : 1
    C_t = a .* var_KF(:,t) ./ var_pred_KF(:,t+1);
    var_KS(:,t) = var_KF(:,t) + C_t.^2 .* (var_KS(:,t+1) - var_pred_KF(:,t+1));
    x_est_KS(:,t) = x_est_KF(:,t) + C_t .* (x_est_KS(:,t+1) - x_pred_KF(:, t+1));
end
end