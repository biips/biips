%% Matbiips example: Stochastic kinetic prey-predator model
% Exact simulation of the Lotka-Volterra model with the Gillespie algorithm




%% Statistical model in BUGS language
%

%%
%
% 
%         EDIT MODEL

%% Installation of Matbiips
% Unzip the Matbiips archive in some folder
% and add the Matbiips folder to the Matlab path
% 

matbiips_path = '../../matbiips/matlab';
addpath(matbiips_path)

%% Load model and data
%

%%
% *Model parameters*
t_max = 20;
dt = .25;%0.20;
x_init = [100 ;100];
c = [.5,.0025,.3];
prec_y = 1/10;
data = struct('t_max', t_max, 'dt', dt, 'c',c, 'x_init', x_init, 'prec_y', prec_y);


%%
% *Start BiiPS console*
biips_init;

%%
% *Add the user-defined function 'LV'*
fun_bugs = 'LV'; fun_dim = 'lotka_volterra_dim';funeval = 'lotka_volterra_gillespie';fun_nb_inputs = 5;
biips_add_distrib_sampler(fun_bugs, fun_nb_inputs, fun_dim, funeval)


%%
% *Compile BUGS model and sample data*
model = 'stoch_kinetic_gill.bug'; % BUGS model filename
sample_data = true; % Boolean
[model_id, data] = biips_model(model, data, 'sample_data', sample_data); % Create biips model and sample data

figure('name', 'data')
plot(dt:dt:t_max, data.x_true(1,:), 'linewidth', 2)
hold on
plot(dt:dt:t_max, data.x_true(2,:), 'r', 'linewidth', 2)
hold on
plot(dt:dt:t_max, data.y, 'g*')

legend('Prey', 'Predator', 'Measurements')


%%
% *Run SMC*
n_part = 2000; % Number of particles
variables = {'x'}; % Variables to be monitored
out_smc = biips_smc_samples(model_id, variables, n_part, 'type', 'fs');

summary_smc = biips_summary(out_smc, 'probs', [.025, .975]);

%%
% *Posterior mean and quantiles for x*
x_smc_mean = summary_smc.x.s.mean;
x_smc_quant = summary_smc.x.s.quant;
figure('name', 'PMMH: Posterior mean and quantiles')
h = fill([dt:dt:t_max, t_max:-dt:dt], [squeeze(x_smc_quant(1,1, :))', fliplr(squeeze(x_smc_quant(2,1,:))')],...
    [.7 .7 1]);
set(h, 'edgecolor', 'none')
hold on
plot(dt:dt:t_max,x_smc_mean(1, :), 'linewidth', 3)
h2 = fill([dt:dt:t_max, t_max:-dt:dt], [squeeze(x_smc_quant(1,2, :))', fliplr(squeeze(x_smc_quant(2,2,:))')],...
    [1 .7 .7]);
set(h2, 'edgecolor', 'none')
plot(dt:dt:t_max,x_smc_mean(2, :),'r', 'linewidth', 3)
set(h, 'edgecolor', 'none')
xlabel('Time')
ylabel('Estimates')
legend({'95 % credible interval - prey', 'PMMH Mean Estimate - prey',...
    '95 % credible interval - predator', 'PMMH Mean Estimate - predator'})

