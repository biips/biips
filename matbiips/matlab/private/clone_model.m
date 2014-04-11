function model2 = clone_model(model)

%
% Creates a clone of the model
%
filename = model.filename;
data = model.data;
model2 = biips_model(filename, data, 'sample_data', false);