function [ varname ] = deparse_varname( name, lower, upper )
%DEPARSE_VARNAME returns a string of the variable name including bounds
%   Detailed explanation goes here

if ~ischar(name) || numel(name)==0
    error('invalid variable name')
end

varname = name;

if ~isempty(lower) || ~isempty(upper)
    if ~isnumeric(lower) || ~isnumeric(upper) || numel(lower)~=numel(upper) || any(upper < lower)
        error('invalid variable bounds')
    end
    
    varname = [varname, '['];
        
    for i=1:numel(lower)
        varname = [varname, num2str(lower(i))];
        if lower(i) ~= upper(i)
            varname = [varname, ':', num2str(upper(i))];
        end
        if i~=numel(lower)
            varname = [varname, ','];
        end
    end
    
    varname = [varname, ']'];
end
end