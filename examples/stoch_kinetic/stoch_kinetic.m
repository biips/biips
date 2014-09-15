%% Matbiips example: Stochastic kinetic predator-prey model
%
% Reference: A. Golightly and D. J. Wilkinson. Bayesian parameter inference
% for stochastic biochemical network models using particle Markov chain
% Monte Carlo. Interface Focus, vol.1, pp. 807-820, 2011.

%% Statistical model
%
% Let $\delta_t=1/m$ where $m$ is an integer, and $T$ a multiple of $m$.
% For $t=1,\ldots,T$
% $$ x_t|x_{t-1}\sim \mathcal N(x_{t-1}+\alpha(x_{t-1},c)\delta_t,\beta(x_{t-1},c)\delta_t) $$
%
% where $$ \alpha(x,c) = \left(
%                   \begin{array}{c}
%                     c_1x_1-c_2x_1x_2 \\
%                     c_2x_1x_2-c_3x_2 \\
%                   \end{array}
%                 \right) $$
% and $$ \beta(x,c) = \left(
%                   \begin{array}{cc}
%                     c_1x_1+c_2x_1x_2 & -c_2x_1x_2 \\
%                     -c_2x_1x_2 & c_2x_1x_2 + c_3x_2 \\
%                   \end{array}
%                 \right) $$
%
% For $t=m,2m,3m,\ldots,T$,
% $$ y_t|x_t\sim \mathcal N(x_{1t},\sigma^2) $$
%
%
% and for $i=1,\ldots,3$
%
% $$ \log(c_i)\sim Unif(-7,2) $$
%
% $x_{t1}$ and $x_{t2}$ respectively correspond to the number of preys and predators and $y_t$ is the approximated number of preys. The model is the approximation of the Lotka-Volterra model.

%% Statistical model in BUGS language
%
model_filename = 'stoch_kinetic_cle.bug'; % BUGS model filename
type(model_filename);

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
light_red = [1, .7, .7];
dark_blue = [0, 0, .5];
dark_red = [.5, 0, 0];

% Set the random numbers generator seed for reproducibility
if isoctave() || verLessThan('matlab', '7.12')
    rand('state', 0)
else
    rng('default')
end

%% Load model and data
%

%%
% *Model parameters*
t_max = 20;
dt = 0.20;
x_init_mean = [100; 100];
x_init_var = 10*eye(2);
c_true = [.5, 0.0025, .3];
prec_y = 1/10;
data = struct('t_max', t_max, 'dt', dt, 'c_true', c_true,...
    'x_init_mean', x_init_mean, 'x_init_var', x_init_var, 'prec_y', prec_y);

%%
% *Compile BUGS model and sample data*
sample_data = true; % Boolean
model = biips_model(model_filename, data, 'sample_data', sample_data); % Create Biips model and sample data
data = model.data;

%%
% *Plot data*
figure('name', 'Data')
plot(dt:dt:t_max, data.x_true(1,:))
hold on
plot(dt:dt:t_max, data.x_true(2,:), 'r')
plot(dt:dt:t_max, data.y, 'g*')
xlabel('Time')
ylabel('Number of individuals')
legend('Prey', 'Predator', 'Measurements')
box off
legend boxoff

%% Biips Sensitivity analysis with Sequential Monte Carlo
%

%%
% *Parameters of the algorithm*
n_part = 100; % Number of particles
param_names = {'logc[1]', 'logc[2]', 'logc[3]'}; % Parameter for which we want to study sensitivity
param_values = {linspace(-7,1,20), log(c_true(2))*ones(20,1), log(c_true(3))*ones(20,1)}; % Range of values

% n_grid = 5;
% [param_values{1:3}] = meshgrid(linspace(-7,1,n_grid), linspace(-7,1,n_grid), linspace(-7,1,n_grid));
% param_values = cellfun(@(x) x(:), param_values, 'uniformoutput', false);

%%
% *Run sensitivity analysis with SMC*
out = biips_smc_sensitivity(model, param_names, param_values, n_part);

%%
% *Plot penalized log-marginal likelihood*
figure('name', 'Sensitivity: Penalized log-marginal likelihood');
plot(param_values{1}, out.log_marg_like_pen, '.')
xlabel('log(c_1)')
ylabel('Penalized log-marginal likelihood')
box off


%% Biips Particle Marginal Metropolis-Hastings
% We now use Biips to run a Particle Marginal Metropolis-Hastings in order
% to obtain posterior MCMC samples of the parameters and variables $x$.

