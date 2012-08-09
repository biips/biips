x=load('x.dat');
y=load('y.dat');
data=struct('y', y, 'tmax', 100, 'precxinit', 1,'precx', 100, 'precy', 1, 'meanxinit', 0);
bool_et_un = inter_biips('load_module', 'basemod'); 
p0=inter_biips('make_console'); 
inter_biips('check_model', p0, 'hmm_1d_lin.bug'); 
inter_biips('compile_model', p0, data, false, 12);
debilos = inter_biips('get_data',p0);
vars = inter_biips('get_variable_names',p0);
inter_biips('verbosity',2);
inter_biips('set_default_monitors',p0);
inter_biips('set_filter_monitors', p0, {'x', 'y'}, {1, 1}, {1, 1});
inter_biips('set_smooth_tree_monitors', p0, {'x', 'y'}, {1, 1}, {1, 1});
inter_biips('set_smooth_monitors', p0, {'x', 'y'}, {1, 1}, {1, 1});
inter_biips('build_smc_sampler',p0, 0);
bool_et_deux=inter_biips('is_sampler_built',p0)
res=inter_biips('run_smc_sampler',p0, 34, 12, 0.5, 'stratified')
gn=inter_biips('get_log_norm_const', p0)
une_cellule=inter_biips('get_filter_monitors',p0)
deux_cellules=inter_biips('get_smooth_tree_monitors',p0)
trois_cellules=inter_biips('get_smooth_monitors',p0)
inter_biips('run_backward_smoother', p0);
sort_nodes=inter_biips('get_sorted_nodes', p0)
inter_biips('clear_filter_monitors',p0);
inter_biips('clear_smooth_tree_monitors',p0);
inter_biips('clear_smooth_monitors',p0);
inter_biips('clear_console',p0); 
