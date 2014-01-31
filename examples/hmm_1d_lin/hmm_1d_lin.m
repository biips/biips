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
npart = 100000; % Number of particles
variables = {'x'}; % Variables to be monitored
type = 'fs'; rs_type = 'stratified'; rs_thres = 0.5; % Optional parameters

% Run SMC
out_smc = biips_smc_samples(model_id, variables, npart,...
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
figure('name', 'Filtering')
plot(x_f_mean)
hold on
plot(x_f_quant', 'r--')
xlabel('Time')
ylabel('Estimates')

% Smoothing estimates
x_s_mean = summary.x.s.mean;
x_s_quant = summary.x.f.quant;
figure('name', 'Smoothing')
plot(x_s_mean)
hold on
plot(x_s_quant', 'r--')
xlabel('Time')
ylabel('Estimates')


% Marginal filtering and smoothing densities
kde_estimates = biips_density(out_smc);

time_index = [10, 50, 80, 100];
figure('name', 'Marginal posteriors')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    plot(kde_estimates.x.f(tk).x, kde_estimates.x.f(tk).f);
    hold on
    plot(kde_estimates.x.s(tk).x, kde_estimates.x.s(tk).f, 'r');
    plot(data.x_true(tk), 0, '*g');
    xlabel(['x_{' num2str(tk) '}'], 'fontsize', 16);
    ylabel('posterior density', 'fontsize', 16);
    title(['t=', num2str(tk)], 'fontsize', 16);
    legend({'filtering density', 'smoothing density', 'True value'}, 'fontsize', 12);
end


%% ---------------------------- BiiPS PIMH  ---------------------------  %%
%%% Run BiiPS Particle Independent Metropolis-Hastings



%%% Some graphical outputs

%% --------------------------------------------------------------------- %%
% Clear model 
biips_clear(model_id)
