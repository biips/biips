%% Matbiips example: Stochastic volatility
% In this example, we consider the stochastic volatility model SV0 for
% application e.g. in finance.
%
% Reference: S. Chib, F. Nardari, N. Shepard. Markov chain Monte Carlo methods
% for stochastic volatility models. Journal of econometrics, vol. 108, pp. 281-316, 2002.

%% Statistical model
%
% The stochastic volatility model is defined as follows
%
% $$ \alpha\sim \mathcal N (0, .0001),~~~$$
% $$ logit(\beta) \sim  \mathcal N (0, 10),~~~$$
% $$ log(\sigma) \sim  \mathcal N (0, 1)$$
%
% and for $t\leq t_{max}$
%
% $$x_t|(x_{t-1},\alpha,\beta,\sigma) \sim \mathcal N (\alpha + \beta(x_{t-1}
% -\alpha), \sigma^2)$$
%
% $$ y_t|x_t \sim \mathcal N (0, exp(x_t)) $$
%
% where $y_t$ is the response variable and $x_t$ is the unobserved
% log-volatility of $y_t$. $\mathcal N(m,\sigma^2)$ denotes the normal
% distribution of mean $m$ and variance $\sigma^2$.
%
% $\alpha$, $\beta$ and $\sigma$ are unknown
% parameters that need to be estimated.

%% Statistical model in BUGS language
% Content of the file |'stoch_volatility.bug'|:
type('stoch_volatility.bug');

%% Installation of Matbiips
% # <https://alea.bordeaux.inria.fr/biips/doku.php?id=download Download> the latest version of Matbiips
% # Unzip the archive in some folder
% # Add the Matbiips folder to the Matlab search path
matbiips_path = '../../matbiips';
addpath(matbiips_path)

%% General settings
%
set(0, 'DefaultAxesFontsize', 14);
set(0, 'Defaultlinelinewidth', 2);
light_blue = [.7, .7, 1];

% Set the random numbers generator seed for reproducibility
if isoctave() || verLessThan('matlab', '7.12')
    rand('state', 0)
else
    rng('default')
end

%% Load model and load or simulate data
%
sample_data = true; % Simulated data or SP500 data
t_max = 100;

if ~sample_data
    % Load the data
    T = readtable('SP500.csv', 'delimiter', ';');
    y = diff(log(T.Close(end:-1:1)));
    SP500_date_str = T.Date(end:-1:2);
    
    ind = 1:t_max;
    y = y(ind);
    SP500_date_str = SP500_date_str(ind);
    
    SP500_date_num = datenum(SP500_date_str);
    
    % Plot the SP500 data
    figure('name', 'Log-returns')
    plot(SP500_date_num, y)
    datetick('x', 'mmmyyyy', 'keepticks')
    ylabel('Log-returns')
end

%%
% *Model parameters*
if ~sample_data
    data = struct('t_max', t_max, 'y', y);
else
    sigma_true = .4;
    alpha_true = 0;
    beta_true = .99;
    data = struct('t_max', t_max, 'sigma_true', sigma_true,...
        'alpha_true', alpha_true, 'beta_true', beta_true);
end


%%
% *Compile BUGS model and sample data if simulated data*
model_filename = 'stoch_volatility.bug'; % BUGS model filename
model = biips_model(model_filename, data, 'sample_data', sample_data); % Create Biips model and sample data
data = model.data;

%% Biips Particle Marginal Metropolis-Hastings
% We now use Biips to run a Particle Marginal Metropolis-Hastings in order
% to obtain posterior MCMC samples of the parameters $\alpha$, $\beta$ and $\sigma$,
% and of the variables $x$.

%%
% *Parameters of the PMMH*
n_burn = 5000; % nb of burn-in/adaptation iterations
n_iter = 10000; % nb of iterations after burn-in
thin = 5; % thinning of MCMC outputs
n_part = 50; % nb of particles for the SMC

param_names = {'alpha', 'logit_beta', 'log_sigma'}; % name of the variables updated with MCMC (others are updated with SMC)
latent_names = {'x'}; % name of the variables updated with SMC and that need to be monitored

%%
% *Init PMMH*
inits = {0,5, -2};
obj_pmmh = biips_pmmh_init(model, param_names, 'inits', inits,...
    'latent_names', latent_names);

%%
% *Run PMMH*
[obj_pmmh, stats_pmmh_update] = biips_pmmh_update(obj_pmmh, n_burn, n_part); % adaptation and burn-in iterations
[obj_pmmh, out_pmmh, log_marg_like_pen, log_marg_like, stats_pmmh] = biips_pmmh_samples(obj_pmmh, n_iter, n_part,...
    'thin', thin); % Samples

