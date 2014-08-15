function model = clone_model(model)

%
% Creates a clone of the model
%

%% make console and check model
id = matbiips('make_console');
matbiips('check_model', id, model.file)

%% discard unused data
varnames = matbiips('get_variable_names', id);
v = fieldnames(model.data);
unused = setdiff(v, varnames);
data = rmfield(model.data, unused);
if (numel(unused) > 0)
    warning('Unused variables in data: %s ', sprintf('%s ', unused{:}));
end

%% compile model
matbiips('compile_model', id, data, false, get_seed(), true);

%% data after possible sampling (from 'data' block in the BUGS language model)
model_data = matbiips('get_data', id);

% Biips model structure
model.id = id;
model.data = model_data;

model.class = 'biips';