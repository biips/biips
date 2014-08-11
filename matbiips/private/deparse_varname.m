function [ varname ] = deparse_varname( name, lower, upper )
%DEPARSE_VARNAME returns a string of the variable name including bounds
%   Detailed explanation goes here

varname = name;

if ~isempty(lower)
    varname = [varname, '['];
end


for i=1:numel(lower)
    varname = [varname, num2str(lower(i))];
    if lower(i) ~= upper(i)
        varname = [varname, ':', num2str(upper(i))];
    end
    if i~=numel(lower)
        varname = [varname, ','];
    end
end


if ~isempty(lower)
    varname = [varname, ']'];
end