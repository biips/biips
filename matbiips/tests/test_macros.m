clear
close all

%data
t_max=4;
prec_x=1;
prec_x_init=1;
mean_x_init=0;
prec_y=10;
data=struct('t_max', t_max, 'prec_x_init', prec_x_init, 'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);

model = biips_model('hmm_1d_lin.bug', data);
out_smc = biips_smc_samples(model, {'x[1:2]'}, 100);
% console cleaning 
biips_clear(model); 
