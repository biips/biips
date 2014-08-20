%% TESTS BIIPS_PMMH
clear
close all

% Test model with cell data structure
t_max = 20; mean_x_init = 0;prec_x_init = 1;prec_x = 1; mean0 = [0;0]; prec0 = eye(2);
data = {'t_max', 'prec_x_init',...
    'prec_x', 'mean_x_init', 'mean0', 'prec0'};

%%% Compile BUGS model and sample data
model_filename = 'hmm_1d_nonlin_param2.bug'; % BUGS model filename
sample_data = true; % Boolean
model = biips_model(model_filename, data, 'sample_data', sample_data);
data = model.data;

param_names = {'log_prec_y[1:2]','log_prec_y2[1:2]'};
inits = {[0;0], [0;0]};
latent_names = {};
n_burn = 500;
n_part = 50;
n_iter = 520;
%%
% *Init PMMH*
obj_pmmh = biips_pmmh_init(model, param_names, 'inits', inits); % creates a pmmh object

%%
% *Run PMMH*
obj_pmmh = biips_pmmh_update(obj_pmmh, n_burn, n_part); % adaptation and burn-in iterations
[obj_pmmh, out_pmmh, log_marg_like_pen, log_marg_like, stats_pmmh] = biips_pmmh_samples(obj_pmmh, n_iter, n_part,...
    'thin', 1, 'latent_names', latent_names); % Samples
    
biips_clear
%    Warning: RUNTIME ERROR: Can not change data: variable log_prec_y[1:2]
% does not match one node exactly.
%  
% Error using biips_pmmh_object>set_param (line 105)
% Data change failed: invalid initial value for variable log_prec_y
% 
% Error in biips_pmmh_object (line 42)
% sample_param = set_param(console, pn_param, inits);
% 
% Error in test_crash4 (line 21)
%     obj_pmmh = biips_pmmh_object(model, param_names, 'inits', inits);
