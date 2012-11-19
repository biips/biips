addpath('../matlab');
%data=struct('y', y, 'tmax', 10, 'precxinit', 1,'precx', 10, 'precy', 1, 'meanxinit', 0);
bool_et_un = inter_biips('load_module', 'basemod'); 
p0=inter_biips('make_console'); 
%inter_biips('check_model', p0, 'hmm_1d_lin.bug'); 
%inter_biips('compile_model', p0, data, false, 12);
%debilos = inter_biips('get_data',p0);
%vars = inter_biips('get_variable_names',p0);
%inter_biips('set_default_monitors',p0);
%inter_biips('verbosity',2);
%inter_biips('set_filter_monitors', p0, {'x'} , {[1] }, {[1]});
%inter_biips('set_smooth_tree_monitors', p0, {'x', 'y'}, {1, 1}, {1, 1});
%inter_biips('set_smooth_monitors', p0, {'x', 'y'}, {1, 1}, {1, 1});
%inter_biips('build_smc_sampler',p0, 0);
%bool_et_deux=inter_biips('is_sampler_built',p0)
%res=inter_biips('run_smc_sampler',p0, 7, 9, 0.5, 'stratified')
%gn=inter_biips('get_log_norm_const', p0)
%une_cellule=inter_biips('get_filter_monitors',p0)
%deux_cellules=inter_biips('get_smooth_tree_monitors',p0)
%trois_cellules=inter_biips('get_smooth_monitors',p0)
%inter_biips('run_backward_smoother', p0);
%node_samplers=inter_biips('get_nodes_samplers', p0)
%sort_nodes=inter_biips('get_sorted_nodes', p0)
%sample_dat=inter_biips('sample_data', p0, 'x', 1, 1, 12)
%get_prior = inter_biips('get_log_prior_density', p0, 'y', 1 , 1)
%bool_et_trois=inter_biips('is_smc_sampler_at_end', p0) 
%inter_biips('message', 'HHHAHAHHHA');
%inter_biips('set_log_norm_const', p0, 3); 
%inter_biips('print_graphviz',p0, 'debilos');
%inter_biips('clear_filter_monitors',p0);
%inter_biips('clear_smooth_tree_monitors',p0);
%inter_biips('clear_smooth_monitors',p0);
%pb1=inter_biips('make_progress_bar', 100, '*', 'zut')
%for i=1:100
%   inter_biips('advance_progress_bar', pb1, 1);
%end
%bool_et_quatre = inter_biips('is_smooth_tree_monitors', p0, {'x', 'y'}, {1, 1}, {1, 1})
%%inter_biips('sample_smooth_tree_particle', p0, 3);
%%sampl_tree = inter_biips('get_sampled_smooth_tree_particle', p0);
%inter_biips('clear_progress_bar', pb1);
inter_biips('clear_console',p0); 
