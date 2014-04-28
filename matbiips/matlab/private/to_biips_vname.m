function v = to_biips_vname(var)

%% remove spaces
v = strjoin(strsplit(var, ' '), '');

%%% FIXME transfrom variable name. eg: x[1, ] => x[1,1:100]