%%
% *Parameters of the PMMH*.
% param_names indicates the parameters to be sampled using a random walk
% Metroplis-Hastings step. For all the other variables, Biips will use a
% sequential Monte Carlo as proposal.
n_burn = 2000; % nb of burn-in/adaptation iterations
n_iter = 20000; % nb of iterations after burn-in
thin = 10; % thinning of MCMC outputs
n_part = 100; % nb of particles for the SMC

param_names = {'logc[1]', 'logc[2]', 'logc[3]'}; % name of the variables updated with MCMC (others are updated with SMC)
latent_names = {'x'}; % name of the variables updated with SMC and that need to be monitored

%%
% *Init PMMH*
obj_pmmh = biips_pmmh_init(model, param_names, 'inits', {-1, -6, -1},...
    'latent_names', latent_names); % creates a pmmh object

%%
% *Run PMMH*
[obj_pmmh, stats] = biips_pmmh_update(obj_pmmh, n_burn, n_part); % adaptation and burn-in iterations
[obj_pmmh, out_pmmh, log_marg_like_pen, log_marg_like, stats_pmmh] = biips_pmmh_samples(obj_pmmh, n_iter, n_part,...
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
    quantile_param = getfield(getfield(summary_pmmh,param_names{i}), 'quant');
    fprintf('Posterior mean of %s: %.1f\n', leg{i},...
        getfield(getfield(summary_pmmh, param_names{i}), 'mean'));
    fprintf('95%% credibilist interval for %s: [%.1f, %.1f]\n', leg{i},...
        quantile_param{1}, quantile_param{2});
end

%%
% *Trace of MCMC samples for the parameter*
for i=1:length(param_names)
    figure('name', 'PMMH: Trace samples parameter')
    plot(getfield(out_pmmh,param_names{i}), 'linewidth', 1);
    hold on
    plot(0, param_true(i), '*g');
    xlabel('Iterations')
    ylabel('PMMH samples')
    title(leg{i})
    box off
end

%%
% *Histogram and kde estimate of the posterior for the parameter*
for i=1:length(param_names)
    figure('name', 'PMMH: Histogram posterior parameter')
    hist(getfield(out_pmmh,param_names{i}), 15)
    h = findobj(gca, 'Type', 'patch');
    set(h, 'EdgeColor', 'w')
    hold on
    plot(param_true(i), 0, '*g');
    xlabel(leg{i})
    ylabel('Number of samples')
    title(leg{i})
    box off
end
saveas(gca, 'stoch_kinetic_param', 'epsc2')
saveas(gca, 'stoch_kinetic_param', 'png')

for i=1:length(param_names)
    kde_x = getfield(getfield(kde_estimates_pmmh, param_names{i}), 'x');
    kde_f = getfield(getfield(kde_estimates_pmmh, param_names{i}), 'f');
    figure('name', 'PMMH: KDE estimate posterior parameter')
    plot(kde_x, kde_f);
    hold on
    plot(param_true(i), 0, '*g');
    xlabel(leg{i});
    ylabel('Posterior density');
    box off
end


%%
% *Posterior mean and quantiles for $x$ *
x_pmmh_mean = summary_pmmh.x.mean;
x_pmmh_quant = summary_pmmh.x.quant;
figure('name', 'PMMH: Posterior mean and quantiles')
h = fill([1:t_max/dt, t_max/dt:-1:1], [x_pmmh_quant{1}(1,:), fliplr(x_pmmh_quant{2}(1,:))],...
    light_blue);
set(h, 'edgecolor', 'none')
hold on
plot(1:t_max/dt, x_pmmh_mean(1, :), 'linewidth', 3)
plot(1:t_max/dt, data.x_true(1,:), '--', 'color', dark_blue)
h = fill([1:t_max/dt, t_max/dt:-1:1], [x_pmmh_quant{1}(2,:), fliplr(x_pmmh_quant{2}(2,:))],...
    light_red);
set(h, 'edgecolor', 'none')
plot(1:t_max/dt, x_pmmh_mean(2, :), 'r', 'linewidth', 3)
plot(1:t_max/dt, data.x_true(2,:), '--', 'color', dark_red)
xlabel('Time')
ylabel('Estimates')
ylim([0, 1500])
legend({'95 % credible interval (prey)', 'PMMH mean estimate (prey)', 'True number of preys',...
    '95 % credible interval (predator)', 'PMMH mean estimate (predator)',...
    'True number of predators'})
legend boxoff
box off
saveas(gca, 'stoch_kinetic_x', 'epsc2')
saveas(gca, 'stoch_kinetic_x', 'png')

%% Clear model
%
biips_clear(model)
