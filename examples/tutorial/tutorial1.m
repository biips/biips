%% Matbiips: Tutorial 1
% In this tutorial, we consider applying sequential Monte Carlo methods for
% Bayesian inference in a nonlinear non-Gaussian hidden Markov model.

%% Statistical model
% The statistical model is defined as follows.
%
% $$ x_1\sim \mathcal N\left (\mu_0, \frac{1}{\lambda_0}\right )$$
%
% $$ y_1\sim \mathcal N\left (h(x_1), \frac{1}{\lambda_y}\right )$$
%
% For $t=2:t_{max}$
%
% $$ x_t|x_{t-1} \sim \mathcal N\left ( f(x_{t-1},t-1), \frac{1}{\lambda_x}\right )$$
%
% $$ y_t|x_t \sim \mathcal N\left ( h(x_{t}), \frac{1}{\lambda_y}\right )$$
%
% where $\mathcal N\left (m, S\right )$ denotes the Gaussian distribution
% of mean $m$ and covariance matrix $S$, $h(x)=x^2/20$, $f(x,t-1)=0.5 x+25 x/(1+x^2)+8 \cos(1.2 (t-1))$, $\mu_0=0$, $\lambda_0 = 5$, $\lambda_x = 0.1$ and $\lambda_y=1$.

%% Statistical model in BUGS language
% We describe the model in BUGS language in the file |'hmm_1d_nonlin.bug'|:
model_file = 'hmm_1d_nonlin.bug'; % BUGS model filename
type(model_file);

%% Installation of Matbiips
% # <https://alea.bordeaux.inria.fr/biips/doku.php?id=download Download> the latest version of Matbiips
% # Unzip the archive in some folder
% # Add the Matbiips folder to the Matlab search path
matbiips_path = '../../matbiips';
addpath(matbiips_path)

%% General settings
%
set(0, 'defaultaxesfontsize', 14);
set(0, 'defaultlinelinewidth', 2);
light_blue = [.7, .7, 1];
light_red = [1, .7, .7];

%%%
% Set the random numbers generator seed for reproducibility
if isoctave() || verLessThan('matlab', '7.12')
    rand('state', 0)
else
    rng('default')
end

%% Load model and data
%
% *Model parameters*
t_max = 20;
mean_x_init = 0;
prec_x_init = 1/5;
prec_x = 1/10;
prec_y = 1;
data = struct('t_max', t_max, 'prec_x_init', prec_x_init,...
    'prec_x', prec_x, 'prec_y', prec_y, 'mean_x_init', mean_x_init);


%%
% *Compile BUGS model and sample data*
sample_data = true; % Boolean
model = biips_model(model_file, data, 'sample_data', sample_data); % Create Biips model and sample data
data = model.data;

%% Biips Sequential Monte Carlo
% Let now use Biips to run a particle filter.

%%%
% *Parameters of the algorithm*.
% 
% We want to monitor the variable |x|, and to
% get the filtering and smoothing particle approximations. The algorithm
% will use 10000 particles, stratified resampling, with a threshold of 0.5.
n_part = 10000; % Number of particles
variables = {'x'}; % Variables to be monitored
mn_type = 'fs'; rs_type = 'stratified'; rs_thres = 0.5; % Optional parameters

%%
% *Run SMC*
out_smc = biips_smc_samples(model, variables, n_part,...
    'type', mn_type, 'rs_type', rs_type, 'rs_thres', rs_thres);

%%
% *Diagnosis of the algorithm*
diag_smc = biips_diagnosis(out_smc);

%%
% The sequence of filtering distributions is automatically chosen by Biips
% based on the topology of the graphical model, and is returned in the
% subfield |f.conditionals|. For this particular example, the sequence of
% filtering distributions is $\pi(x_{t}|y_{1:t})$, for $t=1,\ldots,t_{max}$.

fprintf('Filtering distributions:\n')
for i=1:numel(out_smc.x.f.conditionals)
    fprintf('%i: x[%i] | ', out_smc.x.f.iterations(i), i);
    fprintf('%s,', out_smc.x.f.conditionals{i}{1:end-1});
    fprintf('%s', out_smc.x.f.conditionals{i}{end});
    fprintf('\n')
end

%%
% while the smoothing distributions are $\pi(x_{t}|y_{1:t_{max}})$, for $t=1,\ldots,t_{max}$.

fprintf('Smoothing distributions:\n')
for i=1:numel(out_smc.x.s.conditionals)
    fprintf('x[%i] | ', i);
    fprintf('%s,', out_smc.x.s.conditionals{1:end-1});
    fprintf('%s', out_smc.x.s.conditionals{end});
    fprintf('\n')
end

%%
% *Summary statistics*
summ_smc = biips_summary(out_smc, 'probs', [.025, .975]);

%%
% *Plot Filtering estimates*
figure('name', 'SMC: Filtering estimates')
x_f_mean = summ_smc.x.f.mean;
x_f_quant = summ_smc.x.f.quant;
h = fill([1:t_max, t_max:-1:1], [x_f_quant{1}; flipud(x_f_quant{2})],...
    light_blue);
