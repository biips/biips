clear
close all

%data
t_max=4;
prec_x=1;
prec_x_init=1;
mean_x_init=0;
prec_y=10;
data=struct('t_max', t_max, 'prec_x_init', prec_x_init, 'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);
if (~matbiips('load_module', 'basemod'))
    error('problem loading module'); 
end
% console init
p0=matbiips('make_console'); 
matbiips('verbosity',2);

% verif et compil du modele
matbiips('check_model', p0, 'hmm_1d_lin.bug'); 
matbiips('compile_model', p0, data, true, 12);
matbiips('set_default_monitors',p0);
matbiips('set_filter_monitors', p0, {'x'} , {1 }, { t_max });
matbiips('set_gen_tree_smooth_monitors', p0, {'x'}, {1}, {t_max});
matbiips('set_backward_smooth_monitors', p0, {'x'}, {1}, {t_max });
if (~matbiips('is_sampler_built',p0))
  matbiips('build_smc_sampler',p0, false);
  sort_nodes=matbiips('get_sorted_nodes', p0)
  node_samplers=matbiips('get_nodes_samplers', p0)
end

if (~matbiips('run_smc_sampler', p0, 100, 9, 0.5, 'stratified'))
   error('smc sampler failed!');
end  
log_norm=matbiips('get_log_norm_const', p0)
filter_monitors=matbiips('get_filter_monitors',p0)
smooth_monitors=matbiips('get_gen_tree_smooth_monitors',p0)
sample_value = matbiips('sample_gen_tree_smooth_particle', p0, 42);
matbiips('clear_gen_tree_smooth_monitors',p0, false);
% on fait un backward
if (~matbiips('is_smc_sampler_at_end', p0))
   error('sampler not finish!');
end   
matbiips('run_backward_smoother', p0);
matbiips('clear_filter_monitors', p0, false);
backward_smooth_monitors=matbiips('get_backward_smooth_monitors',p0)
matbiips('clear_backward_smooth_monitors', p0, false);
% on nettoie la console
matbiips('clear_console',p0); 
