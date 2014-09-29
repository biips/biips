function v = to_biips_vname(var)

if ~ischar(var) || numel(var)==0
    error('invalid variable name')
end
%% remove spaces
v = regexprep(var, ' ', '');

%%% FIXME transform variable name. eg: x[1, ] => x[1,1:100]