%% Matbiips example: Switching Stochastic volatility with estimation of static parameters
% In this example, we consider the Markov switching stochastic volatility
% model with parameter estimation.
%
% Reference: C.M. Carvalho and H.F. Lopes. Simulation-based sequential analysis of Markov switching
% stochastic volatility models. Computational Statistics and Data analysis (2007) 4526-4542.

%% Statistical model
%
% Let $y_t$ be the response variable and $x_t$ the unobserved
% log-volatility of $y_t$. The stochastic volatility model is defined as follows
% for $t\leq t_{max}$
% 
% $$x_t|(x_{t-1},\alpha,\phi,\sigma,c_t) \sim \mathcal N (\alpha_{c_t} + \phi x_{t-1} , \sigma^2)$$
%
% $$ y_t|x_t \sim \mathcal N (0, \exp(x_t)) $$
%
% The regime variables $c_t$ follow a two-state Markov process with
% transition probabilities
%
% $$p_{ij}=Pr(c_t=j|c_{t-1}=i)$$
%
% We assume the following
% priors over the parameters $\alpha $, $\phi $, $\sigma $ and $p$:
%
% $$ \alpha_1=\gamma_1$$
%
% $$\alpha_2 = \gamma_1+\gamma_2$$
%
% $$\gamma_1 \sim \mathcal N(0,100)$$
%
% $$\gamma_2 \sim \mathcal {TN}_{(0,\infty)}(0,100)$$
%
% $$\phi \sim \mathcal {TN}_{(-1,1)}(0,100) $$
%
% $$\sigma^2 \sim invGamma(2.001,1) $$
%
% $$\pi_{11} \sim Beta(0.5,0.5)$$
%
% $$\pi_{22} \sim Beta(0.5,0.5)$$
%
% $\mathcal N(m,\sigma^2)$ denotes the normal
% distribution of mean $m$ and variance $\sigma^2$. 
% $\mathcal {TN}_{(a,b)}(m,\sigma^2)$ denotes the truncated normal
% distribution of mean $m$ and variance $\sigma^2$. 


