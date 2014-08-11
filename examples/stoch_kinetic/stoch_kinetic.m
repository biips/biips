%% Matbiips example: Stochastic kinetic prey/predator model
%
%
% Reference: A. Golightly and D. J. Wilkinson. Bayesian parameter inference 
% for stochastic biochemical network models using particle Markov chain
% Monte Carlo. Interface Focus, vol.1, pp. 807-820, 2011.

%% Statistical model
%
% Let $\delta_t=1/m$ where $m$ is an integer, and $T$ a multiple of $m$.
% For $t=1,\ldots,T$
% $$ x_t|x_{t-1}\sim \mathcal N(x_{t-1}+\alpha(x_{t-1},c)\delta_t,\beta(x_{t-1},c)\delta_t)$$
% 
% where $$\alpha(x,c) = \left(
%                   \begin{array}{c}
%                     c_1x_1-c_2x_1x_2  \\
%                     c_2x_1x_2-c_3x_2 \\
%                   \end{array}
%                 \right)$$
% 
%      $$\beta(x,c) = \left(
%                   \begin{array}{cc}
%                     c_1x_1+c_2x_1x_2 & -c_2x_1x_2\\
%                     -c_2x_1x_2 & c_2x_1x_2 + c_3x_2 \\
%                   \end{array}
%                 \right)$$ 
% 
% 
% For $t=m,2m,3m,\ldots,T$, 
% $$y_t|x_t\sim \mathcal N(x_{1t},\sigma^2)$$
% 
%           
% and for $i=1,\ldots,3$
% 
% $$ \log(c_i)\sim Unif(-7,2) $$
% 
% 
% $x_{t1}$ and $x_{t2}$ respectively correspond to the number of preys and predators and $y_t$ is the approximated number of preys. The model is the approximation of the Lotka-Volterra model.



%% Statistical model in BUGS language
%
%     var x_true[2,t_max/dt],x_true_temp[2,t_max/dt],x_temp[2,t_max/dt], x[2,t_max/dt], y[t_max/dt], beta[2,2,t_max/dt], beta_true[2,2,t_max/dt], logc[3],c[3],c_true[3]
% 
%     data
%     {	
%       x_true[,1] ~ dmnormvar(x_init_mean,  x_init_var)
%       for (t in 2:t_max/dt)
%       { 
%         alpha_true[1,t] <- c_true[1] * x_true[1,t-1] - c_true[2]*x_true[1,t-1]*x_true[2,t-1]
%         alpha_true[2,t] <- c_true[2]*x_true[1,t-1]*x_true[2,t-1] - c_true[3]*x_true[2,t-1]
%         beta_true[1,1,t] <- c_true[1]*x_true[1,t-1] + c_true[2]*x_true[1,t-1]*x_true[2,t-1]
%         beta_true[1,2,t] <- -c_true[2]*x_true[1,t-1]*x_true[2,t-1]
%         beta_true[2,1,t] <- beta_true[1,2,t]
%         beta_true[2,2,t] <- c_true[2]*x_true[1,t-1]*x_true[2,t-1] + c_true[3]*x_true[2,t-1]
%         x_true_temp[,t] ~ dmnormvar(x_true[,t-1]+alpha_true[,t]*dt, (beta_true[,,t])*dt) 
%         # To avoid extinction
%         x_true[1,t] <- max(x_true_temp[1,t],1) 
%         x_true[2,t] <- max(x_true_temp[2,t],1) 
%       }
%       for (t in 1:t_max)	
%       {
%         y[t/dt] ~ dnorm(x_true[1,t/dt], prec_y) 
%       }
%     }
% 
%     model
%     {
%       logc[1] ~ dunif(-7,2)
%       logc[2] ~ dunif(-7,2)
%       logc[3] ~ dunif(-7,2)
%       c[1] <- exp(logc[1])
%       c[2] <- exp(logc[2])
%       c[3] <- exp(logc[3])
%       x[,1] ~ dmnormvar(x_init_mean,  x_init_var)
%       for (t in 2:t_max/dt)
%       { 
%         alpha[1,t] <- c[1]*x[1,t-1] - c[2]*x[1,t-1]*x[2,t-1]
%         alpha[2,t] <- c[2]*x[1,t-1]*x[2,t-1] - c[3]*x[2,t-1]
%         beta[1,1,t] <- c[1]*x[1,t-1] + c[2]*x[1,t-1]*x[2,t-1]
%         beta[1,2,t] <- -c[2]*x[1,t-1]*x[2,t-1]
%         beta[2,1,t] <- beta[1,2,t]
%         beta[2,2,t] <- c[2]*x[1,t-1]*x[2,t-1] + c[3]*x[2,t-1]
%         x_temp[,t] ~ dmnormvar(x[,t-1]+alpha[,t]*dt, beta[,,t]*dt)  
%         # To avoid extinction 
%         x[1,t] <- max(x_temp[1,t],1)
%         x[2,t] <- max(x_temp[2,t],1)
%       }
%       for (t in 1:t_max)	
%       {
%         y[t/dt] ~ dnorm(x[1,t/dt], prec_y) 
%       }
%     }

%% Installation of Matbiips
% Unzip the Matbiips archive in some folder
% and add the Matbiips folder to the Matlab path
% 

matbiips_path = '../../matbiips';
addpath(matbiips_path)

%% Load model and data
%

