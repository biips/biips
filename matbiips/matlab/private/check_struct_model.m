function check_struct_model(model)

%
% Checks if the structure model is a valid biips model
%

fields = {'id', 'definition', 'filename', 'data'};
type = {'numeric', 'char', 'char', 'struct'};
for i=1:length(fields)
    if ~isfield(model, fields{i})
        error('Invalid biips model structure: missing field %s', fields{i});
    else
        if ~isa(model.(fields{i}), type{i})
            error('Invalid biips model structure: field %s must be of type %s',...
                model.(fields{i}), type{i});
        end
    end
end