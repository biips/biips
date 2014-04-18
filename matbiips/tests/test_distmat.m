clear
close all

%% define data
t_max = 20; 
mean_x_init = 0;
prec_x_init = 0.5;
prec_x = 10;
prec_y = 1;

%% build evalutations functions
biips_add_distribution('distmat', 3, 'mydistdim', 'mydistsample')

%% create model
model = biips_model('hmm_1d_nonlin_distmat.bug', who);
data = model.data;
x_true = data.x_true;

%% run SMC
n_part = 100;
out_smc = biips_smc_samples(model, {'x'}, n_part, 'type', 'fs');

%% filtering stats
x_summ = biips_summary(out_smc, 'type', 'fs');

% figure
% plot(x_true, 'g')
% hold on
% plot(x_summ.x.f.mean, 'b')
% plot(x_summ.x.s.mean, 'm')

% compute densities
% x_dens = biips_density(out_smc, 'fsb', 'fs'); 