%%
% *Some summary statistics*
summary_pmmh = biips_summary(out_pmmh, 'probs', [.025, .975]);

%%
% *Compute kernel density estimates*
kde_estimates_pmmh = biips_density(out_pmmh);

%%
% *Posterior mean and credibilist interval for the parameters*
for i=1:length(param_names)
    sum_param = getfield(summary_pmmh, param_names{i});
    fprintf('Posterior mean of %s: %.3f\n', param_names{i}, sum_param.mean);
    fprintf('95%% credibilist interval for %s: [%.3f, %.3f]\n',...
        param_names{i}, sum_param.quant{1}, sum_param.quant{2});
end

%%
% *Trace of MCMC samples for the parameters*
if sample_data
    param_true = [alpha_true, log(data.beta_true/(1-data.beta_true)), log(sigma_true)];
end
title_names = {'\alpha', 'logit(\beta)', 'log(\sigma)'};
for k=1:3
    out_pmmh_param = getfield(out_pmmh, param_names{k});
    figure('name', 'PMMH: Trace samples parameter')
    plot(out_pmmh_param, 'linewidth', 1)
    if sample_data
        hold on
        plot(0, param_true(k), '*g');
    end
    xlabel('Iterations')
    ylabel('PMMH samples')
    title(title_names{k})
    box off
    legend boxoff
end


%%
% *Histogram and kde estimate of the posterior for the parameters*
for k=1:3
    out_pmmh_param = getfield(out_pmmh, param_names{k});
    figure('name', 'PMMH: Histogram posterior parameter')
    hist(out_pmmh_param, 15)
    h = findobj(gca, 'Type', 'patch');
    set(h, 'EdgeColor', 'w')
    if sample_data
        hold on
        plot(param_true(k), 0, '*g');
    end
    xlabel(title_names{k})
    ylabel('Number of samples')
    title(title_names{k})
    box off
    legend boxoff
end


%%
% *Posterior mean and quantiles for $x$*
x_pmmh_mean = summary_pmmh.x.mean;
x_pmmh_quant = summary_pmmh.x.quant;
figure('name', 'PMMH: Posterior mean and quantiles')
h = fill([1:t_max, t_max:-1:1], [x_pmmh_quant{1}; flipud(x_pmmh_quant{2})],...
    light_blue);
set(h, 'edgecolor', 'none')
hold on
plot(x_pmmh_mean, 'linewidth', 3)
if sample_data
    plot(data.x_true, 'g')
    legend({'95 % credible interval', 'PMMH mean estimate', 'True value'})
else
    legend({'95 % credible interval', 'PMMH mean estimate'})
end
xlabel('Time')
ylabel('Estimates')
box off
legend boxoff


%%
% *Trace of MCMC samples for $x$*
time_index = [5, 10, 15];
figure('name', 'PMMH: Trace samples x')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    plot(out_pmmh.x(tk, :), 'linewidth', 1)
    if sample_data
        hold on
        plot(0, data.x_true(tk), '*g');
    end
    xlabel('Iterations')
    ylabel('PMMH samples')
    title(['t=', num2str(tk)]);
    box off
end
if sample_data
    h = legend({'PMMH samples', 'True value'});
    set(h, 'position', [0.7, 0.25, .1, .1])
    legend boxoff
end

%%
% *Histogram and kernel density estimate of posteriors of $x$*
figure('name', 'PMMH: Histograms marginal posteriors')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    hist(out_pmmh.x(tk, :), 15);
    h = findobj(gca, 'Type', 'patch');
    set(h, 'EdgeColor', 'w')
    if sample_data
        hold on
        plot(data.x_true(tk), 0, '*g');
    end
    xlabel(['x_{', num2str(tk), '}']);
    ylabel('number of samples');
    title(['t=', num2str(tk)]);
    box off
end
if sample_data
    h = legend({'Smoothing density', 'True value'});
    set(h, 'position', [0.7, 0.25, .1, .1])
    legend boxoff
end

figure('name', 'PMMH: KDE estimates marginal posteriors')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    plot(kde_estimates_pmmh.x(tk).x, kde_estimates_pmmh.x(tk).f);
    if sample_data
        hold on
        plot(data.x_true(tk), 0, '*g');
    end
    xlabel(['x_{', num2str(tk), '}']);
    ylabel('Posterior density');
    title(['t=', num2str(tk)]);
    box off
end
if sample_data
    h = legend({'Posterior density', 'True value'});
    set(h, 'position', [0.7, 0.25, .1, .1])
    legend boxoff
end


%% Clear model
%
biips_clear()