%%
% *Model parameters*
t_max = 20;
dt = 0.20;
x_init_mean = [100 ;100];
x_init_var = 10*eye(2);
c_true = [.5, 0.0025,.3];
prec_y = 1/10;
data = struct('t_max', t_max, 'dt', dt, 'c_true', c_true,...
    'x_init_mean', x_init_mean, 'x_init_var', x_init_var, 'prec_y', prec_y);


%%
% *Compile BUGS model and sample data*
model_filename = 'stoch_kinetic_cle.bug'; % BUGS model filename
sample_data = true; % Boolean
model = biips_model(model_filename, data, 'sample_data', sample_data); % Create biips model and sample data
data = model.data;

%%
% *Plot data*
figure('name', 'data')
plot(dt:dt:t_max, data.x_true(1,:), 'linewidth', 2)
hold on
plot(dt:dt:t_max, data.x_true(2,:), 'r', 'linewidth', 2)
hold on
plot(dt:dt:t_max, data.y, 'g*')
xlabel('Time')
ylabel('Number of individuals')
legend('Prey', 'Predator', 'Measurements')

%% BiiPS : Sensitivity analysis with Sequential Monte Carlo


%%
% *Parameters of the algorithm*. 
n_part = 100; % Number of particles
param_names = {'logc[1]','logc[2]','logc[3]'}; % Parameter for which we want to study sensitivity
param_values = {linspace(-7,1,20),log(c_true(2))*ones(20,1),log(c_true(3))*ones(20,1)}; % Range of values

% n_grid = 5;
% [param_values{1:3}] = meshgrid(linspace(-7,1,n_grid), linspace(-7,1,n_grid), linspace(-7,1,n_grid));
% param_values = cellfun(@(x) x(:), param_values, 'uniformoutput', false);

%%
% *Run sensitivity analysis with SMC*
out = biips_smc_sensitivity(model, param_names, param_values, n_part); 

%%
% *Plot penalized log-marginal likelihood*
figure('name', 'penalized log-marginal likelihood');
plot(param_values{1}, out.log_post, '.')
xlabel('log(c_1)')
ylabel('Penalized log-marginal likelihood')


%% BiiPS Particle Marginal Metropolis-Hastings
% We now use BiiPS to run a Particle Marginal Metropolis-Hastings in order
% to obtain posterior MCMC samples of the parameters and variables x.

%%
% *Parameters of the PMMH*
% param_names indicates the parameters to be sampled using a random walk
% Metroplis-Hastings step. For all the other variables, biips will use a
% sequential Monte Carlo as proposal.
n_burn = 20;%2000; % nb of burn-in/adaptation iterations
n_iter = 20;%2000; % nb of iterations after burn-in
thin = 20; % thinning of MCMC outputs
n_part = 100; % nb of particles for the SMC

param_names = {'logc[1]','logc[2]', 'logc[3]'}; % name of the variables updated with MCMC (others are updated with SMC)
latent_names = {'x'}; % name of the variables updated with SMC and that need to be monitored

%%
% *Init PMMH*
obj_pmmh = biips_pmmh_init(model, param_names, 'inits', {-1, -6, -1}...
    , 'latent_names', latent_names); % creates a pmmh object

%%
% *Run PMMH*
[obj_pmmh, stats] = biips_pmmh_update(obj_pmmh, n_burn, n_part); % adaptation and burn-in iterations
[obj_pmmh, out_pmmh, log_post, log_marg_like, stats_pmmh] = biips_pmmh_samples(obj_pmmh, n_iter, n_part,...
    'thin', 1); % Samples
 
%%
% *Some summary statistics*
summary_pmmh = biips_summary(out_pmmh, 'probs', [.025, .975]);

%%
% *Compute kernel density estimates*
kde_estimates_pmmh = biips_density(out_pmmh);

param_true = log(c_true);
leg = {'log(c_1)', 'log(c_2)', 'log(c_3)'};

%%
% *Posterior mean and credibilist interval for the parameter*
for i=1:length(param_names)
    fprintf('Posterior mean of %s: %.1f\n', leg{i}, summary_pmmh.(param_names{i}).mean);
    fprintf('95%% credibilist interval for %s: [%.1f,%.1f]\n',leg{i},...
        summary_pmmh.(param_names{1}).quant{1},  summary_pmmh.(param_names{1}).quant{2});
end



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
n_grid = fill([1:t_max/dt, t_max/dt:-1:1], [x_pmmh_quant{1}(1,:), fliplr(x_pmmh_quant{2}(1,:))],...
    [.7 .7 1]);
set(n_grid, 'edgecolor', 'none')
hold on
plot(x_pmmh_mean(1, :), 'linewidth', 3)
h2 = fill([1:t_max/dt, t_max/dt:-1:1], [x_pmmh_quant{1}(2,:), fliplr(x_pmmh_quant{2}(2,:))],...
    [1 .7 .7]);
set(h2, 'edgecolor', 'none')
plot(x_pmmh_mean(2, :),'r', 'linewidth', 3)
set(n_grid, 'edgecolor', 'none')
xlabel('Time')
ylabel('Estimates')
legend({'95 % credible interval - prey', 'PMMH Mean Estimate - prey',...
    '95 % credible interval - predator', 'PMMH Mean Estimate - predator'})
 

%% Clear model
% 

biips_clear(model)
