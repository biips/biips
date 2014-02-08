function biips_init(varargin)
% BIIPS_INIT do some initialization
% biips_init(verb)
% INPUT 
% verb : integer which set the verbosity level
% this function loads the basemod module and set the verbosity
verb = 1;
if (~isempty(varargin))
    verb = varargin{1};
end
inter_biips('verbosity', verb);

% load module
ok = inter_biips('load_module', 'basemod');
if (~ok)
    error('problem loading module'); 
end
