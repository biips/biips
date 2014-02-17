function monitor_biips(p, variable_names, type)
  
pn = cellfun(@(x) parse_varname(x), variable_names); 
names = {pn.name};
lower = {pn.lower};
upper = {pn.upper};
indices = arrayfun(@(x) strfind(type, x), 'fsb', 'UniformOutput', 0); 
  
if (~isempty(indices{1})) % filtering
  inter_biips('set_filter_monitors', p, names, lower, upper);
end 
if (~isempty(indices{2})) % smoothing
%     p, names, lower, upper
  inter_biips('set_gen_tree_smooth_monitors', p, names, lower, upper);
end  
if (~isempty(indices{3})) %backward_smoothing
  inter_biips('set_backward_smooth_monitors', p, names, lower, upper);
end 
