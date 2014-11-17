%% Quick examples of Matbiips usage
% This demo gathers the examples used in the Matbiips functions help.

%%
clear
close all

addpath ..
biips_clear

if isoctave() || verLessThan('matlab', '7.12')
    rand('state', 0)
else
    rng('default')
end

%% Add custom functions and distributions to BUGS language

% Add custom function f
type('f_dim.m');
type('f_eval.m');
biips_add_function('f', 2, 'f_dim', 'f_eval');

% Add custom sampling distribution dMN
type('dMN_dim.m');
type('dMN_sample.m');
biips_add_distribution('dMN', 2, 'dMN_dim', 'dMN_sample');

%% Compile model
modelfile = 'hmm_f.bug';
type(modelfile);

data = struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1), 'logtau', log(1));
model = biips_model(modelfile, data, 'sample_data', true);

biips_clear(model)

tmax = 10;
p = [.5; .5];
logtau_true = log(1);
logtau = log(1);
datanames = {'tmax', 'p', 'logtau_true', 'logtau'};
model = biips_model(modelfile, datanames, 'sample_data', true);

model.data

biips_variable_names(model)

biips_nodes(model)

dotfile = 'hmm.dot';
biips_print_dot(model, dotfile);
% type(dotfile);


%% SMC algorithm
biips_build_sampler(model, 'proposal', 'prior')
biips_nodes(model, 'type', 'stoch', 'observed', false)

biips_build_sampler(model, 'proposal', 'auto')
biips_nodes(model, 'type', 'stoch', 'observed', false)

n_part = 100;
[out_smc, lml] = biips_smc_samples(model, {'x', 'c[2:10]'}, n_part, 'type', 'fs', 'rs_thres', .5, 'rs_type', 'stratified');

out_smc
biips_diagnosis(out_smc);
summ_smc = biips_summary(out_smc)

out_smc.x
biips_diagnosis(out_smc.x);
summ_smc_x = biips_summary(out_smc.x, 'order', 2, 'probs', [.025, .975])
dens_smc_x = biips_density(out_smc.x, 'bw_type', 'nrd0', 'adjust', 1, 'n', 100)

out_smc_c = getfield(out_smc, 'c[2:10]')
biips_diagnosis(out_smc_c);
summ_smc_c  = biips_summary(out_smc_c)
table_smc_c = biips_table(out_smc_c)

out_smc.x.f
out_smc.x.s
biips_diagnosis(out_smc.x.f);
biips_diagnosis(out_smc.x.s);
biips_summary(out_smc.x.f)
biips_summary(out_smc.x.s)
biips_density(out_smc.x.f)
biips_density(out_smc.x.s)
biips_table(out_smc_c.f)
biips_table(out_smc_c.s)

figure
subplot(2,2,1); hold on
plot(model.data.x_true, 'g')
plot(summ_smc_x.f.mean, 'b')
plot(summ_smc_x.s.mean, 'r')
plot([summ_smc_x.f.quant{:}], '--b')
plot([summ_smc_x.s.quant{:}], '--r')
xlabel('t')
ylabel('x[t]')
legend('true', 'SMC filtering estimate', 'SMC smoothing estimate')
legend boxoff

subplot(2,2,2,'YTick',zeros(1,0)); hold on
bar(1:data.tmax, 2+.5*(model.data.c_true==1), 'g', 'barwidth', 1, 'basevalue', 2, 'edgecolor', 'none')
text(2, 2.75, 'true')
bar(2:data.tmax, 1+.5*(summ_smc_c.f.mode==1), 'b', 'barwidth', 1, 'basevalue', 1, 'edgecolor', 'none')
text(2, 1.75, 'SMC filtering mode')
bar(2:data.tmax, .5*(summ_smc_c.s.mode==1), 'r', 'barwidth', 1, 'basevalue', 0, 'edgecolor', 'none')
text(2, .75, 'SMC smoothing mode')
xlim([1,data.tmax+1])
ylim([0,3])
xlabel('t')
ylabel('c[t]==1')

