function check_pimh(obj)

%
% Checks if the structure obj is a valid pimh structure
%

%% main checks
if ~isstruct(obj)
    error('Invalid pimh: must be a struct');
end
if ~isfield(obj, 'class')
    error('Invalid pimh structure: missing field class');
end
if ~ischar(obj.class) || ~strcmp(obj.class, 'pimh')
    error('Invalid pimh structure: invalid field class');
end
