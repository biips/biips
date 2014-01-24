%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Matlab Demo script hmm_1d_lin.m
%
% Linear and Gaussian Hidden Markov Model with known hyperparameters
% Associated BUGS file: hmm_1d_lin.bug
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% BiiPS Project - Bayesian Inference with interacting Particle Systems
%
% Reference: A. Todeschini, M. Fuentes, F. Caron, P. Legrand, P. Del Moral.
% BiiPS: a software for Bayesian inference with interacting particle
% systems. Technical Report, February 2014.
%
% Authors: Adrien Todeschini, Marc Fuentes
% INRIA Bordeaux, France
% email: biips-project@lists.gforge.inria.fr
% Website: https://alea.bordeaux.inria.fr/biips
% Jan 2014; Last revision: 24-01-2014
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%% Model parameters
t_max = 100;
prec_x_init = 1;
mean_x_init = 0;
prec_x = 1;
prec_y = 10;
data = struct('t_max', t_max, 'prec_x_init', prec_x_init, 'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);

%% Start BiiPS console
biips_init;

%% Load model and sample data
BUGS_model = 'hmm_1d_lin.bug'; % BUGS model file
[p, data] = biips_model('hmm_1d_lin.bug', data);

%% Run a sequential Monte Carlo algorithm
% Parameters of the algorithm
npart = 100; % Number of particles
variables = 'x'; % Variables to be monitored
% Run SMC
% Short version with default values
out_smc = biips_smc_samples(p, {variables}, npart);
% Long version with more options
% type = 'fs'; rs_type = 'stratified'; rs_thres = 0.5;
% out_smc = biips_smc_samples(p, {variables}, npart, 'type', type, 'rs_type', rs_type, 'rs_thres', rs_thres);

%% Some graphical outputs
summary = biips_summary(out_smc);

x_f_mean = summary.x.f.mean;
x_s_mean = summary.x.s.mean;
% x_f_var = summary.x.f.var;
figure('name', 'Filtering mean value of x')
plot(x_f_mean)
hold on
plot(x_s_mean, 'g')
% hold on
plot(data.y, '.r')

% console cleaning 
inter_biips('clear_console', p); 

