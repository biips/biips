function ok = is_monitored(p, variable_names, type, check_released)

pn = cellfun(@(x) parse_varname(x), variable_names);
names = {pn.name};
lower = {pn.lower};
upper = {pn.upper};
indices = arrayfun(@(x) strfind(type, x), 'fsb', 'UniformOutput', 0);

if (~isempty(indices{1})) % filtering
    error('Function is_filter_monitored not implemented in matbiips.cpp')
end
if (~isempty(indices{2})) % smoothing
    ok = matbiips('is_gen_tree_smooth_monitored', p, names, lower, upper, check_released);
end
if (~isempty(indices{3})) %backward_smoothing
    error('Function is_backward_smooth_monitored not implemented in matbiips.cpp')
end