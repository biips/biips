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
%  \file     run_bench.m
%  \brief    
%
%  \author   $LastChangedBy$
%  \date     $LastChangedDate$
%  \version  $LastChangedRevision$
%  Id:       $Id$
%

function [errors_filter, errors_smooth, log_norm_const_bench, x_gen, y_obs, x_filter_bench, var_filter_bench, x_smooth_bench, var_smooth_bench] = run_bench(model_const, model_dim, sample_param, sample_obs, bench, smc, n_part, ess_thres, n_smc, mutation_names, period)
%RUN_BENCH Summary of this function goes here
%   Detailed explanation goes here

%% Sample parameters
x_gen = sample_param(model_const, model_dim);

%% Sample observations
y_obs = sample_obs(model_const, model_dim, x_gen);

%% Compute bench filter and smoother reference
[x_filter_bench, var_filter_bench, x_smooth_bench, var_smooth_bench, log_norm_const_bench] = bench(model_const, model_dim, y_obs);

x_filter_bench = x_filter_bench(period:period:end);
var_filter_bench = var_filter_bench(period:period:end);
x_smooth_bench = x_smooth_bench(period:period:end);
var_smooth_bench = var_smooth_bench(period:period:end);

norm_const_bench = exp(log_norm_const_bench(end));

fprintf('True log-norm-const : %g\n', log_norm_const_bench(end))

%% Run SMC filters and smoothers
errors_filter = zeros(n_smc, length(n_part), length(smc));
errors_smooth = zeros(n_smc, length(n_part), length(smc));
norm_const = zeros(1, n_smc);

fprintf('Particles | Mutation | Log-norm-const-smc | Stutent-stat\n')
fprintf('--------------------------------------------------------\n')

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
            
            x_filter_smc = x_filter_smc(period:period:end);
%             ess_filter = ess_filter(period:period:end);
            x_smooth_smc = x_smooth_smc(period:period:end);
%             ess_smooth = ess_smooth(period:period:end);
            
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
        s = sqrt(mean(norm_const.^2) - m^2);
        
        t = (m - norm_const_bench) / s * sqrt(n_smc);
        
        fprintf('%9d | %8s | %18g | %12g\n', n_part(j), mutation_names{k}, log(m), t)
    end
end

toc(t_start)
close(h)
end
