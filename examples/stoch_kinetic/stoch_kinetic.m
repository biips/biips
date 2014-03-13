%% Matbiips example: Stochastic kinetic



%% Statistical model in BUGS language
%

%%
%
% 
%         var x_true[t_max], x[t_max], y[t_max]
% 
%         data
%         {
%           prec_y_true <- exp(log_prec_y_true)
%           x_true[1] ~ dnorm(mean_x_init, prec_x_init)
%           y[1] ~ dnorm(x_true[1]^2/20, prec_y_true)
%           for (t in 2:t_max)
%           {
%             x_true[t] ~ dnorm(0.5*x_true[t-1]+25*x_true[t-1]/(1+x_true[t-1]^2)+8*cos(1.2*(t-1)), prec_x)
%             y[t] ~ dnorm(x_true[t]^2/20, prec_y_true)
%           }
%         }
% 
%         model
%         {
%           log_prec_y ~ dunif(-3, 3)
%           prec_y <- exp(log_prec_y)
%           x[1] ~ dnorm(mean_x_init, prec_x_init)
%           y[1] ~ dnorm(x[1]^2/20, prec_y)
%           for (t in 2:t_max)
%           {
%             x[t] ~ dnorm(0.5*x[t-1]+25*x[t-1]/(1+x[t-1]^2)+8*cos(1.2*(t-1)), prec_x)
%             y[t] ~ dnorm(x[t]^2/20, prec_y)
%           }
%         }

%% Installation of Matbiips
% Unzip the Matbiips archive in some folder
% and add the Matbiips folder to the Matlab path
% 

matbiips_path = '..\..\matbiips\matlab';
addpath(matbiips_path)

%% Load model and data
%

%%
% *Model parameters*
t_max = 20;
dt = 0.20;
x_init = [100 ;100];
alpha_true = .5;
beta_true = .0025;
gamma_true = .3;
prec_y = 1/10;
data = struct('t_max', t_max, 'dt', dt, 'alpha_true',alpha_true, 'beta_true', beta_true, ...
    'gamma_true', gamma_true, 'x_init', x_init, 'prec_y', prec_y);


%%
% *Start BiiPS console*
biips_init;

%%
% *Compile BUGS model and sample data*
model = 'stoch_kinetic_cle.bug'; % BUGS model filename
sample_data = true; % Boolean
[model_id, data] = biips_model(model, data, 'sample_data', sample_data); % Create biips model and sample data

figure('name', 'data')
plot(dt:dt:t_max, data.x_true(1,:), 'linewidth', 2)
hold on
plot(dt:dt:t_max, data.x_true(2,:), 'r', 'linewidth', 2)
hold on
plot(dt:dt:t_max, data.y, 'g*')

legend('Prey', 'Predator', 'Measurements')

%% BiiPS : Sensitivity analysis with Sequential Monte Carlo


%%
% *Parameters of the algorithm*. 
n_part = 100; % Number of particles
param_names = {'logalpha[1:1]','logbeta[1:1]','loggamma[1:1]'}; % Parameter for which we want to study sensitivity
param_values = {linspace(-7,3,20),log(beta_true)*ones(20,1),log(gamma_true)*ones(20,1)}; % Range of values

%%
% *Run sensitivity analysis with SMC*
out = biips_smc_sensitivity(model_id, param_names, param_values, n_part); 

%%
% *Plot log-marginal likelihood and penalized log-marginal likelihood*
figure('name', 'log-marginal likelihood');
plot(param_values{1}, out.log_marg_like, '.')
xlabel('log(\alpha)')
ylabel('Log-marginal likelihood')

figure('name', 'penalized log-marginal likelihood');
plot(param_values{1}, out.log_marg_like_pen, '.')
xlabel('log(\alpha)')
ylabel('Penalized log-marginal likelihood')


%% BiiPS Particle Marginal Metropolis-Hastings
% We now use BiiPS to run a Particle Marginal Metropolis-Hastings in order
% to obtain posterior MCMC samples of the parameters and variables x.

%%
% *Parameters of the PMMH*
% param_names indicates the parameters to be sampled using a random walk
% Metroplis-Hastings step. For all the other variables, biips will use a
% sequential Monte Carlo as proposal.
n_burn = 10; % nb of burn-in/adaptation iterations
n_iter = 10; % nb of iterations after burn-in
thin = 2; % thinning of MCMC outputs
n_part = 100; % nb of particles for the SMC

param_names = {'logalpha[1:1]','logbeta[1:1]', 'loggamma[1:1]'}; % name of the variables updated with MCMC (others are updated with SMC)
% latent_names = {'x'}; % name of the variables updated with SMC and that need to be monitored
latent_names = {};
%%
% *Init PMMH*
obj_pmmh = biips_pmmh_object(model_id, param_names, 'inits', {-1, -6, -1}); % creates a pmmh object

