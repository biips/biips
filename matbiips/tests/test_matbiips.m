close all
clear all

% RUNS A SET OF TESTS

% Test console
N=10;
ids=zeros(N,1);
for i=1:N;
  ids(i)=inter_biips('make_console');
end
p=randperm(N);
for i=1:N
   inter_biips('clear_console',ids(p(i)));
end

% Test model
t_max = 10; mean_x_init = 0;prec_x_init = 1;prec_x = 1;prec_y = 10;
data = struct('t_max', t_max, 'prec_x_init', prec_x_init,...
    'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);
%%% Start BiiPS console
biips_init;
%%% Compile BUGS model and sample data
model = 'hmm_1d_linv2.bug'; % BUGS model filename
sample_data = true; % Boolean
[model_id, data] = biips_model(model, data, 'sample_data', sample_data);

% Test model with cell data structure
t_max = 40; mean_x_init = 0;prec_x_init = 1;prec_x = 1;prec_y = 10;
data = {'t_max', 'prec_x_init',...
    'prec_x', 'prec_y', 'mean_x_init'};
%%% Start BiiPS console
biips_init;
%%% Compile BUGS model and sample data
% model = 'hmm_1d_lin.bug'; % BUGS model filename
sample_data = true; % Boolean
[model_id, data] = biips_model(model, data, 'sample_data', sample_data);


% test model with wrong type for argument
data = {'t_max', 'prec_x_init',...
    'prec_x', 'prec_y', 'mean_x_init'};
sample_data = 'tartealacreme';
[model_id, data] = biips_model(model, data, 'sample_data', sample_data);

% MAKES THE PROGRAMM CRASH
% % Test model with cell data structure
% data = {'t_max', ...
%     'prec_x', 'prec_y', 'mean_x_init'};
% %%% Start BiiPS console
% biips_init;
% %%% Compile BUGS model and sample data
% model = 'hmm_1d_lin.bug'; % BUGS model filename
% sample_data = true; % Boolean
% [model_id, data] = biips_model(model, data, 'sample_data', sample_data);


% Test model with unknown file
badmodel = 'an_unknown_bug_file.bug'; % BUGS model filename
try(biips_model(badmodel, data, 'sample_data', sample_data))
catch err
    fprintf(err.message)    
end

% Test model with bad filename
badmodel2 = 2; % BUGS model filename
try(biips_model(badmodel2, data, 'sample_data', sample_data))
catch err
    fprintf(err.message)
end

%% TESTS BIIPS_SMC_SAMPLES
n_part = 20; % Number of particles
variables = {'x', 'x[1:2]', 'unknownvariable'}; % Variables to be monitored
type = 'fs'; rs_type = 'stratified'; rs_thres = 0.5; % Optional parameters
% Run SMC
out_smc = biips_smc_samples(model_id, variables, n_part,...
    'type', type, 'rs_type', rs_type, 'rs_thres', rs_thres);
nodes = biips_get_nodes(model_id)
