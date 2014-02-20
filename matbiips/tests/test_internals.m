addpath('../matlab');
%data
t_max=4;
prec_x=1;
prec_x_init=1;
mean_x_init=0;
prec_y=10;
data=struct('t_max', t_max, 'prec_x_init', prec_x_init, 'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);
% console initialisation 
biips_init;
console = biips_model('hmm_1d_lin.bug', data);
out_smc = biips_smc_samples(console, {'x[1:2]'}, 100);
% test change data
change_ok = inter_biips('change_data', console, 'x', [3] , [3] , 0.5, true)
%
sample = inter_biips('sample_data', console, 'x', [1] , [1] , 1.2)
% console cleaning 
inter_biips('clear_console', console); 
