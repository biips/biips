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
% Content of the file `stoch_volatility.bug':

%%
%
%
%     var y[t_max,1], x[t_max,1], prec_y[t_max,1]
% 
%     data
%     {  
%       x_true[1,1] ~ dnorm(0, 1/sigma_true^2)
%       prec_y_true[1,1] <- exp(-x_true[1,1]) 
%       y[1,1] ~ dnorm(0, prec_y_true[1,1])
%       for (t in 2:t_max)
%       { 
%         x_true[t,1] ~ dnorm(alpha_true + beta_true*(x_true[t-1,1]-alpha_true), 1/sigma_true^2)
%         prec_y_true[t,1] <- exp(-x_true[t,1])  
%         y[t,1] ~ dnorm(0, prec_y_true[t,1])
%       }
%     }
% 
%     model
%     {
%       alpha ~ dnorm(0,10000)
%       logit_beta ~ dnorm(0,.1)
%       beta <- 1/(1+exp(-logit_beta))
%       log_sigma ~ dnorm(0, 1)
%       sigma <- exp(log_sigma)
% 
%       x[1,1] ~ dnorm(0, 1/sigma^2)
%       prec_y[1,1] <- exp(-x[1,1]) 
%       y[1,1] ~ dnorm(0, prec_y[1,1])
%       for (t in 2:t_max)
%       { 
%         x[t,1] ~ dnorm(alpha + beta*(x[t-1,1]-alpha), 1/sigma^2)
%         prec_y[t,1] <- exp(-x[t,1]) 
%         y[t,1] ~ dnorm(0, prec_y[t,1])
%       }
%     }


%% Installation of Matbiips
% Unzip the Matbiips archive in some folder
% and add the Matbiips folder to the Matlab path
% 

matbiips_path = '..\..\matbiips\matlab';
addpath(matbiips_path)

%% Load model and load or simulate data
%

sample_data = true; % Simulated data or SP500 data
t_max = 50;

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
    figure('name', 'log-returns')
    plot(SP500_date_num, y)
    datetick('x', 'mmmyyyy', 'keepticks')
    ylabel('log-returns')
end

%%
% *Model parameters*

if ~sample_data
    data = struct('t_max', t_max, 'y', y);
else
    sigma_true = .4;
    alpha_true = [0;.1];
    beta_true = .99;    
    pi = [
        .7, .3;
        .5, .5];
    data = struct('t_max', t_max, 'sigma_true', sigma_true,...
        'alp_true', alpha_true, 'beta_true', beta_true, 'pi', pi);
end


%%
% *Start BiiPS console*
biips_init;

%%
% *Compile BUGS model and sample data if simulated data*
model = 'switch_stoch_volatility.bug'; % BUGS model filename
[model_id, data] = biips_model(model, data, 'sample_data', sample_data); % Create biips model and sample data


%% BiiPS Particle Marginal Metropolis-Hastings
% We now use BiiPS to run a Particle Marginal Metropolis-Hastings in order
% to obtain posterior MCMC samples of the parameters \alpha, \beta and \sigma,
% and of the variables x.

%%
% *Parameters of the PMMH*
n_burn = 1000; % nb of burn-in/adaptation iterations
n_iter = 1000; % nb of iterations after burn-in
thin = 1; % thinning of MCMC outputs
n_part = 50; % nb of particles for the SMC

param_names = {'alp[1,1]','alp[2,1]', 'logit_beta', 'log_sigma'}; % name of the variables updated with MCMC (others are updated with SMC)
latent_names = {'x'}; % name of the variables updated with SMC and that need to be monitored

%%
% *Init PMMH*
inits = {0, .1,5,-2};
obj_pmmh = biips_pmmh_object(model_id, param_names, 'inits', inits); % creates a pmmh object
% pause
%%
% *Run PMMH*
[obj_pmmh, stats_pmmh_update] = biips_pmmh_update(obj_pmmh, n_burn, n_part); % adaptation and burn-in iterations
[out_pmmh, log_post, log_marg_like, stats_pmmh] = biips_pmmh_samples(obj_pmmh, n_iter, n_part,...
    'thin', thin, 'latent_names', latent_names); % Samples
 
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
    fprintf('Posterior mean of %s: %.3f\n',param_names{i},sum_param.mean);
    fprintf('95%% credibilist interval for %s: [%.3f,%.3f]\n',...
        param_names{i}, sum_param.quant(1),  sum_param.quant(2));
end

%%
% *Trace of MCMC samples for the parameters*
if sample_data
    param_true = [alpha_true', log(data.beta_true/(1-data.beta_true)), log(sigma_true)];
end
title_names = {'\alpha[1]','\alpha[2]', 'logit(\beta)', 'log(\sigma)'};
% figure('name', 'PMMH: Trace samples parameter')
for k=1:3
    out_pmmh_param = getfield(out_pmmh, param_names{k});
    figure
    plot(out_pmmh_param)
    if sample_data
        hold on
        plot(0, param_true(k), '*g');  
    end
    xlabel('Iterations')
    ylabel('PMMH samples')
    title(title_names{k})
end


%%
% *Histogram and kde estimate of the posterior for the parameters*
for k=1:3
    out_pmmh_param = getfield(out_pmmh, param_names{k});
    figure('name', 'PMMH: Histogram posterior parameter')
    hist(out_pmmh_param, 15)
    if sample_data
        hold on
        plot(param_true(k),0, '*g');  
    end
    xlabel(title_names{k})
    ylabel('number of samples')
    title(title_names{k})
end
  

%%
% *Posterior mean and quantiles for x*
x_pmmh_mean = summary_pmmh.x.mean;
x_pmmh_quant = summary_pmmh.x.quant;
figure('name', 'PMMH: Posterior mean and quantiles')
fill([1:t_max, t_max:-1:1], [x_pmmh_quant(1,:), fliplr(x_pmmh_quant(2,:))],...
    [.7 .7 1])
hold on
plot(x_pmmh_mean, 'linewidth', 3)
if sample_data
    plot(data.x_true, 'g', 'linewidth', 2)
    legend({'95 % credible interval', 'PMMH Mean Estimate', 'True value'})
else
    legend({'95 % credible interval', 'PMMH Mean Estimate'})
end
xlabel('Time')
ylabel('Estimates')


%%
% *Trace of MCMC samples for x*
time_index = [5, 10, 15, 20];
figure('name', 'PMMH: Trace samples x')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    plot(out_pmmh.x(tk, :))
    if sample_data
        hold on
        plot(0, data.x_true(tk), '*g');  
    end
    xlabel('Iterations')
    ylabel('PMMH samples')
    title(['t=', num2str(tk)]);
end
if sample_data
    legend({'PMMH samples', 'True value'});
end

%%
% *Histogram and kernel density estimate of posteriors of x*
figure('name', 'PMMH: Histograms Marginal Posteriors')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    hist(out_pmmh.x(tk, :), 15);
    if sample_data
        hold on    
        plot(data.x_true(tk), 0, '*g');
    end
    xlabel(['x_{' num2str(tk) '}']);
    ylabel('number of samples');
    title(['t=', num2str(tk)]);    
end
if sample_data
    legend({'smoothing density', 'True value'});
end

figure('name', 'PMMH: KDE estimates Marginal posteriors')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    plot(kde_estimates_pmmh.x(tk).x, kde_estimates_pmmh.x(tk).f); 
    if sample_data
        hold on
        plot(data.x_true(tk), 0, '*g');
    end
    xlabel(['x_{' num2str(tk) '}']);
    ylabel('posterior density');
    title(['t=', num2str(tk)]);    
end
if sample_data
    legend({'posterior density', 'True value'});
end


%% Clear model
% 

biips_clear(model_id)