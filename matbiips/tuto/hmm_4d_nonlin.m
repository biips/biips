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
F = [ 1 0 delta_t 0
    0 0 1 0
    delta_t 0 0 1
    0 0 0 1];
G = [ delta_t.^2/2 0
    0 delta_t.^2/2
    delta_t 0
    0 delta_t];

%% create model
biips_init;
[p, data] = biips_model('hmm_4d_nonlin_tracking.bug', who);
x_true = data.x_true(1:2,:);

%% run SMC
n_part = 100;
x_name = ['x[1:2,1:' int2str(t_max) ']'];
y_name = 'y';
out_smc = biips_smc_samples(p, {x_name, y_name}, n_part, 'fsb');

%% filtering stats
x_summ_f = biips_summary(out_smc, {x_name, y_name}, 'fsb');

x_dens_f = cell(2,t_max);
for i=1:2
    for j=1:t_max
        [f, xi, b] = ksdensity(squeeze(out_smc.(x_name).f.values(i,j,:)), 'weights', squeeze(out_smc.(x_name).f.weights(i,j,:)));
        x_dens_f{i,j}.x = xi;
        x_dens_f{i,j}.y = f;
        x_dens_f{i,j}.bw = b;
    end
end


x_dens_s = cell(2,t_max);
for i=1:2
    for j=1:t_max
        [f, xi, b] = ksdensity(squeeze(out_smc.(x_name).s.values(i,j,:)), 'weights', squeeze(out_smc.(x_name).s.weights(i,j,:)));
        x_dens_s{i,j}.x = xi;
        x_dens_s{i,j}.y = f;
        x_dens_s{i,j}.bw = b;
    end
end

x_dens_b = cell(2,t_max);
for i=1:2
    for j=1:t_max
        [f, xi, b] = ksdensity(squeeze(out_smc.(x_name).b.values(i,j,:)), 'weights', squeeze(out_smc.(x_name).b.weights(i,j,:)));
        x_dens_b{i,j}.x = xi;
        x_dens_b{i,j}.y = f;
        x_dens_b{i,j}.bw = b;
    end
end

