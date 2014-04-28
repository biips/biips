function check_biips(model)

%
% Checks if the structure model is a valid biips model
%

%% main checks
if ~isstruct(model)
    error('Invalid biips model: must be a struct');
end
if ~isfield(model, 'class')
    error('Invalid biips model structure: missing field class');
end
if ~ischar(model.class) || ~strcmp(model.class, 'biips')
    error('Invalid biips model structure: invalid field class');
end

%% further checks
fields = {'id', 'definition', 'filename', 'data'};
type = {'numeric', 'char', 'char', 'struct'};
for i=1:numel(fields)
    if ~isfield(model, fields{i})
        error('Invalid biips model structure: missing field %s', fields{i});
    elseif ~isa(model.(fields{i}), type{i})
            error('Invalid biips model structure: field %s must be of type %s',...
                model.(fields{i}), type{i})
    end
end
