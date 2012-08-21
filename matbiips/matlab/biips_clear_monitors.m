function biips_clear_monitors(p, type)
% CLEAR_MONITOR_BIIPS clear some monitors  
% 
indices = arrayfun(@(x) strfind(type, x), 'fsb', 'UniformOutput', 0); 
if (~isempty(indices{1})) % filtering
  inter_biips('clear_filter_monitors', p);
end 
if (~isempty(indices{2})) % smoothing
  inter_biips('clear_smooth_tree_monitors', p);
end  
if (~isempty(indices{3})) %backward_smoothing
  inter_biips('clear_smooth_monitors', p);
end 
