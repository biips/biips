function biips_clear_monitors(p, type, varargin)
% BIIPS_CLEAR_MONITORS clear some monitors  
% biips_clear_monitors(console, type, [release_only])
% INPUT : 
% - console : id of the current console
% - type : string containing 'f' - forward
%                            's' - smoothing
%                            'b' - backward smoothing
% - release_only : boolean flag to indicate what kind of 
% clearing has to be done
opt_argin = length(varargin);
%default values
release_only = false;
if opt_argin >= 1
   sample_data = varargin{1};
end

indices = arrayfun(@(x) strfind(type, x), 'fsb', 'UniformOutput', 0); 
if (~isempty(indices{1})) % filtering
  inter_biips('clear_filter_monitors', p, release_only);
end 
if (~isempty(indices{2})) % smoothing
  inter_biips('clear_smooth_tree_monitors', p, release_only);
end  
if (~isempty(indices{3})) %backward_smoothing
  inter_biips('clear_smooth_monitors', p, release_only);
end 
