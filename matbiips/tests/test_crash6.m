% Test SMC with matrices
addpath ..
t_max = 2;
model = biips_model('hmm.bug', {'t_max'});
biips_smc_samples(model, {'x[1:2,1]'}, 1);
biips_smc_samples(model, {'x[1:2,1]'}, 1);
biips_smc_samples(model, {'x[1:2,1]'}, 1);