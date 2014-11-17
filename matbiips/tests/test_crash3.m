% test_crash3
% PB: Lorsque l'on veut monitorer x[1:2,1], le programme crashe a la fin du
% SMC sans message d'erreur

clear
close all

%%% Model parameters
t_max = 100;
mean_x_init = 0;
prec_x_init = 1;
prec_x = 1;
prec_y = 10;
% y = randn(t_max, 1);
% x_true = randn(t_max, 1);
data = struct('t_max', t_max, 'prec_x_init', prec_x_init,...
    'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);

%%% Compile BUGS model and sample data
model_filename = 'hmm_1d_lin2.bug'; % BUGS model filename
sample_data = true; % Boolean
model = biips_model(model_filename, data, 'sample_data', sample_data); % Create biips model and sample data
data = model.data;

%% TESTS BIIPS_SMC_SAMPLES
n_part = 20; % Number of particles
variables = {'x', 'x[1:2,1]'}; % Variables to be monitored
type = 'fs'; rs_type = 'stratified'; rs_thres = 0.5; % Optional parameters
% Run SMC
out_smc = biips_smc_samples(model, variables, n_part,...
    'type', type, 'rs_type', rs_type, 'rs_thres', rs_thres);

%clear matbiips

% test_crash3
% * Parsing model in: hmm_1d_lin2.bug
% * Compiling data graph
%   Declaring variables
%   Resolving undeclared variables
%   Allocating nodes
%   Graph size: 205
%   Sampling data
%   Reading data back into data table
% * Compiling model graph
%   Declaring variables
%   Resolving undeclared variables
%   Allocating nodes
%   Graph size: 206
% * Assigning node samplers
% * Running SMC forward sampler with 20 particles
%   |--------------------------------------------------| 100%
%   |**************************************************| 100 iterations in 0.86 s
