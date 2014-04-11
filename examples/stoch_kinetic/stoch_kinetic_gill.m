%% Matbiips example: Stochastic kinetic prey-predator model
% Exact simulation of the Lotka-Volterra model with the Gillespie algorithm




%% Statistical model in BUGS language
%

%%
%
% 
%         EDIT MODEL


set(0, 'DefaultAxesFontsize', 14)
set(0, 'Defaultlinelinewidth', 2)
rng('default')

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
t_max = 40;
dt = 1;%0.20;
x_init = [100 ;100];
c = [.5,.0025,.3];
prec_y = 1/100;
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
model_filename = 'stoch_kinetic_gill.bug'; % BUGS model filename
sample_data = true; % Boolean
[model, data] = biips_model(model_filename, data, 'sample_data', sample_data); % Create biips model and sample data

% Plot data
figure('name', 'data')
plot(dt:dt:t_max, data.x_true(1,:), 'linewidth', 2)
hold on
plot(dt:dt:t_max, data.x_true(2,:), 'r', 'linewidth', 2)
hold on
plot(dt:dt:t_max, data.y, 'g*')
xlabel('Time')
ylabel('Number of individuals')
legend('Prey', 'Predator', 'Measurements')
legend('boxoff')
box off
ylim([0,500])
saveas(gca, 'kinetic_data', 'epsc2')

%%
% *Run SMC*
n_part = 10000; % Number of particles
variables = {'x'}; % Variables to be monitored
out_smc = biips_smc_samples(model, variables, n_part, 'type', 'fs');

summary_smc = biips_summary(out_smc, 'probs', [.025, .975]);


%%
% *Smoothing ESS*
figure('name', 'SESS')
semilogy(out_smc.x.s.ess(1,:))
hold on
plot(30*ones(length(out_smc.x.s.ess(1,:)),1), 'k--')
xlabel('Time')
ylabel('SESS')
ylim([1,n_part])
saveas(gca, 'kinetic_sess', 'epsc2')


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
legend({'95 % interval (prey)', 'Posterior mean (prey)',...
    '95 % interval (predator)', 'Posterior mean (predator)'})
legend('boxoff')
box off
ylim([0,500])
alpha(.7)
saveas(gca, 'kinetic_smc', 'epsc2')

%% Clear model
% 

biips_clear(model)
