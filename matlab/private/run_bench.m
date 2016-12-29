function [errors_filter, errors_smooth, log_norm_const_bench, x_gen, y_obs, x_filter_bench, var_filter_bench, x_smooth_bench, var_smooth_bench] = run_bench(model_const, model_dim, sample_param, sample_obs, bench, smc, n_part, ess_thres, n_smc, mutation_names, ind_x)
%RUN_BENCH Summary of this function goes here
%   Detailed explanation goes here

%% Sample parameters
x_gen = sample_param(model_const, model_dim);

%% Sample observations
y_obs = sample_obs(model_const, model_dim, x_gen);

%% Compute bench filter and smoother reference
[x_filter_bench, var_filter_bench, x_smooth_bench, var_smooth_bench, log_norm_const_bench] = bench(model_const, model_dim, y_obs);

x_gen = x_gen(ind_x);
x_filter_bench = x_filter_bench(ind_x);
var_filter_bench = var_filter_bench(ind_x);
x_smooth_bench = x_smooth_bench(ind_x);
var_smooth_bench = var_smooth_bench(ind_x);

norm_const_bench = exp(log_norm_const_bench(end));

log_norm_const_bench = log_norm_const_bench(ind_x);

fprintf('True log-norm-const: %g\n', log_norm_const_bench(end))

%% Run SMC filters and smoothers
errors_filter = zeros(n_smc, length(n_part), length(smc));
errors_smooth = zeros(n_smc, length(n_part), length(smc));
norm_const = zeros(1, n_smc);

fprintf('Particles | Mutation | Log-norm-const-smc | Student p-value (>0.05)\n')
fprintf('-------------------------------------------------------------------\n')

h = waitbar(0,'Please wait...');
steps = sum(n_part) * length(smc) * n_smc;
step = 0;
t_start = tic;

for j = 1:length(n_part)
    for k = 1:length(smc)
        for i = 1:n_smc
            t_temp = tic;
%             [x_filter_smc, ess_filter, x_smooth_smc, ess_smooth] = smc{k}(model_const, model_dim, y_obs, n_part(j), ess_thres);
            [x_filter_smc, ~, x_smooth_smc, ~, log_norm_const] = smc{k}(model_const, model_dim, y_obs, n_part(j), ess_thres);
            
            %             t_final = model_const(1);
            %             figure()
            %             plot(0:t_final, x_gen, 0:t_final, x_filter_bench, 0:t_final, x_filter_smc, 0:t_final, x_smooth_bench, 0:t_final, x_smooth_smc)
            %             legend('true', 'bench filter', 'smc filter', 'bench smooth', 'smc smooth')
            
            x_filter_smc = x_filter_smc(ind_x);
%             ess_filter = ess_filter(ind_x);
            x_smooth_smc = x_smooth_smc(ind_x);
%             ess_smooth = ess_smooth(ind_x);
            
%             errors_filter(i,j,k) = sum((x_filter_smc-x_filter_bench).^2 ./ var_filter_bench .* ess_filter);
%             errors_smooth(i,j,k) = sum((x_smooth_smc-x_smooth_bench).^2 ./ var_smooth_bench .* ess_smooth);
            errors_filter(i,j,k) = sum((x_filter_smc-x_filter_bench).^2 ./ var_filter_bench) * n_part(j);
            errors_smooth(i,j,k) = sum((x_smooth_smc-x_smooth_bench).^2 ./ var_smooth_bench) * n_part(j);

            norm_const(i) = exp(log_norm_const(end));
            
            step = step + n_part(j);
            speed_instant = n_part(j)/toc(t_temp);
            t_remaining_sec = (steps-step) / speed_instant;
            t_remaining_min = floor(t_remaining_sec/60);
            t_remaining_sec = floor(t_remaining_sec-t_remaining_min*60);
            
            waitbar(step / steps, h, sprintf('N = %d (%d/%d), %s mutation (%d/%d), run %d/%d\nremaining %d min %d sec', n_part(j), j, length(n_part), mutation_names{k}, k, length(smc), i, n_smc, t_remaining_min, t_remaining_sec));
        end
        
        % T-test on the normalizing constants mean
        % H0 : E[norm_const] = norm_const_bench
        
        m = mean(norm_const);
        s = std(norm_const);
        
        t_stat = (m - norm_const_bench) / s * sqrt(n_smc);
        p_val = tcdf(t_stat, n_smc-1, 'upper');
        
        fprintf('%9d | %8s | %18g | %15g (%d)\n', n_part(j), mutation_names{k}, log(m), p_val, p_val>.05)
    end
end

toc(t_start)
close(h)
end

