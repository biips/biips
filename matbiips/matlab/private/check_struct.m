function check_struct(obj, class)

%
% Checks if the structure is a valid object of class class
%

%% check arguments
if ~isstruct(obj)
    error('check_struct: 1-st argument must be a struct');
end
if ~ismember(class, {'biips', 'pimh', 'pmmh'})
    error('check_struct: 2-nd argument is invalid')
end

%% common checks
if ~isfield(obj, 'class')
    error('Invalid %s structure: missing field class', class);
end
if ~ischar(obj.class) || ~strcmp(obj.class, class)
    error('Invalid %s structure: invalid field class');
end

%% specific checks
switch class
    case 'biips'
        fields = {'id', 'model', 'file', 'data'};
        type = {'numeric', 'char', 'char', 'struct'};
        for i=1:numel(fields)
            if ~isfield(obj, fields{i})
                error('Invalid biips model structure: missing field %s', fields{i});
            elseif ~isa(obj.(fields{i}), type{i})
                error('Invalid biips model structure: field %s must be of type %s',...
                    obj.(fields{i}), type{i})
            end
        end
        
        %%% TODO check console id
    case 'pimh'
        check_struct(obj.model, 'biips')
    case 'pmmh'
        check_struct(obj.model, 'biips')
end
