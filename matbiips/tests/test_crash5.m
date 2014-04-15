clear
close all

% THIS IS OK
t_max = 10; mean_x_init = 0;prec_x_init = 1;prec_x = 1;prec_y = 10;
data = struct('t_max', t_max, 'prec_x_init', prec_x_init,...
    'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);

%%% Compile BUGS model and sample data
model_filename = 'hmm_1d_lin.bug'; % BUGS model filename
sample_data = true; % Boolean
model = biips_model(model_filename, data, 'sample_data', sample_data);
data = model.data;

variables = {'x', 'x[1:2]'};
n_part = 100;
n_iter = 20;
biips_pimh_update(model, variables, n_iter, n_part);
out_pimh = biips_pimh_samples(model, variables, n_iter, n_part);
biips_clear(model)

% THE SAME WITH MATRICES IS NOT
t_max = 10; mean_x_init = 0;prec_x_init = 1;prec_x = 1;prec_y = 10;
data = struct('t_max', t_max, 'prec_x_init', prec_x_init,...
    'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);

%%% Compile BUGS model and sample data
model_filename = 'hmm_1d_lin2.bug'; % BUGS model filename
sample_data = true; % Boolean
model = biips_model(model_filename, data, 'sample_data', sample_data);
data = model.data;

variables = {'x', 'x[1:2,1]'};
n_part = 100;
n_iter = 20;
biips_pimh_update(model, variables, n_iter, n_part);
out_pimh = biips_pimh_samples(model, variables, n_iter, n_part);
biips_clear(model)