%% Statistical model in BUGS language
% Content of the file `switch_stoch_volatility_param.bug':

%%
%
%
%         var y[t_max,1], x[t_max,1], prec_y[t_max,1],mu[t_max,1],mu_true[t_max,1],alpha[2,1],gamma[2,1],c[t_max],c_true[t_max],pi[2,2]
% 
%         data
%         {  
%           c_true[1] ~ dcat(pi_true[1,])
%           mu_true[1,1] <- alpha_true[1,1] * (c_true[1]==1) + alpha_true[2,1]*(c_true[1]==2)
%           x_true[1,1] ~ dnorm(mu_true[1,1], 1/sigma_true^2)
%           prec_y_true[1,1] <- exp(-x_true[1,1]) 
%           y[1,1] ~ dnorm(0, prec_y_true[1,1])
%           for (t in 2:t_max)
%           { 
%             c_true[t] ~ dcat(ifelse(c_true[t-1]==1,pi_true[1,],pi_true[2,]))
%             mu_true[t,1] <- alpha_true[1,1]*(c_true[t]==1) + alpha_true[2,1]*(c_true[t]==2) + phi_true*x_true[t-1,1];
%             x_true[t,1] ~ dnorm(mu_true[t,1], 1/sigma_true^2)
%             prec_y_true[t,1] <- exp(-x_true[t,1])  
%             y[t,1] ~ dnorm(0, prec_y_true[t,1])
%           }
%         }
% 
%         model
%         {
%           gamma[1,1] ~ dnorm(0, 1/100)
%           gamma[2,1] ~ dnorm(0, 1/100)T(0,)
%           alpha[1,1] <- gamma[1,1]
%           alpha[2,1] <- gamma[1,1] + gamma[2,1]
%           phi ~ dnorm(0, 1/100)T(-1,1)
%           tau ~ dgamma(2.001, 1)
%           sigma <- 1/sqrt(tau)
%           pi[1,1] ~ dbeta(.5, .5)
%           pi[1,2] <- 1.00 - pi[1,1]
%           pi[2,2] ~ dbeta(.5, .5)
%           pi[2,1] <- 1.00 - pi[2,2]
% 
% 
%           c[1] ~ dcat(pi[1,])
%           mu[1,1] <- alpha[1,1] * (c[1]==1) + alpha[2,1]*(c[1]==2)
%           x[1,1] ~ dnorm(mu[1,1], 1/sigma^2)
%           prec_y[1,1] <- exp(-x[1,1]) 
%           y[1,1] ~ dnorm(0, prec_y[1,1])
%           for (t in 2:t_max)
%           { 
%             c[t] ~ dcat(ifelse(c[t-1]==1, pi[1,], pi[2,]))
%             mu[t,1] <- alpha[1,1] * (c[t]==1) + alpha[2,1]*(c[t]==2) + phi*x[t-1,1]
%             x[t,1] ~ dnorm(mu[t,1], 1/sigma^2)
%             prec_y[t,1] <- exp(-x[t,1])  
%             y[t,1] ~ dnorm(0, prec_y[t,1])
%           }
%         }


%% Installation of Matbiips
% Unzip the Matbiips archive in some folder
% and add the Matbiips folder to the Matlab path
%

%%
% *Add Matbiips functions in the search path*
matbiips_path = '../../matbiips';
addpath(matbiips_path)

%% General settings
%
set(0, 'DefaultAxesFontsize', 14);
set(0, 'Defaultlinelinewidth', 2)

% Set the random numbers generator seed for reproducibility
if isoctave() || verLessThan('matlab', '7.12')
    rand ('state', 0)
else
    rng('default')
end

%% Load model and load or simulate data
%

sample_data = true; % Simulated data or SP500 data
t_max = 200;

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
    alpha_true = [-2.5; -1];
    phi_true = .5;    
    pi11 = .9;
    pi22 = .9;
    pi_true = [
        pi11, 1-pi11;
        1-pi22, pi22];
    
    data = struct('t_max', t_max, 'sigma_true', sigma_true,...
        'alpha_true', alpha_true, 'phi_true', phi_true, 'pi_true', pi_true);
end


%%
% *Compile BUGS model and sample data if simulated data*
model_filename = 'switch_stoch_volatility_param.bug'; % BUGS model filename
model = biips_model(model_filename, data, 'sample_data', sample_data); % Create biips model and sample data
data = model.data;

%% BiiPS Particle Marginal Metropolis-Hastings
% We now use BiiPS to run a Particle Marginal Metropolis-Hastings in order
% to obtain posterior MCMC samples of the parameters \alpha, \beta and \sigma,
% and of the variables x.

%%
% *Parameters of the PMMH*
n_burn = 2000; % nb of burn-in/adaptation iterations
n_iter = 40000; % nb of iterations after burn-in
thin = 10; % thinning of MCMC outputs
n_part = 50; % nb of particles for the SMC

param_names = {'gamma[1,1]','gamma[2,1]', 'phi', 'tau', 'pi[1,1]', 'pi[2,2]'}; % name of the variables updated with MCMC (others are updated with SMC)
latent_names = {'x','alpha[1,1]','alpha[2,1]', 'sigma'}; % name of the variables updated with SMC and that need to be monitored

%%
% *Init PMMH*
inits = {-1, 1,.5,5,.8,.8};
obj_pmmh = biips_pmmh_init(model, param_names, 'inits', inits, 'latent_names', latent_names); % creates a pmmh object
% pause
%%
% *Run PMMH*
[obj_pmmh, stats_pmmh_update] = biips_pmmh_update(obj_pmmh, n_burn, n_part); % adaptation and burn-in iterations
[obj_pmmh, out_pmmh, log_post, log_marg_like, stats_pmmh] =...
    biips_pmmh_samples(obj_pmmh, n_iter, n_part,'thin', thin); % Samples
 
%%
% *Some summary statistics*
summary_pmmh = biips_summary(out_pmmh, 'probs', [.025, .975]);

%%
% *Compute kernel density estimates*
kde_estimates_pmmh = biips_density(out_pmmh);

param_plot = {'alpha[1,1]','alpha[2,1]', 'phi', 'sigma','pi[1,1]','pi[2,2]'};
%%
% *Posterior mean and credible interval for the parameters*
for i=1:length(param_plot)
    sum_param = getfield(summary_pmmh, param_plot{i});
    fprintf('Posterior mean of %s: %.3f\n',param_plot{i},sum_param.mean);
    fprintf('95%% credibilist interval for %s: [%.3f,%.3f]\n',...
        param_plot{i}, sum_param.quant{1},  sum_param.quant{2});
end

%%
% *Trace of MCMC samples for the parameters*
if sample_data
    param_true = [alpha_true', phi_true, sigma_true, pi11, pi22];
end
title_names = {'\alpha[1]','\alpha[2]', '\phi', '\sigma','\pi[1,1]','\pi[2,2]'};

for k=1:length(param_plot)
    out_pmmh_param = getfield(out_pmmh, param_plot{k});
    figure
    plot(out_pmmh_param, 'r')
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
for k=1:length(param_plot)
    out_pmmh_param = getfield(out_pmmh, param_plot{k});
    figure('name', 'PMMH: Histogram posterior parameter')
    hist(out_pmmh_param, 15)
    h = findobj(gca,'Type','patch');
    set(h,'FaceColor','r','EdgeColor','w')
    if sample_data
        hold on
        plot(param_true(k),0, '*g');  
    end
    xlabel(title_names{k})
    ylabel('number of samples')
    saveas(gca, ['switch_stoch_param' num2str(k)], 'epsc2')
    saveas(gca, ['switch_stoch_param' num2str(k)], 'png')
    title(title_names{k})    
end
  

%%
% *Posterior mean and quantiles for x*
x_pmmh_mean = summary_pmmh.x.mean;
x_pmmh_quant = summary_pmmh.x.quant;
figure('name', 'PMMH: Posterior mean and quantiles')
h = fill([1:t_max, t_max:-1:1], [x_pmmh_quant{1}; flipud(x_pmmh_quant{2})],...
    [1 .7 .7]);
set(h, 'edgecolor', 'none')
hold on
plot(x_pmmh_mean, 'r', 'linewidth', 3)
if sample_data
    plot(data.x_true, 'g', 'linewidth', 2)
    legend({'95 % credible interval', 'PMMH Mean Estimate', 'True value'})
else
    legend({'95 % credible interval', 'PMMH Mean Estimate'})
end
xlabel('Time')
ylabel('Estimates')
saveas(gca, ['switch_stoch_param_x'], 'epsc2')
saveas(gca, ['switch_stoch_param_x'], 'png')


%%
% *Trace of MCMC samples for x*
time_index = [5, 10, 15];
figure('name', 'PMMH: Trace samples x')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    plot(out_pmmh.x(tk, :), 'r')
    if sample_data
        hold on
        plot(0, data.x_true(tk), '*g');  
    end
    xlabel('Iterations')
    ylabel('PMMH samples')
    title(['t=', num2str(tk)]);
end
if sample_data
    h = legend({'PMMH samples', 'True value'});
    set(h, 'position',[0.7, 0.25, .1, .1])
    legend('boxoff')  
end

%%
% *Histogram and kernel density estimate of posteriors of x*
figure('name', 'PMMH: Histograms Marginal Posteriors')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    hist(out_pmmh.x(tk, :), -10:.5:0);
    h = findobj(gca,'Type','patch');
    set(h,'FaceColor','r','EdgeColor','w')
    if sample_data
        hold on    
        plot(data.x_true(tk), 0, '*g');
    end
    xlim([-10,0])
    xlabel(['x_{' num2str(tk) '}']);
    ylabel('number of samples');
    title(['t=', num2str(tk)]);    
end
if sample_data
    h = legend({'Posterior samples', 'True value'});
    set(h, 'position',[0.7, 0.25, .1, .1])
    legend('boxoff')  
end

figure('name', 'PMMH: KDE estimates Marginal posteriors')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    plot(kde_estimates_pmmh.x(tk).x, kde_estimates_pmmh.x(tk).f, 'r'); 
    if sample_data
        hold on
        plot(data.x_true(tk), 0, '*g');
    end
    xlim([-10,0])
    xlabel(['x_{' num2str(tk) '}']);
    ylabel('posterior density');
    title(['t=', num2str(tk)]);    
end
if sample_data
    h = legend({'Posterior density', 'True value'});
    set(h, 'position',[0.7, 0.25, .1, .1])
    legend('boxoff')    
end


%% Clear model
% 

biips_clear()
