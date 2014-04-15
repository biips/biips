clear
close all

%data
t_max=4;
prec_x=1;
prec_x_init=1;
mean_x_init=0;
prec_y=10;
data=struct('t_max', t_max, 'prec_x_init', prec_x_init, 'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);
if (~inter_biips('load_module', 'basemod'))
    error('problem loading module'); 
end
% console init
p0=inter_biips('make_console'); 
inter_biips('verbosity',2);

% verif et compil du modele
inter_biips('check_model', p0, 'hmm_1d_lin.bug'); 
inter_biips('compile_model', p0, data, true, 12);
inter_biips('set_default_monitors',p0);
inter_biips('set_filter_monitors', p0, {'x'} , {1 }, { t_max });
inter_biips('set_gen_tree_smooth_monitors', p0, {'x'}, {1}, {t_max});
inter_biips('set_backward_smooth_monitors', p0, {'x'}, {1}, {t_max });
if (~inter_biips('is_sampler_built',p0))
  inter_biips('build_smc_sampler',p0, false);
  sort_nodes=inter_biips('get_sorted_nodes', p0)
  node_samplers=inter_biips('get_nodes_samplers', p0)
end

if (~inter_biips('run_smc_sampler', p0, 100, 9, 0.5, 'stratified'))
   error('smc sampler failed!');
end  
log_norm=inter_biips('get_log_norm_const', p0)
filter_monitors=inter_biips('get_filter_monitors',p0)
smooth_monitors=inter_biips('get_gen_tree_smooth_monitors',p0)
inter_biips('sample_gen_tree_smooth_particle', p0, 42);
sample_value =  inter_biips('get_sampled_gen_tree_smooth_particle', p0);
inter_biips('clear_gen_tree_smooth_monitors',p0, false);
% on fait un backward
if (~inter_biips('is_smc_sampler_at_end', p0))
   error('sampler not finish!');
end   
inter_biips('run_backward_smoother', p0);
inter_biips('clear_filter_monitors', p0, false);
backward_smooth_monitors=inter_biips('get_backward_smooth_monitors',p0)
inter_biips('clear_backward_smooth_monitors', p0, false);
% on nettoie la console
inter_biips('clear_console',p0); 
