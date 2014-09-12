% [SOLVED] test_crash2
% PB: Pb avec les dimensions de y

clear
close all

%%% Model parameters
t_max = 100;
mean_x_init = 0;
prec_x_init = 1;
prec_x = 1;
prec_y = 10;
y = randn(t_max, 1);
x_true = randn(t_max, 1);
data = struct('y', y, 't_max', t_max, 'prec_x_init', prec_x_init,...
    'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);

%%% Compile BUGS model and sample data
model_filename = 'hmm_1d_lin.bug'; % BUGS model filename
sample_data = false; % Boolean
model = biips_model(model_filename, data, 'sample_data', sample_data); % Create biips model and sample data

% * Parsing model in: hmm_1d_lin.bug
% * Compiling data graph
%   Declaring variables
%   Resolving undeclared variables
%   Allocating nodes
% LOGIC ERROR: Subset y[1] out of range [1:100] in Compiler::setConstantMask.