%%% Model parameters
clust_max = 30;
t_max = 100;
y = .1*randn(t_max, 1);
mu_0 = 0;
prec_0 = 1;
prec_y = 5;
alpha = 3;
rho = .9;
gamma = .9;
data = {'rho', 'gamma', 't_max', 'clust_max', 'y', 'mu_0', 'prec_0', 'prec_y', 'alpha'};
% data = {'t_max', 'y', 'mu_0', 'prec_y'};

%%% Start BiiPS console
biips_init;

%%% Compile BUGS model and sample data
model = 'tvdp.bug'; % BUGS model filename
sample_data = false; % Boolean
[model_id, data] = biips_model(model, data, 'sample_data', sample_data); % Create biips model and sample data


%% TESTS BIIPS_SMC_SAMPLES
n_part = 1000; % Number of particles
variables = {'c', 'mu', 'm'}; % Variables to be monitored
type = 'fs'; rs_type = 'stratified'; rs_thres = 0.5; % Optional parameters
% Run SMC
out_smc = biips_smc_samples(model_id, variables, n_part,...
    'type', type, 'rs_type', rs_type, 'rs_thres', rs_thres);
% 

n0 = 100;
x0 = linspace(-5,5,n0);
out = zeros(t_max, n0);
for t=1:t_max
    for i=1:n_part
        c_i = logical(out_smc.c.f.values(t, :, i));
        mu_i = out_smc.mu.f.values(t, :, i);
        m_i =   out_smc.m.f.values(t, :, i);
        w_i = out_smc.mu.f.weights(t,1,i);
        out(t, :) = out(t, :) ...
            + w_i*sum( repmat(m_i', 1, n0).*normpdf(repmat(x0, clust_max, 1),...
            repmat(mu_i', 1, n0), 1/sqrt(prec_y)))/t;
    end
    
end
figure
for t=1:5:t_max
    line(t*ones(length(x0)), x0, out(t, :))
    hold on
end
view(3)
