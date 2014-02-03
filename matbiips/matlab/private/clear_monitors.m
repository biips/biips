function clear_monitors(p, type, release_only)
% CLEAR_MONITORS clear some monitors  
% clear_monitors(console, type, [release_only])
% INPUT : 
% - console : id of the current console
% - type : string containing 'f' - forward
%                            's' - smoothing
%                            'b' - backward smoothing
% - release_only : boolean flag to indicate what kind of 
% clearing has to be done

% opt_argin = length(varargin);
%default values

if nargin<3
    release_only = false;
end

indices = arrayfun(@(x) strfind(type, x), 'fsb', 'UniformOutput', 0); 
if (~isempty(indices{1})) % filtering
  inter_biips('clear_filter_monitors', p, release_only);
end 
if (~isempty(indices{2})) % smoothing
  inter_biips('clear_gen_tree_smooth_monitors', p, release_only);
end  
if (~isempty(indices{3})) %backward_smoothing
  inter_biips('clear_backward_smooth_monitors', p, release_only);
end 