t = 5;
subplot(2,2,3); hold on
plot(model.data.x_true(t), 0, 'g^', 'markerfacecolor', 'g')
plot(dens_smc_x.f(t).x, dens_smc_x.f(t).f, 'b')
plot(dens_smc_x.s(t).x, dens_smc_x.s(t).f, 'r')
xlabel(sprintf('x[%d]', t))
ylabel('posterior density')

subplot(2,2,4); hold on
bar(table_smc_c.f(t-1).x, table_smc_c.f(t-1).f, 'b', 'barwidth', .1)
bar(table_smc_c.s(t-1).x+.1, table_smc_c.s(t-1).f, 'r', 'barwidth', .1)
plot(model.data.c_true(t), 0, 'g^', 'markerfacecolor', 'g')
xlabel(sprintf('c[%d]', t))
ylabel('posterior probability mass')

%% PIMH algorithm
modelfile = 'hmm.bug';
type(modelfile);

data = struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1), 'logtau', log(1));
model = biips_model(modelfile, data);

n_part = 50;
obj_pimh = biips_pimh_init(model, {'x', 'c[2:10]'}); % Initialize
[obj_pimh, lml_pimh_burn] = biips_pimh_update(obj_pimh, 100, n_part); % Burn-in
[obj_pimh, out_pimh, lml_pimh] = biips_pimh_samples(obj_pimh, 100, n_part); % Samples

out_pimh
summ_pimh = biips_summary(out_pimh)

summ_pimh_x = biips_summary(out_pimh.x, 'order', 2, 'probs', [.025, .975])
dens_pimh_x = biips_density(out_pimh.x)

out_pimh_c = getfield(out_pimh, 'c[2:10]');
summ_pimh_c = biips_summary(out_pimh_c)
table_pimh_c = biips_table(out_pimh_c)

figure
subplot(2,1,1); hold on
plot([lml_pimh_burn, lml_pimh])
xlabel('PIMH iteration')
ylabel('log marginal likelihood')

t = 5;
subplot(2,1,2); hold on
plot(0, model.data.x_true(t), 'g>', 'markerfacecolor', 'g')
plot(out_pimh.x(t,:))
xlabel('PIMH iteration')
ylabel(sprintf('x[%d]', t))

figure
subplot(2,2,1); hold on
plot(model.data.x_true, 'g')
plot(summ_pimh_x.mean, 'b')
plot([summ_pimh_x.quant{:}], '--b')
xlabel('t')
ylabel('x[t]')
legend('true', 'PIMH estimate')
legend boxoff

subplot(2,2,2,'YTick',zeros(1,0)); hold on
bar(1:data.tmax, 1+.5*(model.data.c_true==1), 'g', 'barwidth', 1, 'basevalue', 1, 'edgecolor', 'none')
text(2, 1.75, 'true')
bar(2:data.tmax, .5*(summ_pimh_c.mode==1), 'b', 'barwidth', 1, 'basevalue', 0, 'edgecolor', 'none')
text(2, .75, 'PIMH mode')
xlim([1,data.tmax+1])
ylim([0,2])
xlabel('t')
ylabel('c[t]==1')

subplot(2,2,3); hold on
plot(model.data.x_true(t), 0, 'g^', 'markerfacecolor', 'g')
plot(dens_pimh_x(t).x, dens_pimh_x(t).f, 'b')
xlabel(sprintf('x[%d]', t))
ylabel('posterior density')

subplot(2,2,4); hold on
bar(table_pimh_c(t-1).x, table_pimh_c(t-1).f, 'b', 'barwidth', .1)
plot(model.data.c_true(t), 0, 'g^', 'markerfacecolor', 'g')
xlabel(sprintf('c[%d]', t))
ylabel('posterior probability mass')

%% SMC sensitivity analysis
modelfile = 'hmm.bug';
type(modelfile);

data = struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1), 'logtau', log(1));
model = biips_model(modelfile, data);

n_part = 50;
logtau_val = -10:10;
out_sens = biips_smc_sensitivity(model, {'logtau'}, {logtau_val}, n_part);

figure
subplot(2,1,1); hold on
title('SMC sensitivity')
plot(logtau_val, out_sens.log_marg_like)
plot(data.logtau, min(out_sens.log_marg_like), 'g^', 'markerfacecolor', 'g')
xlabel('logtau')
ylabel('log p(y|logtau)')

