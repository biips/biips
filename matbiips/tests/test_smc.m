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
id=matbiips('make_console'); 
matbiips('verbosity',2);

% verif et compil du modele
matbiips('check_model', id, 'hmm_1d_lin.bug'); 
matbiips('compile_model', id, data, true, 12, false);
matbiips('set_default_monitors',id);
matbiips('set_filter_monitors', id, {'x'} , {1 }, { t_max });
matbiips('set_gen_tree_smooth_monitors', id, {'x'}, {1}, {t_max});
matbiips('set_backward_smooth_monitors', id, {'x'}, {1}, {t_max });
if (~matbiips('is_sampler_built',id))
  matbiips('build_smc_sampler',id, false);
  sort_nodes=matbiips('get_sorted_nodes', id)
  node_samplers=matbiips('get_nodes_samplers', id)
end

if (~matbiips('run_smc_sampler', id, 100, 9, 0.5, 'stratified'))
   error('smc sampler failed!');
end  
log_norm=matbiips('get_log_norm_const', id)
filter_monitors=matbiips('get_filter_monitors',id);
if isoctave()
    getfield(filter_monitors, 'x[1:4]').conditionals{:}
else
    filter_monitors.('x[1:4]').conditionals{:}
end
smooth_monitors=matbiips('get_gen_tree_smooth_monitors',id);
if isoctave()
    getfield(smooth_monitors, 'x[1:4]').conditionals{:}
else
    smooth_monitors.('x[1:4]').conditionals{:}
end
sample_value = matbiips('sample_gen_tree_smooth_particle', id, 42);
matbiips('clear_gen_tree_smooth_monitors',id, false);
% on fait un backward
if (~matbiips('is_smc_sampler_at_end', id))
   error('sampler not finish!');
end   
matbiips('run_backward_smoother', id);
matbiips('clear_filter_monitors', id, false);
backward_smooth_monitors=matbiips('get_backward_smooth_monitors',id);
if isoctave()
    getfield(backward_smooth_monitors, 'x[1:4]').conditionals{:}
else
    backward_smooth_monitors.('x[1:4]').conditionals{:}
end
matbiips('clear_backward_smooth_monitors', id, false);
% on nettoie la console
matbiips('clear_console',id); 
