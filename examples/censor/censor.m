%% Example for Pierre
% Trajectory constrained on a tube 

%%%
clear all
close all

addpath('../../matbiips')

%% General settings
set(0, 'defaultaxesfontsize', 14);
set(0, 'defaultlinelinewidth', 2);
light_blue = [.7, .7, 1];
light_red = [1, .7, .7];

%%%
% Set the random numbers generator seed for reproducibility
if isoctave() || verLessThan('matlab', '7.12')
    rand('state', 0)
else
    rng('default')
end

%% Statistical model in BUGS language
% Content of the file |'censor.bug'|:
type('censor.bug');


%% Define data
x0 = 0; % initial state
tmax = 20; % time horizon
sigma = 1;
rho = .9;
interv = repmat([1, 5]', 1, tmax); % intervals bounds
% interv = [-exp(-(0:tmax-1)/tmax); exp(-(0:tmax-1)/tmax)]; % decreasing intervals

% indicator variable s.t. y[i] = { 0 if x[i] <= interv[1]
%                                { k if interv[k] < x[i] <= interv[k+1]
%                                { kmax if interv[kmax] < x[i]
% y = zeros(tmax, 1);
% y(1:10) = 0; % x in ]-inf, 1]
% y(11:20) = 1; % x in ]1, 5]
% y(21:30) = 2; % x in ]5, +inf[
% y(31:40) = 1; % x in ]1, 5]
% y(41:50) = NaN; % unobserved

y = ones(tmax, 1);

%% Compile model
model = biips_model('censor.bug', {'x0', 'tmax', 'interv', 'y', 'sigma', 'rho'});

%% Estimate x with SMC
[out_smc, log_marg_like] = biips_smc_samples(model, {'x'}, 1000);

%% 
% *Plot trajectories of the particles constrained to a tube*
figure;
plot(1:t_max, out_smc.x.s.values(:,out_smc.x.s.weights(end,:)>0));
hold on
plot(repmat((1:tmax)', 1, size(interv,1)), interv', 'k--')
ylim([0,6])
xlabel('Time')
ylabel('x')
title('particles')
box off
% saveas(gca, 'tube1', 'png')


biips_diagnosis(out_smc);

%%
% *Summary statistics*
summ = biips_summary(out_smc, 'probs', [.025,.975]);


%%
% *Plot filtering posterior density of x*
kde = biips_density(out_smc);

t_all = [5, 10, 15, 20];
figure
hold on
for k=1:numel(t_all)
    subplot(2,2,k)
    plot(kde.x.f(t_all(k)).x, kde.x.f(t_all(k)).f)
    xlabel(['x[' num2str(t_all(k)) ']'])
    ylabel('posterior density')   
    xlim([0, 6])
    box off
end
% saveas(gca, 'tube2', 'png')

%% 
% *Plot estimates of x*
x_mean = summ.x.s.mean;
x_inf = summ.x.s.quant{1};
x_sup = summ.x.s.quant{2};

figure
hold on
h = fill([1:tmax, tmax:-1:1], [x_inf; flipud(x_sup)], 0);
set(h, 'edgecolor', 'none', 'facecolor', light_blue)
plot(1:tmax, x_mean)
plot(repmat((1:tmax)', 1, size(interv,1)), interv', 'k--')
xlabel('Time')
ylabel('x')
y_lim = ylim;
legend({'95% credible interval', 'Mean estimate', 'Cutpoints'})
box off
legend boxoff
ylim([0,6])
% saveas(gca, 'tube3', 'png')