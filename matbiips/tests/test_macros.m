%data
tmax=4;
addpath('../matlab');
precx=1;
precxinit=1;
meanxinit=0;
precy=10;
data=struct('tmax', tmax, 'precxinit', precxinit, 'precx', precx,  'precy', precy, 'meanxinit', meanxinit);
% intialisation console
biips_init;
p=biips_model('hmm_1d_lin.bug', data);
out_smc=biips_smc_samples(p, {'x[1:2]'}, 100);
% on nettoie la console
inter_biips('clear_console', p); 
