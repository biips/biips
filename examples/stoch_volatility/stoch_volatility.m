%%
% *Load and plot data*

T = readtable('SP500.csv', 'delimiter', ';');
y = diff(log(T.Close(end:-1:1)));
SP500_date_str = T.Date(end:-1:2);
% SP500_date_num = datenum(SP500_date_str);

ind = 1:200;
y = y(ind);
SP500_date_str = SP500_date_str(ind);


SP500_date_num = datenum(SP500_date_str);
figure
plot(SP500_date_num, y)
% xlim([min(SP500_date_num), max(SP500_date_num)])
datetick('x', 'mmmyyyy', 'keepticks')



%%
% *Model parameters*
t_max = length(y);
sigma = .4;
% alpha = 0.6;
alpha = 0;
beta_true = .99;
data = struct('t_max', t_max, 'sigma', sigma,...
    'alpha', alpha, 'beta_true', beta_true);

%%
% *Start BiiPS console*
biips_init;

%%
% *Compile BUGS model and sample data*
model = 'stoch_volatility.bug'; % BUGS model filename
sample_data = true;
[model_id, data] = biips_model(model, data, 'sample_data', true); % Create biips model and sample data


%% BiiPS Particle Marginal Metropolis-Hastings
% We now use BiiPS to run a Particle Marginal Metropolis-Hastings in order
% to obtain posterior MCMC samples of the parameter and variables x.

%%
% *Parameters of the PMMH*
% param_names indicates the parameters to be sampled using a random walk
% Metroplis-Hastings step. For all the other variables, biips will use a
% sequential Monte Carlo as proposal.
n_burn = 100; % nb of burn-in/adaptation iterations
n_iter = 100; % nb of iterations after burn-in
thin = 1; % thinning of MCMC outputs
n_part = 50; % nb of particles for the SMC

param_names = {'logit_beta[1:1]'}; % name of the variables updated with MCMC (others are updated with SMC)
latent_names = {'x'}; % name of the variables updated with SMC and that need to be monitored
% latent_names = {};
var_name = param_names{1};
%%
% *Init PMMH*
obj_pmmh = biips_pmmh_object(model_id, param_names, 'inits', {5}); % creates a pmmh object

%%
% *Run PMMH*
obj_pmmh = biips_pmmh_update(obj_pmmh, n_burn, n_part); % adaptation and burn-in iterations
[out_pmmh, log_post, log_marg_like, stats_pmmh] = biips_pmmh_samples(obj_pmmh, n_iter, n_part,...
    'thin', 1, 'latent_names', latent_names); % Samples
 
%%
% *Some summary statistics*
summary_pmmh = biips_summary(out_pmmh, 'probs', [.025, .975]);

%%
% *Compute kernel density estimates*
kde_estimates_pmmh = biips_density(out_pmmh);

%%
% *Posterior mean and credibilist interval for the parameter*
fprintf('Posterior mean of log_prec_y: %.1f\n',summary_pmmh.(param_names{1}).mean);
fprintf('95%% credibilist interval for log_prec_y: [%.1f,%.1f]\n',...
    summary_pmmh.(param_names{1}).quant(1),  summary_pmmh.(param_names{1}).quant(2));


%%
% *Trace of MCMC samples for the parameter*
figure('name', 'PMMH: Trace samples parameter')
plot(1./(1+exp(-out_pmmh.(var_name))))
% hold on
% plot(0, log(data.alpha_true), '*g');  
xlabel('Iterations')
ylabel('PMMH samples')
title('\beta')

%%
% *Histogram and kde estimate of the posterior for the parameter*
figure('name', 'PMMH: Histogram posterior parameter')
hist(out_pmmh.(var_name), 15)
% hold on
% plot(data.log_prec_y_true, 0, '*g');  
xlabel('log\_prec\_y')
ylabel('number of samples')
title('log\_prec\_y')

figure('name', 'PMMH: KDE estimate posterior parameter')
plot(kde_estimates_pmmh.(var_name).x, kde_estimates_pmmh.(var_name).f); 
hold on
plot(data.log_prec_y_true, 0, '*g');
xlabel('log\_prec\_y');
ylabel('posterior density');
   

%%
% *Posterior mean and quantiles for x*
x_pmmh_mean = summary_pmmh.x.mean;
x_pmmh_quant = summary_pmmh.x.quant;
figure('name', 'PMMH: Posterior mean and quantiles')
fill([1:t_max, t_max:-1:1], [x_pmmh_quant(1,:), fliplr(x_pmmh_quant(2,:))],...
    [.7 .7 1], 'edgecolor', 'none')
hold on
plot(x_pmmh_mean, 'linewidth', 3)
xlabel('Time')
ylabel('Estimates')
legend({'95 % credible interval', 'PMMH Mean Estimate'})

%%
% *Trace of MCMC samples for x*
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

%%
% *Histogram and kernel density estimate of posteriors of x*
figure('name', 'PMMH: Histograms Marginal Posteriors')
for k=1:length(time_index)
    tk = time_index(k);
    subplot(2, 2, k)
    hist(out_pmmh.x(tk, :), 15);
    hold on    
    plot(data.x_true(tk), 0, '*g');
    xlabel(['x_{' num2str(tk) '}']);
    ylabel('number of samples');
    title(['t=', num2str(tk)]);    
end
legend({'smoothing density', 'True value'});

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


%% Clear model
% 

biips_clear(model_id)