% %% BiiPS : Sensitivity analysis with Sequential Monte Carlo
% 
% 
% %%
% % *Parameters of the algorithm*. 
% n_part = 100; % Number of particles
% param_names = {'logc[1]','logc[2]','logc[3]'}; % Parameter for which we want to study sensitivity
% param_values = {linspace(-7,1,10),log(c_true(2))*ones(10,1),log(c_true(3))*ones(10,1)}; % Range of values
% 
% %%
% % *Run sensitivity analysis with SMC*
% out = biips_smc_sensitivity(model_id, param_names, param_values, n_part); 
% % 
% %%
% % *Plot log-marginal likelihood and penalized log-marginal likelihood*
% figure('name', 'log-marginal likelihood');
% plot(param_values{1}, out.log_marg_like, '.')
% xlabel('log(\alpha)')
% ylabel('Log-marginal likelihood')
% 
% figure('name', 'penalized log-marginal likelihood');
% plot(param_values{1}, out.log_marg_like_pen, '.')
% xlabel('log(\alpha)')
% ylabel('Penalized log-marginal likelihood')
% % 
% % 
% %% BiiPS Particle Marginal Metropolis-Hastings
% % We now use BiiPS to run a Particle Marginal Metropolis-Hastings in order
% % to obtain posterior MCMC samples of the parameters and variables x.
% 
% %%
% % *Parameters of the PMMH*
% % param_names indicates the parameters to be sampled using a random walk
% % Metroplis-Hastings step. For all the other variables, biips will use a
% % sequential Monte Carlo as proposal.
% n_burn = 2000; % nb of burn-in/adaptation iterations
% n_iter = 2000; % nb of iterations after burn-in
% thin = 1; % thinning of MCMC outputs
% n_part = 50; % nb of particles for the SMC
% 
% param_names = {'logc[1]','logc[2]', 'logc[3]'}; % name of the variables updated with MCMC (others are updated with SMC)
% latent_names = {'x'}; % name of the variables updated with SMC and that need to be monitored
% 
% %%
% % *Init PMMH*
% obj_pmmh = biips_pmmh_object(model_id, param_names, 'inits', {-1, -6, -1}); % creates a pmmh object
% 
% %%
% % *Run PMMH*
% [obj_pmmh, stats] = biips_pmmh_update(obj_pmmh, n_burn, n_part); % adaptation and burn-in iterations
% [out_pmmh, log_post, log_marg_like, stats_pmmh] = biips_pmmh_samples(obj_pmmh, n_iter, n_part,...
%     'thin', 1, 'latent_names', latent_names); % Samples
%  
% %%
% % *Some summary statistics*
% summary_pmmh = biips_summary(out_pmmh, 'probs', [.025, .975,.5]);
% 
% %%
% % *Compute kernel density estimates*
% kde_estimates_pmmh = biips_density(out_pmmh);
% 
% %%
% % *Posterior mean and credibilist interval for the parameter*
% fprintf('Posterior mean of log_prec_y: %.1f\n',summary_pmmh.(param_names{1}).mean);
% fprintf('95%% credibilist interval for log_prec_y: [%.1f,%.1f]\n',...
%     summary_pmmh.(param_names{1}).quant(1),  summary_pmmh.(param_names{1}).quant(2));
% 
% 
% param_true = [log(c_true(1)), log(c_true(2)), log(c_true(3))];
% leg = {'log(c_1)', 'log(c_2)', 'log(c_3)'};
% %%
% % *Trace of MCMC samples for the parameter*
% for i=1:length(param_names)
%     figure('name', 'PMMH: Trace samples parameter')
%     plot(out_pmmh.(param_names{i}))
%     hold on
%     plot(0, param_true(i), '*g');  
%     xlabel('Iterations')
%     ylabel('PMMH samples')
%     title(leg{i})
% end
% 
% %%
% % *Histogram and kde estimate of the posterior for the parameter*
% for i=1:length(param_names)
%     figure('name', 'PMMH: Histogram posterior parameter')
%     hist(out_pmmh.(param_names{i}), 15)
%     hold on
%     plot(param_true(i), 0, '*g');  
%     xlabel(leg{i})
%     ylabel('number of samples')
%     title(leg{i})
% end
% 
% for i=1:length(param_names)
%     figure('name', 'PMMH: KDE estimate posterior parameter')
%     plot(kde_estimates_pmmh.(param_names{i}).x,...
%         kde_estimates_pmmh.(param_names{i}).f); 
%     hold on
%     plot(param_true(i), 0, '*g');  
%     xlabel(leg{i});
%     ylabel('posterior density');
% end
%    
% 
% %%
% % *Posterior mean and quantiles for x*
% x_pmmh_mean = summary_pmmh.x.mean;
% x_pmmh_quant = summary_pmmh.x.quant;
% figure('name', 'PMMH: Posterior mean and quantiles')
% h = fill([1:t_max/dt, t_max/dt:-1:1], [squeeze(x_pmmh_quant(1,1, :))', fliplr(squeeze(x_pmmh_quant(2,1,:))')],...
%     [.7 .7 1]);
% set(h, 'edgecolor', 'none')
% hold on
% plot(x_pmmh_mean(1, :), 'linewidth', 3)
% h2 = fill([1:t_max/dt, t_max/dt:-1:1], [squeeze(x_pmmh_quant(1,2, :))', fliplr(squeeze(x_pmmh_quant(2,2,:))')],...
%     [1 .7 .7]);
% set(h2, 'edgecolor', 'none')
% plot(x_pmmh_mean(2, :),'r', 'linewidth', 3)
% set(h, 'edgecolor', 'none')
% xlabel('Time')
% ylabel('Estimates')
% legend({'95 % credible interval - prey', 'PMMH Mean Estimate - prey',...
%     '95 % credible interval - predator', 'PMMH Mean Estimate - predator'})
 

%% Clear model
% 

biips_clear(model_id)
