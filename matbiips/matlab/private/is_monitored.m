function ok = is_monitored(p, variable_names, type, check_released)

% FRANCOIS: A VERIFIER - PB AVEC  is_gen_tree_smooth_monitored


pn = cellfun(@(x) parse_varname(x), variable_names); 
names = {pn.name};
lower = {pn.lower};
upper = {pn.upper};
indices = arrayfun(@(x) strfind(type, x), 'fsb', 'UniformOutput', 0); 
  
if (~isempty(indices{1})) % filtering
    error('Function is_filter_monitored not implemented in inter_biips.cpp')
end 
if (~isempty(indices{2})) % smoothing
%     p, names, lower, upper
  ok = inter_biips('is_gen_tree_smooth_monitored', p, names, lower, upper)
end  
if (~isempty(indices{3})) %backward_smoothing
  error('Function is_backward_smooth_monitored not implemented in inter_biips.cpp')
end