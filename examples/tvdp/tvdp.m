%% Matbiips example: Dynamic density estimation
% In this example, we consider the (parametric version) of the dynamic
% Chinese restaurant process derived in the reference below.
%
% Reference: F. Caron, M. Davy, A. Doucet. Generalized Polya Urn for
% Time-Varying Dirichlet Process Mixtures. Uncertainty in Artificial Intelligence, 2007.
%
% <http://arxiv.org/ftp/arxiv/papers/1206/1206.5254.pdf>
%
% Note: this example requires the |normpdf| function present in the Matlab
% Statistical Toolbox

%% Statistical model in BUGS language
% *Content of the file |'tvdp.bug'|:*
model_filename = 'tvdp.bug'; % BUGS model filename
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

% Set the random numbers generator seed for reproducibility
if isoctave() || verLessThan('matlab', '7.12')
    rand('state', 0)
else
    rng('default')
end

%% Load model and sample data
% Model parameters
clust_max = 10;
t_max = 1000;

mu_0 = 0;
prec_0 = 1;
prec_y = 10;
alpha = 3;
rho = .99;
gamma = .99;

%%
% *Sample and plot data*
y = zeros(t_max, 1);
y(1:300) = sqrt(1/prec_y)*randn(300, 1) + 2*(rand(300, 1)>0.4) -1;
u = rand(300, 1);
y(301:600) = sqrt(1/prec_y)*randn(300, 1) + 3*(u>0.25) + 3*(u>0.75)  -3;
mov_mean = linspace(0, 3, 400);
y(601:1000) = sqrt(1/prec_y)*randn(400, 1) + repmat(mov_mean', 1, 1);
figure
plot(y, '*g')
xlabel('Time')
ylabel('y_t')
box off

%%
% *Compile BUGS model*
data = {'rho', 'gamma', 't_max', 'clust_max', 'y', 'mu_0', 'prec_0', 'prec_y', 'alpha'};
model = biips_model(model_filename, data); % Create Biips model
data = model.data;

%% Biips Sequential Monte Carlo
%

%%
% *Parameters of the SMC algorithm*
n_part = 1000; % Number of particles
variables = {'c', 'mu', 'm'}; % Variables to be monitored

%%
% *Run SMC*
out_smc = biips_smc_samples(model, variables, n_part);

%% Plot the true and estimated densities over time
%
stepsize = 20;
n0 = 200;
x0 = linspace(-5,5,n0);
out = zeros(t_max, n0);
out_true = zeros(t_max, n0);
for t=stepsize:stepsize:t_max
    for i=1:n_part
        c_i = logical(out_smc.c.f.values(t, :, i));
        mu_i = out_smc.mu.f.values(t, :, i);
        m_i =   out_smc.m.f.values(t, :, i);
        w_i = out_smc.mu.f.weights(t,1,i);
        out(t, :) = out(t, :) ...
            + w_i*sum( repmat(m_i', 1, n0).*normpdf(repmat(x0, clust_max, 1),...
            repmat(mu_i', 1, n0), 1/sqrt(prec_y)))/sum(m_i);
    end
    if t<301
        out_true(t,:) = .5*normpdf(x0, 1, 1/sqrt(prec_y)) + .5*normpdf(x0, -1, 1/sqrt(prec_y));
    elseif (t>300 && t<=600)
        out_true(t,:) = .25*normpdf(x0, -3, 1/sqrt(prec_y)) ...
            + .25*normpdf(x0, 3, 1/sqrt(prec_y)) + .5*normpdf(x0, 0, 1/sqrt(prec_y));
    else
        out_true(t,:) = normpdf(x0, mov_mean(t-600), 1/sqrt(prec_y));
    end
    
end

% True density
figure
hold on
for t=stepsize:stepsize:t_max
    line(t*ones(numel(x0),1), x0, out_true(t, :), 'color', 'k')
end
view(-70, 52)
xlabel('Time')
ylabel('y')
zlabel('$F_t(y)$', 'interpreter', 'latex')
title('True density')
box off

% Estimated density
figure
hold on
for t=stepsize:stepsize:t_max
    line(t*ones(numel(x0),1), x0, out(t, :), 'color', 'k')
end
view(-70, 52)
xlabel('Time')
ylabel('y')
zlabel('$\widehat F_t(y)$', 'interpreter', 'latex')
title('Estimated density')
box off

%% Clear model
%
biips_clear()