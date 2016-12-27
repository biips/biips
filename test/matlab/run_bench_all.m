%                                               -*- matlab -*-
%
%  Biips software is a set of C++ libraries for
%  Bayesian inference with interacting Particle Systems.
%  Copyright (C) Inria, 2012
%  Authors: Adrien Todeschini, Francois Caron
%  
%  Biips is derived software based on:
%  JAGS, Copyright (C) Martyn Plummer, 2002-2010
%  SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
%
%  This file is part of Biips.
%
%  Biips is free software: you can redistribute it and/or modify
%  it under the terms of the GNU General Public License as published by
%  the Free Software Foundation, either version 3 of the License, or
%  (at your option) any later version.
%
%  This program is distributed in the hope that it will be useful,
%  but WITHOUT ANY WARRANTY; without even the implied warranty of
%  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%  GNU General Public License for more details.
%
%  You should have received a copy of the GNU General Public License
%  along with this program.  If not, see <http://www.gnu.org/licenses/>.
%
%
%  \file     run_bench_all.m
%  \brief    
%
%  \author   $LastChangedBy$
%  \date     $LastChangedDate$
%  \version  $LastChangedRevision$
%  Id:       $Id$
%


function [errors_filter_all, errors_smooth_all, log_norm_const_bench_all, models] = run_bench_all(model_ids, n_part, ess_thres, n_smc, results_file_names)
%RUN_BENCH_ALL Runs all implemented benches
%   [errors_filter_all, errors_smooth_all, norm_const_bench_all, models] = run_bench_all(model_ids, n_part, ess_thres, n_smc, results_file_names)
%   
%   For each model ids, the program
%   - samples one set of observations,
%   - computes the true filtering/smoothing mean and variance of the
%   unobserved variable
%   - runs n_smc SMC algorithms with Multinomial resampling for each 
%   implemented mutation and each particles number
%   - computes the normalized errors
%   - plots errors empirical distributions.
%   - prints everything in a configuration file using a syntax that can be 
%   input in BiipsTest program.
% 
%   Example:
%     run_bench_all([1,2], [100, 500, 1000], 0.5, 100, {'hmm_1d_lin_gauss.cfg', 'hmm_1d_non_lin_gauss.cfg'})
%   
%   will run 100 SMC algorithms for models 1 (HMM 1d linear gaussian) and 2 (HMM 1d non linear
%   gaussian, using:
%   - 100, 500 and 1000 particles for each implemented mutation
%   - Multinomial resampling when ESS/N <= 0.5
%   Output will be written in 'hmm_1d_lin_gauss.cfg' file for model 1 and in
%   'hmm_1d_non_lin_gauss.cfg' file for model 2.
models = {};
errors_filter_all = {};
errors_smooth_all = {};
log_norm_const_bench_all = {};


%% HMM 1D linear gaussian

if (sum(model_ids == 1))
    model_id = 1;
    dimx = 1;
    dimy = 1;
    
    t_max = 20;
    mu_x_0 = 0;
    var_x_0 = 1;
    var_x = 1;
    var_y = 0.5;
    ind_x = 2:(t_max+1);    
      
    initiate = @(n) mu_x_0 + sqrt(var_x_0) * randn(1, n);
    evolution_model = @(x_t, t) x_t + sqrt(var_x) * randn(size(x_t));
    measure_model = @(x_t, t) x_t + sqrt(var_y) * randn(size(x_t));
    
    sample_param = @(model_const, model_dim) generate_state(model_dim(1), model_const(1), initiate, evolution_model);
    sample_obs = @(model_const, model_dim, x_gen) generate_measures(model_dim(2), model_const(1), x_gen, measure_model);
    
    lognormpdf = @(x, m, v) -1/2 * (log(2*pi*v) + (x-m).^2./v);
    
    log_prior_pdf = @(x_tplus1, x_t, t) lognormpdf(x_tplus1, x_t, var_x);
    log_like_pdf = @(y_t, x_t, t) lognormpdf(y_t, x_t, var_y);
    
    bench = @(model_const, model_dim, y_obs) bench_hmm_1d_lin_gauss(model_const, y_obs);
    
    prior_mutation = @(x_t, y_tplus1, t) prior_mutation_hmm(x_t, y_tplus1, t, evolution_model, log_like_pdf);
    optimal_mutation = @(x_t, y_tplus1, t) optimal_mutation_1D_lin(x_t, y_tplus1, 1, sqrt(var_x), 1, sqrt(var_y));
%   bad_mutation = @(x_t, y_tplus1, t) bad_mutation_hmm(x_t, y_tplus1, t, evolution_model);
    
    backward_smoother = @(x_PF_t, x_PF_tplus1, w_PF_t, w_PS_tplus1, t) backward_smoother_hmm_1d(x_PF_t, x_PF_tplus1, w_PF_t, w_PS_tplus1, t, log_prior_pdf);
    
    smc_prior = @(model_const, model_dim, y_obs, n_part, ess_thres) smc_hmm(model_const, model_dim, y_obs, n_part, ess_thres, initiate, prior_mutation, backward_smoother);
    smc_optimal = @(model_const, model_dim, y_obs, n_part, ess_thres) smc_hmm(model_const, model_dim, y_obs, n_part, ess_thres, initiate, optimal_mutation, backward_smoother);
%     smc_bad = @(model_const, model_dim, y_obs, n_part, ess_thres) smc_hmm(model_const, model_dim, y_obs, n_part, ess_thres, initiate, bad_mutation, backward_smoother);
    
    smc = {smc_prior, smc_optimal};
    mutation_names = {'prior', 'optimal'};
%     smc = {smc_prior, smc_optimal, smc_bad};
%     mutation_names = {'prior', 'optimal', 'bad'};
    
    model_const = [t_max, mu_x_0, var_x_0, var_x, var_y];
    const_names = {'t_max', 'mean_x0', 'var_x0', 'var_x', 'var_y'};
   
    model_dim = [dimx, dimy];
    var_names = {'x', 'y'};
    
    model_name = sprintf('HMM 1D linear gaussian, t = 0:%d', t_max);
    
    model = {model_const, model_dim, sample_param, sample_obs, bench, smc, mutation_names, model_name, const_names, var_names, model_id};
    models = [models, {model} ];
end

%% HMM 1D non linear gaussian

if (sum(model_ids == 2))
    model_id = 2;
    dimx = 1;
    dimy = 1;
    t_max = 20;
    mu_x_0 = 0;
    var_x_0 = 5;
    var_x = 10;
    var_y = 1;
    ind_x = 2:(t_max+1);    
    
    f_evolution = @(x, t) 1/2*x + 25*x./(1+x.^2) + 8*cos(1.2*t);
    g_measure = @(x, t) x.^2/20;
    
    initiate = @(n) mu_x_0 + sqrt(var_x_0) * randn(1, n);
    evolution_model = @(x_t, t) f_evolution(x_t, t) + sqrt(var_x) * randn(size(x_t));
    measure_model = @(x_t, t) g_measure(x_t, t) + sqrt(var_y) * randn(size(x_t));
    
    sample_param = @(model_const, model_dim) generate_state(model_dim(1), model_const(1), initiate, evolution_model);
    sample_obs = @(model_const, model_dim, x_gen) generate_measures(model_dim(2), model_const(1), x_gen, measure_model);
    
    lognormpdf = @(x, m, v) -1/2 * (log(2*pi*v) + (x-m).^2./v);
    
    log_init_prior_pdf = @(x_0) lognormpdf(x_0, mu_x_0, var_x_0);
    log_prior_pdf = @(x_tplus1, x_t, t) lognormpdf(x_tplus1, f_evolution(x_t, t), var_x);
    log_like_pdf = @(y_t, x_t, t) lognormpdf(y_t, g_measure(x_t, t), var_y);
    
    n_cell = 1000;
    bench = @(model_const, model_dim, y_obs) bench_hmm_1d_non_lin_gauss(model_const, y_obs, f_evolution, log_init_prior_pdf, log_prior_pdf, log_like_pdf, n_cell);
    
    prior_mutation = @(x_t, y_tplus1, t) prior_mutation_hmm(x_t, y_tplus1, t, evolution_model, log_like_pdf);
    
    backward_smoother = @(x_PF_t, x_PF_tplus1, w_PF_t, w_PS_tplus1, t) backward_smoother_hmm_1d(x_PF_t, x_PF_tplus1, w_PF_t, w_PS_tplus1, t, log_prior_pdf);
    
    smc_prior = @(model_const, model_dim, y_obs, n_part, ess_thres) smc_hmm(model_const, model_dim, y_obs, n_part, ess_thres, initiate, prior_mutation, backward_smoother);
    
    smc = {smc_prior};
    mutation_names = {'prior'};
    
    model_const = [t_max, mu_x_0, var_x_0, var_x, var_y];
    const_names = {'t_max', 'mean_x0', 'var_x0', 'var_x', 'var_y'};
    
    model_dim = [dimx, dimy];
    var_names = {'x', 'y'};
    
    model_name = sprintf('HMM 1D nonlinear gaussian, t = 0:%d', t_max);
    
    model = {model_const, model_dim, sample_param, sample_obs, bench, smc, mutation_names, model_name, const_names, var_names, model_id};
    
    models = [models, {model} ];
end

%% Run all model benches

for i = 1:length(models)
    model = models{i};
    
    model_const = model{1};
    model_dim = model{2};
    sample_param = model{3};
    sample_obs = model{4};
    bench = model{5};
    smc = model{6};
    mutation_names = model{7};
    model_name = model{8};
    
    fprintf('--------------------------------------------------------\n')
    fprintf('Model: %s\n', model_name)
    
    [errors_filter, errors_smooth, log_norm_const_bench, x_gen, y_obs, x_filter_bench, var_filter_bench, x_smooth_bench, var_smooth_bench] = run_bench(model_const, model_dim, sample_param, sample_obs, bench, smc, n_part, ess_thres, n_smc, mutation_names, ind_x);
    
    % plot errors distributions
%     scrsz = get(0, 'ScreenSize');
%     figure('Position', [1 1 scrsz(3)/2 scrsz(4)])
    %h_fig = figure();
    
    %t_max = model_const(1);
    %k_degree = floor((t_max+1)/period);
    %n_bin = 40;
    %
    %fig_title = sprintf('%s\nParticle filter: distributions of the sum of normalized squared errors (period = %d)\n%d smc runs, ESS/N resampling threshold = %g', model_name, period, n_smc, ess_thres);
    %%plot_error_dist(errors_filter, h_fig, 1, n_bin, fig_title, n_part, mutation_names, k_degree)
    %
    %fig_title = sprintf('%s\nParticle smoother: distributions of the sum of normalized squared errors (period = %d)\n%d smc runs, ESS/N resampling threshold = %g', model_name, period, n_smc, ess_thres);
    %%plot_error_dist(errors_smooth, h_fig, 2, n_bin, fig_title, n_part, mutation_names, k_degree)

    %% print results
    print_results(results_file_names{i}, n_part, ess_thres, model, x_gen, y_obs, log_norm_const_bench(end), x_filter_bench, var_filter_bench, x_smooth_bench, var_smooth_bench, errors_filter, errors_smooth);

    %errors_filter_all = [errors_filter_all, {errors_filter} ];
    %errors_smooth_all = [errors_smooth_all, {errors_smooth} ];
    %log_norm_const_bench_all = [log_norm_const_bench_all, {log_norm_const_bench(end)} ];
end

end

%% Helper functions for HMM models

function [x_tplus1, log_w_update] = prior_mutation_hmm(x_t, y_tplus1, t, evolution_model, log_like_pdf)
x_tplus1 = evolution_model(x_t, t);

log_w_update = log_like_pdf(y_tplus1, x_tplus1, t);
end

function [x_tplus1, log_w_update] = bad_mutation_hmm(x_t, y_tplus1, t, evolution_model)
x_tplus1 = evolution_model(x_t, t);

log_w_update = ones(size(x_t,2), 1);
end

function [x_filter_smc, ess_filter, x_smooth_smc, ess_smooth, log_norm_const] = smc_hmm(model_const, model_dim, y_obs, n_part, ess_thres, initiate, mutation, backward_smoother)
t_max = model_const(1);
dimx = model_dim(1);

% Forward particle filter
[x_filter_smc, ~, ~, x_PF, w_PF, ess_filter, log_norm_const] = run_particle_filter(dimx, t_max, y_obs, initiate, mutation, n_part, ess_thres);
% Backward particle smoother
[x_smooth_smc, ~, ess_smooth] = run_backward_smoother(t_max, x_PF, w_PF, backward_smoother);
end


function [x_filter_bench, var_filter_bench, x_smooth_bench, var_smooth_bench, log_norm_const] = bench_hmm_1d_lin_gauss(model_const, y_obs)
t_max = model_const(1);
mu_x_0 = model_const(2);
var_x_0 = model_const(3);
var_x = model_const(4);
var_y = model_const(5);

% Kalman filter
[x_filter_bench, var_filter_bench, x_pred_KF, var_pred_KF, log_norm_const] = run_kalman_filter_1D(mu_x_0, var_x_0, t_max, zeros(1,t_max), y_obs, 1, 0, var_x, 1, var_y);
% Kalman smoother
[x_smooth_bench, var_smooth_bench] = run_kalman_smoother_1D(x_filter_bench, var_filter_bench, x_pred_KF, var_pred_KF, t_max, 1);
end


function [x_filter_bench, var_filter_bench, x_smooth_bench, var_smooth_bench, log_norm_const] = bench_hmm_1d_non_lin_gauss(model_const, y_obs, f_evolution, log_init_prior_pdf, log_prior_pdf, log_like_pdf, n_cell)
t_max = model_const(1);
mu_x_0 = model_const(2);
var_x_0 = model_const(3);
var_x = model_const(4);

% bounds computation
xcenter = zeros(t_max+1,1);
xinf = zeros(t_max+1,1);
xsup = zeros(t_max+1,1);
xdelta0 = 4*var_x_0;
xdelta = 4*var_x;

xcenter(1) = mu_x_0;
xinf(1) = xcenter(1) - xdelta0;
xsup(1) = xcenter(1) + xdelta0;
for t=1:t_max
    xcenter(t+1) = f_evolution(xcenter(t),t);
end
xinf(2:end) = xcenter(2:end) - xdelta;
xsup(2:end) = xcenter(2:end) + xdelta;

[x_filter_bench, var_filter_bench, x_smooth_bench, var_smooth_bench, log_norm_const] = run_grid_hmm_1d_non_lin(t_max, y_obs, xinf, xsup, log_init_prior_pdf, log_prior_pdf, log_like_pdf, n_cell);
end


%function plot_error_dist(errors, h_fig, i_sub, n_bin, fig_title, n_part, mutation_names, k_degree)
%
%    err_max = max(max(max(errors)));
%    delta = err_max/(n_bin-1);
%    edges = 0:delta:err_max;
%    pos = edges + delta/2;
%    
%    % filter errors distributions
%    n_N_part = size(errors, 2);
%    n_mut = size(errors, 3);
%    err_hist = zeros(n_bin, n_N_part, n_mut);
%    for j=1:n_N_part
%        for k = 1 : n_mut
%            err_hist(:,j,k) = histc(errors(:,j,k), edges);
%            err_hist(:,j,k) = err_hist(:,j,k)/ sum(err_hist(:,j,k)) / delta;
%        end
%    end
%    err_hist = reshape(err_hist, n_bin, n_N_part*n_mut);
%    
%    figure(h_fig)
%    subplot(2,1,i_sub)
%    plot(pos, err_hist)
%    title(fig_title)
%    leg_mut = zeros(n_mut*n_N_part,10);
%    for j = 1 : n_mut
%        leg_mut((j-1)*n_N_part+1:j*n_N_part,:) = repmat(sprintf('%10.10s', mutation_names{j}), n_N_part, 1);
%    end
%    leg = [repmat('N = ',n_N_part*n_mut,1) repmat(int2str(n_part'),n_mut,1), repmat(' ',n_N_part*n_mut,1), leg_mut];
%    legend(leg)
%    axis tight
%    
%    hold on
%    % chi squared curve
%    n = 50000;
%    chi_sq = sum(randn(k_degree, n).^2);
%    chi_sq_hist = histc(chi_sq, edges);
%    chi_sq_hist = chi_sq_hist/sum(chi_sq_hist) / delta;
%    plot(pos, chi_sq_hist, 'b--', 'LineWidth', 2)
%    
%    % normal approximation curve
%%     normpdf = @(x, mu, sig) 1/(sqrt(2*pi)*sig)*exp(-0.5*(x-mu).^2./sig.^2);
%%     npdf = normpdf(pos,k_degree, sqrt(2*k_degree));
%%     plot(pos, npdf, 'g--', 'LineWidth', 2)
%end


function print_results(file_name, n_part, ess_thres, model, x_gen, y_obs, log_norm_const_bench, x_filter_bench, var_filter_bench, x_smooth_bench, var_smooth_bench, errors_filter, errors_smooth)
    
    model_const = model{1};
    model_dim = model{2};
    mutation_names = model{7};
    const_names = model{9};
    var_names = model{10};
    model_id = model{11};
    
    fid = fopen(file_name, 'w');
    
    % print generic parameters
    fprintf(fid, 'model-id = %d\n', model_id);
    
    for n = n_part
        fprintf(fid, 'particles = %d\n', n);
    end
    
    fprintf(fid, 'resampling = multinomial\n');
    
    fprintf(fid, 'ess-threshold = %g\n', ess_thres);
    
    for i = 1:length(mutation_names)
        fprintf(fid, 'mutations = %s\n', mutation_names{i});
    end
    
%     fprintf(fid, 'period = %g\n', period);
    
    % print model constants
    fprintf(fid, '[model]\n');
    for i=1:length(model_const)
        fprintf(fid, '%s = %g\n', const_names{i}, model_const(i));
    end
    
    % print variable dimensions
    fprintf(fid, '[dim]\n');
    for i=1:length(model_dim)
        fprintf(fid, '%s = %g\n', var_names{i}, model_dim(i));
    end
    
    % print sampled data
    fprintf(fid, '[data]\n');
    if (size(x_gen, 1) == 1)
        fprintf(fid, '%s =', var_names{1});
        for j=1:size(x_gen,2)
            fprintf(fid, ' %g', x_gen(1,j));
        end
        fprintf(fid, '\n');
    else
        for i=1:size(x_gen,1)
            fprintf(fid, '%s.%d =', var_names{1}, i);
            for j=1:size(x_gen,2)
                fprintf(fid, ' %g', x_gen(i,j));
            end
            fprintf(fid, '\n');
        end
    end
    
    if (size(y_obs, 1) == 1)
        fprintf(fid, '%s =', var_names{2});
        for j=1:size(y_obs,2)
            fprintf(fid, ' %g', y_obs(1,j));
        end
        fprintf(fid, '\n');
    else
        for i=1:size(y_obs,1)
            fprintf(fid, '%s.%d =', var_names{2}, i);
            for j=1:size(y_obs,2)
                fprintf(fid, ' %g', y_obs(i,j));
            end
            fprintf(fid, '\n');
        end
    end
    
    % print bench normalizing constant
    fprintf(fid, '[bench]\n');
    fprintf(fid, 'log-norm-const = %g\n', log_norm_const_bench);
    
    % print bench filtering results
    fprintf(fid, '[bench.filter]\n');
    if (size(x_filter_bench, 1) == 1)
        fprintf(fid, '%s =', var_names{1});
        for j=1:size(x_filter_bench,2)
            fprintf(fid, ' %g', x_filter_bench(1,j));
        end
        fprintf(fid, '\n');
    else
        for i=1:size(x_filter_bench,1)
            fprintf(fid, '%s.%d =', var_names{1}, i);
            for j=1:size(x_filter_bench,2)
                fprintf(fid, ' %g', x_filter_bench(i,j));
            end
            fprintf(fid, '\n');
        end
    end
    
    if (size(var_filter_bench, 1) == 1)
        fprintf(fid, 'var.%s =', var_names{1});
        for j=1:size(var_filter_bench,2)
            fprintf(fid, ' %g', var_filter_bench(1,j));
        end
        fprintf(fid, '\n');
    else
        for i=1:size(var_filter_bench,1)
            fprintf(fid, 'var.%s.%d =', var_names{1}, i);
            for j=1:size(var_filter_bench,2)
                fprintf(fid, ' %g', var_filter_bench(i,j));
            end
            fprintf(fid, '\n');
        end
    end
    
    % print bench smoothing results
    fprintf(fid, '[bench.smooth]\n');
    if (size(x_smooth_bench, 1) == 1)
        fprintf(fid, '%s =', var_names{1});
        for j=1:size(x_smooth_bench,2)
            fprintf(fid, ' %g', x_smooth_bench(1,j));
        end
        fprintf(fid, '\n');
    else
        for i=1:size(x_smooth_bench,1)
            fprintf(fid, '%s.%d =', var_names{1}, i);
            for j=1:size(x_smooth_bench,2)
                fprintf(fid, ' %g', x_smooth_bench(i,j));
            end
            fprintf(fid, '\n');
        end
    end
    
    if (size(var_smooth_bench, 1) == 1)
        fprintf(fid, 'var.%s =', var_names{1});
        for j=1:size(var_smooth_bench,2)
            fprintf(fid, ' %g', var_smooth_bench(1,j));
        end
        fprintf(fid, '\n');
    else
        for i=1:size(var_smooth_bench,1)
            fprintf(fid, 'var.%s.%d =', var_names{1}, i);
            for j=1:size(var_smooth_bench,2)
                fprintf(fid, ' %g', var_smooth_bench(i,j));
            end
            fprintf(fid, '\n');
        end
    end
    
    % print smc filtering errors
    fprintf(fid, '[errors.filter]\n');
    for j = 1:length(n_part)
        for k = 1:length(mutation_names)
            fprintf(fid, '%s.%d =', mutation_names{k}, n_part(j));
            for i = 1:size(errors_filter, 1)
                fprintf(fid, ' %g', errors_filter(i,j,k));
            end
            fprintf(fid, '\n');
        end
    end
    
    % print smc smoothing errors
    fprintf(fid, '[errors.smooth]\n');
    for j = 1:length(n_part)
        for k = 1:length(mutation_names)
            fprintf(fid, '%s.%d =', mutation_names{k}, n_part(j));
            for i = 1:size(errors_smooth, 1)
                fprintf(fid, ' %g', errors_smooth(i,j,k));
            end
            fprintf(fid, '\n');
        end
    end
    
end
