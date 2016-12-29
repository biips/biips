function [x_t_res, log_w_t_res, w_t_res, n_child_t, ind_parent_t] = resample_particles(x_t, w_t, N)
% Resampling/Selection

partition         = cat(3,zeros(1,1,1), cumsum(w_t));
sample            = rand(1, N);
[n, ind_parent_t] = histc(sample, partition);
n_child_t         = n(1:N);
x_t_res           = x_t(:,1,ind_parent_t);
log_w_t_res       = -log(N) * ones(1, N);
w_t_res           = 1/N * ones(1, N);

end
