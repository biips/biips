function monitor(console, varnames, type)
  
pn = cellfun(@(x) parse_varname(x), varnames); 
names = {pn.name};
lower = {pn.lower};
upper = {pn.upper};

indices = arrayfun(@(x) strfind(type, x), 'fsb', 'UniformOutput', 0); 
  
if (~isempty(indices{1})) % filtering
  matbiips('set_filter_monitors', console, names, lower, upper);
end 
if (~isempty(indices{2})) % smoothing
  matbiips('set_gen_tree_smooth_monitors', console, names, lower, upper);
end  
if (~isempty(indices{3})) %backward_smoothing
  matbiips('set_backward_smooth_monitors', console, names, lower, upper);
end 
