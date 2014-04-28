function check_pmmh(obj)

%
% Checks if the structure obj is a valid pmmh structure
%

%% main checks
if ~isstruct(obj)
    error('Invalid pmmh: must be a struct');
end
if ~isfield(obj, 'class')
    error('Invalid pmmh structure: missing field class');
end
if ~ischar(obj.class) || ~strcmp(obj.class, 'pmmh')
    error('Invalid pmmh structure: invalid field class');
end
