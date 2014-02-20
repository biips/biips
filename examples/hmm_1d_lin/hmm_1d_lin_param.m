%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Matlab Demo script hmm_1d_lin_param.m
%
% Linear and Gaussian Hidden Markov Model with unknown hyperparameters
% Associated BUGS file: hmm_1d_lin_param.bug
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% BiiPS Project - Bayesian Inference with interacting Particle Systems
%
% Reference: A. Todeschini, M. Fuentes, F. Caron, P. Legrand, P. Del Moral.
% BiiPS: a software for Bayesian inference with interacting particle
% systems. Technical Report, INRIA. February 2014.
%
% Authors: Adrien Todeschini, Marc Fuentes
% INRIA Bordeaux, France
% email: biips-project@lists.gforge.inria.fr
% Website: https://alea.bordeaux.inria.fr/biips
% Feb 2015; Last revision: 20-02-2014
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Add the matlab matbiips path if necessary
% matbiips_path = 'D:\Projects\biips\matbiips\matlab';
% addpath(matbiips_path)

%% ------------------------ LOAD MODEL AND DATA ------------------------ %%
% rng(1)
%%% Model parameters
t_max = 20;
mean_x_init = 0;
prec_x_init = 1;
prec_x = 10; 
log_prec_y_true = log(1);
data = struct('t_max', t_max, 'prec_x_init', prec_x_init,...
    'prec_x', prec_x,  'log_prec_y_true', log_prec_y_true, 'mean_x_init', mean_x_init);

%%% Start BiiPS console
biips_init;

%%% Compile BUGS model and sample data
model = 'hmm_1d_lin_param.bug'; % BUGS model filename
sample_data = true; % Boolean
[model_id, data] = biips_model(model, data, 'sample_data', sample_data); % Create biips model and sample data
% nodes = biips_get_nodes(model_id)
% pause

% %% ---------------------------- BiiPS SMC: sensitivity analysis  ---------------------------  %%
% n_part = 50;
% param_names = {'log_prec_y[1:1]'};
% param_values = {-10:.5:10};
% out = biips_smc_sensitivity(model_id, param_names, param_values, n_part);
% 
% log_marg_like = out.log_marg_like;
% log_marg_like_pen = out.log_marg_like_pen;
% 
% 
% figure;
% plot(param_values{1}, log_marg_like)
% 
% figure;
% plot(param_values{1}, log_marg_like_pen)
% 



%% ---------------------------- BiiPS PMMH  ---------------------------  %%
%%% Run BiiPS Particle Marginal Metropolis-Hastings

% Parameters of the PMMH
n_burn = 2000;
n_iter = 2000;
thin = 1;
n_part = 50;
param_names = {'log_prec_y[1:1]'}; % name of the variables updated with mcmc (others are updated with SMC)
latent_names = {'x','x[1:2]'}; % name of the variables updated with SMC and that need to be monitored

% Run PMMH
obj_pmmh = biips_pmmh_object(model_id, param_names, 'inits', {1});
% pause
obj_pmmh = biips_pmmh_update(obj_pmmh, n_burn, n_part); % adaptation and burn-in iterations
[out_pmmh, log_post, log_marg_like, stats_pmmh] = biips_pmmh_samples(obj_pmmh, n_iter, n_part,...
    'thin', 1, 'latent_names', latent_names);

var_name = param_names{1};
figure
plot(log_post)
figure
plot(out_pmmh.(var_name), log_post, '.')
 
% Some summary statistics
summary_pmmh = biips_summary(out_pmmh, 'probs', [.025, .975]);

% Computes kde estimates
kde_estimates_pmmh = biips_density(out_pmmh);
% 
%%% Some graphical outputs


% Posterior mean and quantiles
log_prec_y_pmmh_mean = summary_pmmh.(var_name).mean
log_prec_y_pmmh_quant = summary_pmmh.(var_name).quant

% Trace of MCMC samples - parameter
figure('name', 'PMMH: Trace samples parameter')
plot(out_pmmh.(var_name))
hold on
plot(0, data.log_prec_y_true, '*g');  
xlabel('Iterations')
ylabel('PMMH samples')
title('log\_prec\_y')

% Histogram of MCMC samples - parameter
figure('name', 'PMMH: Histogram posterior parameter')
hist(out_pmmh.(var_name), 20)
hold on
plot(data.log_prec_y_true, 0, '*g');  
xlabel('Iterations')
ylabel('PMMH samples')
title('log\_prec\_y')

figure('name', 'PMMH: KDE estimate posterior parameter')
% kdre estimate of the posterior
plot(kde_estimates_pmmh.(var_name).x, kde_estimates_pmmh.(var_name).f); 
hold on
plot(data.log_prec_y_true, 0, '*g');
xlabel('log\_prec\_y');
ylabel('posterior density');
   

% Posterior mean and quantiles for x
x_pmmh_mean = summary_pmmh.x.mean;
x_pmmh_quant = summary_pmmh.x.quant;
figure('name', 'PMMH: Posterior mean and quantiles for x')
plot(x_pmmh_mean)
hold on
plot(x_pmmh_quant', 'r--')
xlabel('Time')
ylabel('Estimates')

% Trace of MCMC samples for x
time_index = [5, 10, 15, 20];
figure('name', 'PMMH: Trace samples x')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    plot(out_pmmh.x(tk, :))
    hold on
    plot(0, data.x_true(tk), '*g');  
    xlabel('Iterations')
    ylabel('PMMH samples')
    title(['t=', num2str(tk)]);
end
legend({'PMMH samples', 'True value'});

% Histograms of posteriors for x
figure('name', 'PMMH: Histograms Marginal Posteriors')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    hist(out_pmmh.x(tk, :), 20);
    hold on    
    plot(data.x_true(tk), 0, '*g');
    xlabel(['x_{' num2str(tk) '}']);
    ylabel('posterior density');
    title(['t=', num2str(tk)]);    
end
legend({'smoothing density', 'True value'});

% Kernel density estimates of posteriors

figure('name', 'PMMH: KDE estimates Marginal posteriors')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    plot(kde_estimates_pmmh.x(tk).x, kde_estimates_pmmh.x(tk).f); 
    hold on
    plot(data.x_true(tk), 0, '*g');
    xlabel(['x_{' num2str(tk) '}']);
    ylabel('posterior density');
    title(['t=', num2str(tk)]);    
end
legend({'posterior density', 'True value'}, 'fontsize', 12);




%% --------------------------------------------------------------------- %%
% Clear model 
biips_clear(model_id)
