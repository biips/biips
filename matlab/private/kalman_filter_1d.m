function [x_tplus1, var_tplus1, x_pred_tplus1, var_pred_tplus1, y_pred_tplus1, s_tplus1] = kalman_filter_1d(x_t, var_t, y_t, u_t, f_t, c_t, q_t, h_t, r_t)

% Prediction
x_pred_tplus1 = f_t .* x_t + c_t * u_t;
var_pred_tplus1 = f_t.^2 .* var_t + q_t;
y_pred_tplus1 = h_t .* x_pred_tplus1;

% Update
inn_tplus1 = y_t - y_pred_tplus1;
s_tplus1 = h_t.^2 .* var_pred_tplus1 + r_t;
k_tplus1 = h_t .* var_pred_tplus1 ./ s_tplus1;
x_tplus1 = x_pred_tplus1 + k_tplus1 .* inn_tplus1;
var_tplus1 = (1 - h_t .* k_tplus1) .* var_pred_tplus1;

end