set(h, 'edgecolor', 'none')
hold on
plot(1:t_max, x_f_mean, 'linewidth', 3)
plot(1:t_max, data.x_true, 'g')
xlabel('Time')
ylabel('x')
legend({'95% credible interval', 'Filtering mean estimate', 'True value'})
legend boxoff
box off

%%
% *Plot Smoothing estimates*
figure('name', 'SMC: Smoothing estimates')
x_s_mean = summ_smc.x.s.mean;
x_s_quant = summ_smc.x.s.quant;
h = fill([1:t_max, t_max:-1:1], [x_s_quant{1}; flipud(x_s_quant{2})],...
    light_red);
set(h, 'edgecolor', 'none')
hold on
plot(1:t_max, x_s_mean, 'r', 'linewidth', 3)
plot(1:t_max, data.x_true, 'g')
xlabel('Time')
ylabel('x')
legend({'95% credible interval', 'Smoothing mean estimate', 'True value'})
legend boxoff
box off

%%
% *Marginal filtering and smoothing densities*
figure('name', 'SMC: Marginal posteriors')
kde_smc = biips_density(out_smc);
time_index = [5, 10, 15];
for k=1:numel(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    plot(kde_smc.x.f(tk).x, kde_smc.x.f(tk).f);
    hold on
    plot(kde_smc.x.s(tk).x, kde_smc.x.s(tk).f, 'r');
    plot(data.x_true(tk), 0, '*g');
    xlabel(['x_{', num2str(tk), '}']);
    ylabel('Posterior density');
    title(['t=', num2str(tk)]);
    box off
end
h = legend({'Filtering density', 'Smoothing density', 'True value'});
set(h, 'position', [0.7, 0.25, .1, .1])
legend boxoff


%% Biips Particle Independent Metropolis-Hastings
% We now use Biips to run a Particle Independent Metropolis-Hastings.

%%%
% *Parameters of the PIMH*
n_burn = 500;
n_iter = 500;
thin = 1;
n_part = 100;

%%
% *Run PIMH*
obj_pimh = biips_pimh_init(model, variables);
obj_pimh = biips_pimh_update(obj_pimh, n_burn, n_part); % burn-in iterations
[obj_pimh, samples_pimh, log_marg_like_pimh] = biips_pimh_samples(obj_pimh,...
    n_iter, n_part, 'thin', thin);

%%
% *Some summary statistics*
summ_pimh = biips_summary(samples_pimh, 'probs', [.025, .975]);

%%
% *Posterior mean and quantiles*
figure('name', 'PIMH: Posterior mean and quantiles')
x_pimh_mean = summ_pimh.x.mean;
x_pimh_quant = summ_pimh.x.quant;
h = fill([1:t_max, t_max:-1:1], [x_pimh_quant{1}; flipud(x_pimh_quant{2})],...
    light_blue);
set(h, 'edgecolor', 'none')
hold on
plot(1:t_max, x_pimh_mean, 'linewidth', 3)
plot(1:t_max, data.x_true, 'g')
xlabel('Time')
ylabel('x')
legend({'95% credible interval', 'PIMH mean estimate', 'True value'})
legend boxoff
box off

%%
% *Trace of MCMC samples*
figure('name', 'PIMH: Trace samples')
time_index = [5, 10, 15];
for k=1:numel(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    plot(samples_pimh.x(tk, :), 'linewidth', 1)
    hold on
    plot(0, data.x_true(tk), '*g');
    xlabel('Iteration')
    ylabel(['x_{', num2str(tk), '}'])
    title(['t=', num2str(tk)]);
    box off
end
h = legend({'PIMH samples', 'True value'});
set(h, 'position', [0.7, 0.25, .1, .1])
legend boxoff

%%
% *Histograms of posteriors*
figure('name', 'PIMH: Histograms marginal posteriors')
for k=1:numel(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    hist(samples_pimh.x(tk, :), -15:1:15);
    h = findobj(gca, 'Type', 'patch');
    set(h, 'EdgeColor', 'w')
    hold on
    plot(data.x_true(tk), 0, '*g');
    xlabel(['x_{', num2str(tk), '}']);
    ylabel('Number of samples');
    title(['t=', num2str(tk)]);
    box off
end
h = legend({'Posterior density', 'True value'});
set(h, 'position', [0.7, 0.25, .1, .1])
legend boxoff

%%
% *Kernel density estimates of posteriors*
figure('name', 'PIMH: KDE estimates marginal posteriors')
kde_pimh = biips_density(samples_pimh);
for k=1:numel(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    plot(kde_pimh.x(tk).x, kde_pimh.x(tk).f);
    hold on
    plot(data.x_true(tk), 0, '*g');
    xlabel(['x_{', num2str(tk), '}']);
    ylabel('Posterior density');
    title(['t=', num2str(tk)]);
    box off
end
h = legend({'Posterior density', 'True value'});
set(h, 'position', [0.7, 0.25, .1, .1])
legend boxoff

%% Clear model
%
biips_clear()
