clear
close
addpath('../matlab');

%% define data
t_max = 10;
mean_x_init = [0 0 1 0]';
prec_x_init = diag(1000*ones(4,1));
x_pos = [-10  0];
mean_v = zeros(2, 1);
prec_v = diag(1*ones(2,1));
prec_y = diag([10 100]);
delta_t = 1;

%% create model
biips_init;

%% build evalutations functions
biips_add_function('funcmat',2, 'myfuncdim', 'myfunceval')
[p, data] = biips_model('funcmat.bug', who);
x_true = data.x_true(1:2,:);

%% run SMC
n_part = 100;
x_name = ['x[1:2,1:' int2str(t_max) ']'];
y_name = ['y[1,1:' int2str(t_max) ']'];
out_smc = biips_smc_samples(p, {x_name, y_name}, n_part, 'fsb');

%% filtering stats
x_summ = biips_summary(out_smc, {}, 'fs');

% compute densities
x_dens = biips_density(out_smc, {}, 'fs'); 
