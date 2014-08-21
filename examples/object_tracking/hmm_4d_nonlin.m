%% Matbiips example: Object tracking
% In this example, we consider the tracking of an object in 2D, observed by
% a radar.
%
% Reference: B. Ristic. Beyond the Kalman filter: Particle filters for
% Tracking applications. Artech House, 2004.

%% Statistical model
%
%
% Let $X_t$ be a 4-D vector containing the position and velocity of an
% object in 2D. We obtain distance-angular measurements $Y_t$ from a radar. 
%
% The model is defined as follows. For $t=1,\ldots,t_{\max}$
%
% $$ X_t = F X_{t-1} + G V_t,~~ V_t\sim\mathcal N(0,Q)$$
%
% $$ Y_{t} = g(X_t) + W_t,~~ W_t \sim\mathcal N(0,R)$$
%
% $F$ and $G$ are known matrices, $g(X_t)$ is the known nonlinear measurement function and $Q$ and $R$ are known covariance matrices.


%% Statistical model in BUGS language
%

%%
%
% 
%     var v_true[2,t_max-1], x_true[4,t_max], x_radar_true[2,t_max],
%     v[2,t_max-1], x[4,t_max], x_radar[2,t_max], y[2,t_max]
% 
%     data
%     {
%       x_true[,1] ~ dmnorm(mean_x_init, prec_x_init)
%       x_radar_true[,1] <- x_true[1:2,1] - x_pos 
%       mu_y_true[1,1] <- sqrt(x_radar_true[1,1]^2+x_radar_true[2,1]^2)
%       mu_y_true[2,1] <- arctan(x_radar_true[2,1]/x_radar_true[1,1])
%       y[,1] ~ dmnorm(mu_y_true[,1], prec_y)
% 
%       for (t in 2:t_max)
%       {
%         v_true[,t-1] ~ dmnorm(mean_v, prec_v)
%         x_true[,t] <- F %*% x_true[,t-1] + G %*% v_true[,t-1]
%         x_radar_true[,t] <- x_true[1:2,t] - x_pos
%         mu_y_true[1,t] <- sqrt(x_radar_true[1,t]^2+x_radar_true[2,t]^2)
%         mu_y_true[2,t] <- arctan(x_radar_true[2,t]/x_radar_true[1,t])
%         y[,t] ~ dmnorm(mu_y_true[,t], prec_y)
%       }
%     }
% 
%     model
%     {
%       x[,1] ~ dmnorm(mean_x_init, prec_x_init)
%       x_radar[,1] <- x[1:2,1] - x_pos
%       mu_y[1,1] <- sqrt(x_radar[1,1]^2+x_radar[2,1]^2)
%       mu_y[2,1] <- arctan(x_radar[2,1]/x_radar[1,1])
%       y[,1] ~ dmnorm(mu_y[,1], prec_y)
% 
%       for (t in 2:t_max)
%       {
%         v[,t-1] ~ dmnorm(mean_v, prec_v)
%         x[,t] <- F %*% x[,t-1] + G %*% v[,t-1]
%         x_radar[,t] <- x[1:2,t] - x_pos
%         mu_y[1,t] <- sqrt(x_radar[1,t]^2+x_radar[2,t]^2)
%         mu_y[2,t] <- arctan(x_radar[2,t]/x_radar[1,t])
%         y[,t] ~ dmnorm(mu_y[,t], prec_y)
%       }
%     }


%% Installation of Matbiips
% Unzip the Matbiips archive in some folder
% and add the Matbiips folder to the Matlab path
% 

matbiips_path = '../../matbiips';
addpath(matbiips_path)

%% General settings
% Set the random numbers generator seed for reproducibility
if isoctave() || verLessThan('matlab', '7.12')
    rand ('state', 0)
else
    rng('default')
end

%% Load model and data
%

%%
% *Model parameters*
t_max = 20;
mean_x_init = [0 0 1 0]';
prec_x_init = diag(1000*ones(4,1));
x_pos = [60  0];
mean_v = zeros(2, 1);
prec_v = diag(1*ones(2,1));
prec_y = diag([100 500]);
delta_t = 1;
F =[1 0 delta_t 0
    0 1 0 delta_t
    0, 0, 1, 0
    0 0 0 1];