%%
% *Run PMMH*
[obj_pmmh, stats] = biips_pmmh_update(obj_pmmh, n_burn, n_part); % adaptation and burn-in iterations
[out_pmmh, log_post, log_marg_like, stats_pmmh] = biips_pmmh_samples(obj_pmmh, n_iter, n_part,...
    'thin', 1, 'latent_names', latent_names); % Samples
 
%%
% *Some summary statistics*
summary_pmmh = biips_summary(out_pmmh, 'probs', [.025, .975,.5]);

%%
% *Compute kernel density estimates*
kde_estimates_pmmh = biips_density(out_pmmh);

%%
% *Posterior mean and credibilist interval for the parameter*
fprintf('Posterior mean of log_prec_y: %.1f\n',summary_pmmh.(param_names{1}).mean);
fprintf('95%% credibilist interval for log_prec_y: [%.1f,%.1f]\n',...
    summary_pmmh.(param_names{1}).quant(1),  summary_pmmh.(param_names{1}).quant(2));


param_true = [log(alpha_true), log(beta_true), log(gamma_true)];
leg = {'log(\alpha)', 'log(\beta)', 'log(\gamma)'}
%%
% *Trace of MCMC samples for the parameter*
for i=1:length(param_names)
    figure('name', 'PMMH: Trace samples parameter')
    plot(out_pmmh.(param_names{i}))
    hold on
    plot(0, param_true(i), '*g');  
    xlabel('Iterations')
    ylabel('PMMH samples')
    title(leg{i})
end

%%
% *Histogram and kde estimate of the posterior for the parameter*
for i=1:length(param_names)
    figure('name', 'PMMH: Histogram posterior parameter')
    hist(out_pmmh.(param_names{i}), 15)
    hold on
    plot(param_true(i), 0, '*g');  
    xlabel(leg{i})
    ylabel('number of samples')
    title(leg{i})
end

for i=1:length(param_names)
    figure('name', 'PMMH: KDE estimate posterior parameter')
    plot(kde_estimates_pmmh.(param_names{i}).x,...
        kde_estimates_pmmh.(param_names{i}).f); 
    hold on
    plot(param_true(i), 0, '*g');  
    xlabel(leg{i});
    ylabel('posterior density');
end
   

%%
% *Posterior mean and quantiles for x*
x_pmmh_mean = summary_pmmh.x.mean;
x_pmmh_quant = summary_pmmh.x.quant;
figure('name', 'PMMH: Posterior mean and quantiles')
fill([1:t_max/dt, t_max/dt:-1:1], [squeeze(x_pmmh_quant(1,1, :))', fliplr(squeeze(x_pmmh_quant(2,1,:))')],...
    [.7 .7 1], 'edgecolor', 'none')
hold on
plot(x_pmmh_mean(1, :), 'linewidth', 3)
xlabel('Time')
ylabel('Estimates')
legend({'95 % credible interval', 'PMMH Mean Estimate'})
 
% %%
% % *Trace of MCMC samples for x*
% time_index = [5, 10, 15, 20];
% figure('name', 'PMMH: Trace samples x')
% for k=1:length(time_index)
%     tk = time_index(k);
%     subplot(2, 2, k)
%     plot(out_pmmh.x(tk, :))
%     hold on
%     plot(0, data.x_true(tk), '*g');  
%     xlabel('Iterations')
%     ylabel('PMMH samples')
%     title(['t=', num2str(tk)]);
% end
% legend({'PMMH samples', 'True value'});
% 
% %%
% % *Histogram and kernel density estimate of posteriors of x*
% figure('name', 'PMMH: Histograms Marginal Posteriors')
% for k=1:length(time_index)
%     tk = time_index(k);
%     subplot(2, 2, k)
%     hist(out_pmmh.x(tk, :), 15);
%     hold on    
%     plot(data.x_true(tk), 0, '*g');
%     xlabel(['x_{' num2str(tk) '}']);
%     ylabel('number of samples');
%     title(['t=', num2str(tk)]);    
% end
% legend({'smoothing density', 'True value'});
% 
% figure('name', 'PMMH: KDE estimates Marginal posteriors')
% for k=1:length(time_index)
%     tk = time_index(k);
%     subplot(2, 2, k)
%     plot(kde_estimates_pmmh.x(tk).x, kde_estimates_pmmh.x(tk).f); 
%     hold on
%     plot(data.x_true(tk), 0, '*g');
%     xlabel(['x_{' num2str(tk) '}']);
%     ylabel('posterior density');
%     title(['t=', num2str(tk)]);    
% end
% legend({'posterior density', 'True value'}, 'fontsize', 12);


%% Clear model
% 

biips_clear(model_id)