subplot(2,1,2); hold on
plot(logtau_val, out_sens.log_marg_like_pen)
yl = ylim;
plot(data.logtau, yl(1), 'g^', 'markerfacecolor', 'g')
xlabel('logtau')
ylabel('log p(y|logtau) + log p(logtau)')

%% PMMH algorithm
modelfile = 'hmm.bug';
type(modelfile);

data = struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1));
model = biips_model(modelfile, data);

n_part = 50;
obj_pmmh = biips_pmmh_init(model, {'logtau'}, 'latent_names', {'x', 'c[2:10]'}, 'inits', {-2}); % Initialize
[obj_pmmh, plml_pmmh_burn] = biips_pmmh_update(obj_pmmh, 100, n_part); % Burn-in
[obj_pmmh, out_pmmh, plml_pmmh] = biips_pmmh_samples(obj_pmmh, 100, n_part, 'thin', 1); % Samples

out_pmmh
summ_pmmh = biips_summary(out_pmmh)

summ_pmmh_lt = biips_summary(out_pmmh.logtau, 'order', 2, 'probs', [.025, .975], 'mode', true)
dens_pmmh_lt = biips_density(out_pmmh.logtau)

summ_pmmh_x = biips_summary(out_pmmh.x, 'order', 2, 'probs', [.025, .975], 'mode', true)
dens_pmmh_x = biips_density(out_pmmh.x)

out_pmmh_c  = getfield(out_pmmh, 'c[2:10]');
summ_pmmh_c = biips_summary(out_pmmh_c)
table_pmmh_c = biips_table(out_pmmh_c)

figure
subplot(2,2,1); hold on
plot([plml_pmmh_burn, plml_pmmh])
xlabel('PMMH iteration')
ylabel('log p(y|logtau) + log p(logtau)')

subplot(2,2,2); hold on
plot(0, model.data.logtau_true, 'g>', 'markerfacecolor', 'g')
plot(out_pmmh.logtau)
xlabel('PMMH iteration')
ylabel('logtau')

subplot(2,2,3); hold on
plot(model.data.logtau_true, 0, '^g', 'markerfacecolor', 'g')
plot(dens_pmmh_lt.x, dens_pmmh_lt.f, 'b')
xlabel('logtau')
ylabel('posterior density')

subplot(2,2,4); hold on
hist(out_pmmh.logtau)
plot(model.data.logtau_true, 0, '^g', 'markerfacecolor', 'g')
xlabel('logtau')
ylabel('posterior density')

figure
subplot(2,2,1); hold on
plot(model.data.x_true, 'g')
plot(summ_pmmh_x.mean, 'b')
plot([summ_pmmh_x.quant{:}], '--b')
xlabel('t')
ylabel('x[t]')
legend('true', 'PMMH estimate')
legend boxoff

subplot(2,2,2,'YTick',zeros(1,0)); hold on
bar(1:data.tmax, 1+.5*(model.data.c_true==1), 'g', 'barwidth', 1, 'basevalue', 1, 'edgecolor', 'none')
text(2, 1.75, 'true')
bar(2:data.tmax, .5*(summ_pmmh_c.mode==1), 'b', 'barwidth', 1, 'basevalue', 0, 'edgecolor', 'none')
text(2, .75, 'PMMH mode')
xlim([1,data.tmax+1])
ylim([0,2])
xlabel('t')
ylabel('c[t]==1')

t = 5;
subplot(2,2,3); hold on
plot(model.data.x_true(t), 0, 'g^', 'markerfacecolor', 'g')
plot(dens_pmmh_x(t).x, dens_pmmh_x(t).f, 'b')
xlabel(sprintf('x[%d]', t))
ylabel('posterior density')

subplot(2,2,4); hold on
bar(table_pmmh_c(t-1).x, table_pmmh_c(t-1).f, 'b', 'barwidth', .1)
plot(model.data.c_true(t), 0, 'g^', 'markerfacecolor', 'g')
xlabel(sprintf('c[%d]', t))
ylabel('posterior probability mass')