G = [ delta_t.^2/2 0
    0 delta_t.^2/2
    delta_t 0
    0 delta_t];
data = struct('t_max', t_max, 'mean_x_init', mean_x_init, 'prec_x_init', ...
    prec_x_init, 'x_pos', x_pos, 'mean_v', mean_v, 'prec_v', prec_v,...
    'prec_y', prec_y, 'F', F, 'G', G);


%%
% *Compile BUGS model and sample data*
sample_data = true; % Boolean
model = biips_model('hmm_4d_nonlin_tracking.bug', data, 'sample_data', sample_data);
data = model.data;
x_pos_true = data.x_true(1:2,:);

%% Biips: Particle filter
%

%%
% *Parameters of the algorithm*. 
n_part = 100000; % Number of particles
variables = {'x'}; % Variables to be monitored

%%
% *Run SMC*
out_smc = biips_smc_samples(model, {'x'}, n_part);

%% 
% *Diagnostic*
diagnostic = biips_diagnosis(out_smc);

%% 
% *Summary statistics*
summary = biips_summary(out_smc, 'probs', [.025, .975]);

%% 
% *Plot estimates*
x_f_mean = summary.x.f.mean;
x_s_mean = summary.x.s.mean;
figure('name', 'Filtering and Smoothing estimates')
plot(x_f_mean(1, :), x_f_mean(2, :), 'linewidth', 2)
hold on
plot(x_s_mean(1, :), x_s_mean(2, :), '-.r', 'linewidth', 2)
plot(x_pos_true(1,:), x_pos_true(2,:), '--g', 'linewidth', 2)
plot(x_pos(1), x_pos(2), 'sk')
legend('Filtering estimate', 'Smoothing estimate', 'True trajectory',...
    'Position of the radar', 'location', 'Northwest')
legend boxoff
xlabel('Position X')
ylabel('Position Y')

figure('name', 'Particles')
plot(out_smc.x.f.values(1,:), out_smc.x.f.values(2,:), 'ro', ...
    'markersize', 3, 'markerfacecolor', 'r')
hold on
plot(x_pos_true(1,:), x_pos_true(2,:), '--g', 'linewidth', 2)
plot(x_pos(1), x_pos(2), 'sk')
legend('Particles', 'True trajectory', 'Position of the radar', 'location', 'Northwest')
legend boxoff
xlabel('Position X')
ylabel('Position Y')



%%
% *Plot Filtering estimates*
x_f_quant = summary.x.f.quant;
title_fig = {'Position X', 'Position Y', 'Velocity X', 'Velocity Y'};
for k=1:4
    figure('name', 'SMC: Filtering estimates')
    title(title_fig{k})
    hold on
    h = fill([1:t_max, t_max:-1:1], [x_f_quant{1}(k,:), fliplr(x_f_quant{2}(k,:))],...
        [.7 .7 1]);
    set(h, 'edgecolor', 'none')
    hold on
    plot(x_f_mean(k, :), 'linewidth', 3)
    hold on
    plot(data.x_true(k,:), 'g', 'linewidth', 2)
    xlabel('Time')
    ylabel('Estimates')
    legend({'95 % credible interval', 'Filtering Mean Estimate', 'True value'},...
        'location', 'Northwest')
    legend('boxoff')
    box off
end

%%
% *Plot Smoothing estimates*
x_s_quant = summary.x.s.quant;
for k=1:4
    figure('name', 'SMC: Smoothing estimates')
    title(title_fig{k})
    hold on
    h = fill([1:t_max, t_max:-1:1], [x_f_quant{1}(k,:), fliplr(x_f_quant{2}(k,:))],...
    [1, .7 .7]);
    set(h, 'edgecolor', 'none')
    hold on
    plot(x_s_mean(k, :), 'r', 'linewidth', 3)
    hold on
    plot(data.x_true(k,:), 'g', 'linewidth', 2)
    xlabel('Time')
    ylabel('Estimates')
    legend({'95 % credible interval', 'Smoothing Mean Estimate', 'True value'},...
        'location', 'Northwest')
    legend('boxoff')
    box off
end

%% Clear model
% 

biips_clear()
