%% Quick examples of Matbiips usage

%%
clear
close all

addpath ..
biips_clear

%% Add user-defined functions and distributions to BUGS language

% Add function f
type('f_dim.m');
type('f_eval.m');
biips_add_function('f', 2, 'f_dim', 'f_eval');

% Add sampling distribution dMN
type('dMN_dim.m');
type('dMN_sample.m');
biips_add_distribution('dMN', 2, 'dMN_dim', 'dMN_sample');

%% Compile model
modelfile = 'hmm.bug';
type(modelfile);

data = struct('tmax', 10, 'logtau', log(10));
model = biips_model(modelfile, data, 'sample_data', true);

model.data

biips_variable_names(model)

biips_nodes(model)

dotfile = 'hmm.dot';
biips_print_dot(model, dotfile);
type(dotfile);

biips_clear(model)

tmax = 10;
logtau = log(10);
datanames = {'tmax', 'logtau'};
model = biips_model(modelfile, datanames, 'sample_data', true);

%% SMC
biips_build_sampler(model, 'proposal', 'prior')
biips_nodes(model, 'type', 'stoch', 'observed', false)

biips_build_sampler(model, 'proposal', 'auto')
biips_nodes(model, 'type', 'stoch', 'observed', false)

n_part = 100;

[out_smc, lml] = biips_smc_samples(model, {'x[1]', 'x[8:10]'}, n_part, 'type', 'fs', 'rs_thres', .5, 'rs_type', 'stratified');

out_smc
biips_diagnosis(out_smc);
summ_smc = biips_summary(out_smc)
dens_smc = biips_density(out_smc)

out_smc2 = getfield(out_smc, 'x[8:10]')
biips_diagnosis(out_smc2);
summ_smc = biips_summary(out_smc2)
dens_smc = biips_density(out_smc2)

out_smc2.f
out_smc2.s
biips_diagnosis(out_smc2.f);
summ_smc = biips_summary(out_smc2.f)
dens_smc = biips_density(out_smc2.f)


[out_smc, lml] = biips_smc_samples(model, {'x'}, n_part);

out_smc.x.f
out_smc.x.s

biips_diagnosis(out_smc);
summ_smc = biips_summary(out_smc, 'order', 2, 'probs', [.025, .975]);

figure
subplot(2,1,1); hold on
plot(model.data.x_true, 'g')
plot(summ_smc.x.f.mean, 'b')
plot(summ_smc.x.s.mean, 'r')
plot(summ_smc.x.f.quant{1}, '--b')
plot(summ_smc.x.f.quant{2}, '--b')
plot(summ_smc.x.s.quant{1}, '--r')
plot(summ_smc.x.s.quant{2}, '--r')
xlabel('t')
ylabel('x[t]')
legend('true', 'SMC filtering estimate', 'SMC smoothing estimate')
legend boxoff

dens_smc = biips_density(out_smc, 'bw_type', 'nrd0', 'adjust', 1, 'n', 100);

subplot(2,1,2); hold on
plot(model.data.x_true(1), 0, 'g^', 'markerfacecolor', 'g')
plot(dens_smc.x.f(1).x, dens_smc.x.f(1).f, 'b')
plot(dens_smc.x.s(1).x, dens_smc.x.s(1).f, 'r')
xlabel('x[1]')
ylabel('posterior density')

%% PIMH
modelfile = 'hmm.bug';
type(modelfile);

data = struct('tmax', 10, 'logtau', log(10));
model = biips_model(modelfile, data, 'sample_data', true);

n_part = 50;
obj_pimh = biips_pimh_init(model, {'x'}); % Initialize
[obj_pimh, lml_pimh_burn] = biips_pimh_update(obj_pimh, 100, n_part); % Burn-in
[obj_pimh, out_pimh, lml_pimh] = biips_pimh_samples(obj_pimh, 100, n_part); % Samples

out_pimh
summ_pimh = biips_summary(out_pimh)
dens_pimh = biips_density(out_pimh)

out_pimh.x
summ_pimh = biips_summary(out_pimh.x)
dens_pimh = biips_density(out_pimh.x)

