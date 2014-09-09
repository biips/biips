clear all
close all

addpath('../matbiips')

%% General settings
%
set(0, 'defaultaxesfontsize', 14);
set(0, 'defaultlinelinewidth', 2);
light_blue = [.7, .7, 1];
light_red = [1, .7, .7];

% Set the random numbers generator seed for reproducibility
if isoctave() || verLessThan('matlab', '7.12')
    rand('state', 0)
else
    rng('default')
end


%% Define data
x0 = 0; % initial state
tmax = 50; % time horizon
interv = repmat([1, 5]', 1, tmax); % intervals bounds

% indicator variable s.t. y[i] = { 0 if x[i] <= interv[1]
%                                { k if interv[k] < x[i] <= interv[k+1]
%                                { kmax if interv[kmax] < x[i]
y = zeros(tmax, 1);
y(1:10) = 0; % x in ]-inf, 1]
y(11:20) = 1; % x in ]1, 5]
y(21:30) = 2; % x in ]5, +inf[
y(31:40) = 1; % x in ]1, 5]
y(41:50) = NaN; % unobserved

% interv = [-exp(-(0:tmax-1)/tmax); exp(-(0:tmax-1)/tmax)]; % intervals bounds
% y = ones(tmax, 1);

%% Compile model
model = biips_model('censor.bug', {'x0', 'tmax', 'interv', 'y'});

%% Estimate tau and x with SMC
[out_smc, log_marg_like] = biips_smc_samples(model, {'sigma', 'x'}, 10000);

biips_diagnosis(out_smc);

%%
% Summary statistics
summ = biips_summary(out_smc, 'probs', [.025,.975]);


fprintf('* Summary statistics\n');
fprintf('Mean estimate of sigma: %g\n', summ.sigma.s.mean);
fprintf('95%% credible interval of sigma: [%g, %g]\n', summ.sigma.s.quant{1}, summ.sigma.s.quant{2});

%%
% Plot posterior density of sigma
kde = biips_density(out_smc, 'variable_names', {'sigma'});

figure
hold on
plot(kde.sigma.s.x, kde.sigma.s.f)
xlabel('sigma')
ylabel('posterior density')

%% 
% Plot estimates of x
x_mean = summ.x.s.mean;
x_inf = summ.x.s.quant{1};
x_sup = summ.x.s.quant{2};

figure
hold on
h = fill([1:tmax, tmax:-1:1], [x_inf; flipud(x_sup)], light_blue);
set(h, 'edgecolor', 'none')
plot(1:tmax, x_mean)
plot(repmat((1:tmax)', 1, size(interv,1)), interv', 'k--')
xlabel('time')
ylabel('x')
y_lim = ylim;
legend({'95 % credible interval', 'Mean estimate', 'Cutpoints'})
box off
