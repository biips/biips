%% Matbiips example: Stochastic kinetic prey-predator model
% 
% Reference: R.J. Boys, D.J. Wilkinson and T.B.L. Kirkwood. Bayesian
% inference for a discretely observed stochastic kinetic model. Statistics
% and Computing (2008) 18:125-135.


%% Statistical model
% The continuous-time Lotka-Volterra Markov jump process describes the
% evolution of two species $X_{1}(t)$ (prey) and $X_{2}(t)$ (predator) at time $t$. Let $dt$ be an infinitesimal interval. The process evolves as
%  
% $$\Pr(X_1(t+dt)=x_1(t)+1,X_2(t+dt)=x_2(t)|x_1(t),x_2(t))=c_1x_1(t)dt+o(dt)$$
%
% $$\Pr(X_1(t+dt)=x_1(t)-1,X_2(t+dt)=x_2(t)+1|x_1(t),x_2(t))=c_2x_1(t)x_2(t)dt+o(dt)$$
%
% $$\Pr(X_1(t+dt)=x_1(t),X_2(t+dt)=x_2(t)-1|x_1(t),x_2(t))=c_3 x_2(t)dt+o(dt)$$
% 
% where $c_1=0.5$, $c_2=0.0025$ and $c_3=0.3$. Forward simulation can be done using the Gillespie algorithm. We additionally assume that we observe at some time $t=1,2,\ldots,t_{\max}$ the number of preys with some noise
%
% $$ Y(t)=X_1(t) + \epsilon(t), ~~\epsilon(t)\sim\mathcal N(0,\sigma^2) $$

%% Statistical model in BUGS language
%
% *Content of the file 'stoch_kinetic_gill.bug':*
%
%     var x_true[2,t_max], x[2,t_max], y[t_max],c[3]
% 
%     data
%     {
%       x_true[,1] ~ LV(x_init,c[1],c[2],c[3],1)
%       y[1] ~ dnorm(x_true[1,1], 1/sigma^2) 
%       for (t in 2:t_max)
%       {      
%         x_true[1:2, t] ~ LV(x_true[1:2,t-1],c[1],c[2],c[3],1)   
%         y[t] ~ dnorm(x_true[1,t], 1/sigma^2)  
%       }
%     }
% 
%     model
%     {
%       x[,1] ~ LV(x_init,c[1],c[2],c[3],1)
%       y[1] ~ dnorm(x_true[1,1], 1/sigma^2) 
%       for (t in 2:t_max)
%       {    
%         x[, t] ~ LV(x[,t-1],c[1],c[2],c[3],1) 
%         y[t] ~ dnorm(x[1,t], 1/sigma^2) 
%       }
%     }

%% User-defined Matlab functions
%
% *Content of the Matlab file `lotka_volterra_gillepsie.m':*
%
%     function x = lotka_volterra_gillespie(x, c1, c2, c3, dt)
% 
%     % Simulation from a Lotka-Volterra model with the Gillepsie algorithm
%     % x1 is the number of prey
%     % x2 is the number of predator
%     % R1: (x1,x2) -> (x1+1,x2)      At rate c1x1
%     % R2: (x1,x2) -> (x1-1,x2+1)    At rate c2x1x2
%     % R3: (x1,x2) -> (x1,x2-1)      At rate c3xx2
% 
%     z = [1, -1, 0;
%         0, 1, -1];
% 
%     t=0;
%     while 1   
%         rate = [c1*x(1), c2*x(1)*x(2), c3*x(2)];
%         sum_rate = sum(rate);
%         t = t - log(rand)/sum_rate; % Sample next event from an exponential distribution
%         ind = find((sum_rate*rand)<=cumsum(rate), 1); % Sample the type of event    
%         if t>dt
%             break
%         end
%         x = x + z(:, ind);
%     end

%%
% *Content of the Matlab file `lotka_volterra_dim.m':*
%
%     function out_dim = f_dim(x_dim, c1_dim, c2_dim, c3_dim, dt_dim)
% 
%     out_dim = [2,1];
%

%% Installation of Matbiips
% Unzip the Matbiips archive in some folder
% and add the Matbiips folder to the Matlab path
%

%%
% *Add Matbiips functions in the search path*
matbiips_path = '../../matbiips';
addpath(matbiips_path)

%% General settings
%
set(0, 'DefaultAxesFontsize', 14);
set(0, 'Defaultlinelinewidth', 2)

% Set the random numbers generator seed for reproducibility
if isoctave() || verLessThan('matlab', '7.12')
    rand ('state', 0)
else
    rng('default')
end

%% Add new sampler to BiiPS
%


%%
% *Add the user-defined function 'LV' to simulate from the Lotka-Volterra model*
fun_bugs = 'LV'; fun_dim = 'lotka_volterra_dim';funeval = 'lotka_volterra_gillespie';fun_nb_inputs = 5;
biips_add_distribution(fun_bugs, fun_nb_inputs, fun_dim, funeval)

%% Load model and data
%

%%
% *Model parameters*
t_max = 40;
x_init = [100 ;100];
c = [.5,.0025,.3];
sigma = 10;
data = struct('t_max', t_max, 'c',c, 'x_init', x_init, 'sigma', sigma);



%%
% *Compile BUGS model and sample data*
model_filename = 'stoch_kinetic_gill.bug'; % BUGS model filename
sample_data = true; % Boolean
model = biips_model(model_filename, data, 'sample_data', sample_data); % Create biips model and sample data
data = model.data;

%%
% *Plot data*
figure('name', 'data')
plot(1:t_max, data.x_true(1,:), 'linewidth', 2)
hold on
plot(1:t_max, data.x_true(2,:), 'r', 'linewidth', 2)
hold on
plot(1:t_max, data.y, 'g*')
xlabel('Time')
ylabel('Number of individuals')
legend('Prey', 'Predator', 'Measurements')
legend('boxoff')
box off
ylim([0,450])
saveas(gca, 'kinetic_data', 'epsc2')

%% BiiPS Sequential Monte Carlo algorithm
%

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
h = fill([1:t_max, t_max:-1:1], [x_smc_quant{1}(1,:), fliplr(x_smc_quant{2}(1,:))],...
    [.7 .7 1]);
set(h, 'edgecolor', 'none')
hold on
plot(1:t_max, x_smc_mean(1, :), 'linewidth', 3)
h2 = fill([1:t_max, t_max:-1:1], [x_smc_quant{1}(2,:), fliplr(x_smc_quant{2}(2,:))],...
    [1 .7 .7]);
set(h2, 'edgecolor', 'none')
plot(1:t_max, x_smc_mean(2, :),'r', 'linewidth', 3)
set(h, 'edgecolor', 'none')
xlabel('Time')
ylabel('Estimates')
legend({'95 % interval (prey)', 'Posterior mean (prey)',...
    '95 % interval (predator)', 'Posterior mean (predator)'})
legend('boxoff')
box off
ylim([0,450])
saveas(gca, 'kinetic_smc', 'epsc2')
saveas(gca, 'kinetic_smc', 'png')

%% Clear model
% 

biips_clear(model)
