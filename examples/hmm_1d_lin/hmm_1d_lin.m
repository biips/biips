%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Matlab Demo script hmm_1d_lin.m
%
% Linear and Gaussian Hidden Markov Model with known hyperparameters
% Associated BUGS file: hmm_1d_lin.bug
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
% Jan 2014; Last revision: 24-01-2014
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Add the matlab matbiips path if necessary
% matbiips_path = 'D:\Projects\biips\matbiips\matlab';
% addpath(matbiips_path)

%% ------------------------ LOAD MODEL AND DATA ------------------------ %%

%%% Model parameters
t_max = 100;
mean_x_init = 0;
prec_x_init = 1;
prec_x = 1;
prec_y = 10;
data = struct('t_max', t_max, 'prec_x_init', prec_x_init,...
    'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);

%%% Start BiiPS console
biips_init;

%%% Compile BUGS model and sample data
model = 'hmm_1d_lin.bug'; % BUGS model filename
sample_data = true; % Boolean
[model_id, data] = biips_model(model, data, 'sample_data', sample_data); % Create biips model and sample data

%% ---------------------------- BiiPS SMC  ----------------------------- %%
%%% Run BiiPS SMC and report filtering and smoothing estimates

% Parameters of the algorithm
n_part = 500; % Number of particles
variables = {'x', 'x[1:2]'}; % Variables to be monitored
type = 'fs'; rs_type = 'stratified'; rs_thres = 0.5; % Optional parameters

% Run SMC
out_smc = biips_smc_samples(model_id, variables, n_part,...
    'type', type, 'rs_type', rs_type, 'rs_thres', rs_thres);

% Diagnostic on the algorithm
diag = biips_diagnostic(out_smc);

% Get some summary statistics on the posterior distributions (mean, variance, quantiles)
summary = biips_summary(out_smc, 'probs', [.025, .975]);

%%% Some graphical outputs
% Filtering estimates
x_f_mean = summary.x.f.mean;
x_f_med = summary.x.f.med;
x_f_quant = summary.x.f.quant;
figure('name', 'SMC: Filtering estimates')
plot(x_f_mean)
hold on
plot(x_f_quant', 'r--')
xlabel('Time')
ylabel('Estimates')

% Smoothing estimates
x_s_mean = summary.x.s.mean;
x_s_quant = summary.x.f.quant;
figure('name', 'SMC: Smoothing estimates')
plot(x_s_mean)
hold on
plot(x_s_quant', 'r--')
xlabel('Time')
ylabel('Estimates')

% Marginal filtering and smoothing densities
kde_estimates = biips_density(out_smc);
time_index = [10, 50, 80, 100];
figure('name', 'SMC: Marginal posteriors')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    plot(kde_estimates.x.f(tk).x, kde_estimates.x.f(tk).f);
    hold on
    plot(kde_estimates.x.s(tk).x, kde_estimates.x.s(tk).f, 'r');
    plot(data.x_true(tk), 0, '*g');
    xlabel(['x_{' num2str(tk) '}']);
    ylabel('posterior density');
    title(['t=', num2str(tk)]);    
end
legend({'filtering density', 'smoothing density', 'True value'}, 'fontsize', 12);


%% ---------------------------- BiiPS PIMH  ---------------------------  %%
%%% Run BiiPS Particle Independent Metropolis-Hastings

% Parameters of the PIMH
n_burn = 500;
n_iter = 500;
thin = 1;
n_part = 25;

% Run PIMH
biips_pimh_update(model_id, variables, n_burn, n_part); % burn-in iterations
[out_pimh, log_marg_like_pimh] = biips_pimh_samples(model_id, variables,...
    n_iter, n_part, 'thin', thin);

% Some summary statistics
summary_pimh = biips_summary(out_pimh, 'probs', [.025, .975]);

%%% Some graphical outputs

% Posterior mean and quantiles
x_pimh_mean = summary_pimh.x.mean;
x_pimh_quant = summary_pimh.x.quant;
figure('name', 'PIMH: Posterior mean and quantiles')
plot(x_pimh_mean)
hold on
plot(x_pimh_quant', 'r--')
xlabel('Time')
ylabel('Estimates')

% Trace of MCMC samples
time_index = [10, 50, 80, 100];
figure('name', 'PIMH: Trace samples')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    plot(out_pimh.x(tk, :))
    hold on
    plot(0, data.x_true(tk), '*g');  
    xlabel('Iterations')
    ylabel('PIMH samples')
    title(['t=', num2str(tk)]);
end
legend({'PIMH samples', 'True value'});

% Histograms of posteriors
figure('name', 'PIMH: Histograms Marginal Posteriors')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    hist(out_pimh.x(tk, :), 20);
    hold on    
    plot(data.x_true(tk), 0, '*g');
    xlabel(['x_{' num2str(tk) '}']);
    ylabel('posterior density');
    title(['t=', num2str(tk)]);    
end
legend({'smoothing density', 'True value'});

% Kernel density estimates of posteriors
kde_estimates_pimh = biips_density(out_pimh);
figure('name', 'PIMH: KDE estimates Marginal posteriors')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    plot(kde_estimates_pimh.x(tk).x, kde_estimates_pimh.x(tk).f); 
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
