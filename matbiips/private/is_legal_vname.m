function [ is_legal ] = is_legal_vname( name )
%IS_LEGAL_VNAME check if vname is a legal BUGS variable name
%   Detailed explanation goes here

if ~ischar(name) || numel(name)==0
    error('invalid variable name: %s', name)
end

is_legal = strcmp(regexp(name, '[a-zA-Z][\.\w]*', 'match'), name);

end

