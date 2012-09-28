function biips_init(varargin)
% BIIPS_INIT do some initialization
verb = 1;
if (~isempty(varargin))
    verb = varargin{1};
end
inter_biips('verbosity', verb);
biips_load_module('basemod');
