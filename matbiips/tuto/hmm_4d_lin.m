clear
addpath('../matlab');

t_max = 10;
mean_x_init = [0 0 1 0]';
prec_x_init = diag(100*ones(4,1));
delta_t = 1;
F = [ 1 0 delta_t 0; 0 1 0 delta_t; 0 0 1 0; 0 0 0 1];
G = [ delta_t.^2/2 0; 0 delta_t.^2/2 ; delta_t 0; 0 delta_t];
H = [ diag(ones(2,1)) zeros(2,2) ];
mean_v = zeros(2,1);
prec_v = diag(1*ones(2,1));
prec_y = diag([2 2]);

%% create model
biips_init;
[p, data_out] = biips_model('hmm_4d_lin_tracking.bug', who);

%% run SMC
n_part = 100;
out_smc = biips_smc_samples(p, {'x'}, n_part);