figure
subplot(2,2,1); hold on
plot([lml_pimh_burn, lml_pimh])
xlabel('PIMH iteration')
ylabel('log marginal likelihood')

subplot(2,2,2); hold on
plot(0, model.data.x_true(1), 'g>', 'markerfacecolor', 'g')
plot(out_pimh.x(1,:))
xlabel('PIMH iteration')
ylabel('x[1]')

summ_pimh = biips_summary(out_pimh, 'order', 2, 'probs', [.025, .975]);

subplot(2,2,3); hold on
plot(model.data.x_true, 'g')
plot(summ_pimh.x.mean, 'b')
plot(summ_pimh.x.quant{1}, '--b')
plot(summ_pimh.x.quant{2}, '--b')
xlabel('t')
ylabel('x[t]')
legend('true', 'PIMH estimate')
legend boxoff

dens_pimh = biips_density(out_pimh);

subplot(2,2,4); hold on
plot(model.data.x_true(1), 0, 'g^', 'markerfacecolor', 'g')
plot(dens_pimh.x(1).x, dens_pimh.x(1).f, 'b')
xlabel('x[1]')
ylabel('posterior density')

%% SMC sensitivity analysis
modelfile = 'hmm.bug';
type(modelfile);

data = struct('tmax', 10, 'logtau', log(10));
model = biips_model(modelfile, data, 'sample_data', true);

n_part = 50;
logtau_val = -10:10;
out_sens = biips_smc_sensitivity(model, {'logtau'}, {logtau_val}, n_part);

figure
subplot(2,1,1); hold on
title('SMC sensitivity')
plot(logtau_val, out_sens.log_marg_like)
plot(data.logtau, min(out_sens.log_marg_like), 'g^', 'markerfacecolor', 'g')
xlabel('logtau')
ylabel('log marginal likelihood')

subplot(2,1,2); hold on
plot(logtau_val, out_sens.log_marg_like_pen)
yl = ylim;
plot(data.logtau, yl(1), 'g^', 'markerfacecolor', 'g')
xlabel('logtau')
ylabel('penalized log marginal likelihood')

%% PMMH
modelfile = 'hmm.bug';
type(modelfile);

logtau_true = 10;
data = struct('tmax', 10);
model = biips_model(modelfile, data, 'sample_data', true);

n_part = 50;
obj_pmmh = biips_pmmh_init(model, {'logtau'}, 'latent_names', {'x'}, 'inits', {-2}); % Initialize
[obj_pmmh, plml_pmmh_burn] = biips_pmmh_update(obj_pmmh, 100, n_part); % Burn-in
[obj_pmmh, out_pmmh, plml_pmmh] = biips_pmmh_samples(obj_pmmh, 100, n_part, 'thin', 1); % Samples

out_pmmh
summ_pmmh = biips_summary(out_pmmh)
dens_pmmh = biips_density(out_pmmh)

out_pmmh.x
summ_pmmh = biips_summary(out_pmmh.x)
dens_pmmh = biips_density(out_pmmh.x)

figure
subplot(2,2,1); hold on
plot([plml_pmmh_burn, plml_pmmh])
xlabel('PMMH iteration')
ylabel('penalized log marginal likelihood')

subplot(2,2,2); hold on
plot(0, logtau_true, 'g>', 'markerfacecolor', 'g')
plot(out_pmmh.logtau)
xlabel('PMMH iteration')
ylabel('logtau')

summ_pmmh = biips_summary(out_pmmh, 'order', 2, 'probs', [.025, .975]);

subplot(2,2,3); hold on
plot(model.data.x_true, 'g')
plot(summ_pmmh.x.mean, 'b')
plot(summ_pmmh.x.quant{1}, '--b')
plot(summ_pmmh.x.quant{2}, '--b')
xlabel('t')
ylabel('x[t]')
legend('true', 'PMMH estimate')
legend boxoff

dens_pmmh = biips_density(out_pmmh);

subplot(2,2,4); hold on
plot(logtau_true, 0, '^g', 'markerfacecolor', 'g')
plot(dens_pmmh.logtau.x, dens_pmmh.logtau.f, 'b')
xlabel('logtau')
ylabel('posterior density')
