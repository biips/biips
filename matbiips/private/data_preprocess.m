function [ data ] = data_preprocess( data )
%DATA_PREPROCESS preprocess the data for Biips
%   Error if the objects given name are not found or not allowed in Biips.
%   remove non-numeric elements


if iscell(data)
    data = data(:);
    
    isch = cellfun(@ischar, data);
    if ~all(isch)
        error('non character elements in ''data'' cell argument.');
    end
    
    % remove duplicated names
    data = unique(data);
    
    ispres = cellfun(@(x) evalin('base', ['exist(''' x ''', ''var'')']), data);
    if ~all(ispres)
        miss = data(~ispres);
        varnames = sprintf('%s ', miss{:});
        error('variables not found: %s', varnames);
    end
    
    data = cell2struct(cellfun(@(x) evalin('base',x), data, 'UniformOutput', false), data, 1);
elseif isstruct(data)
    if numel(data)>1
        error('''data'' argument is a non-scalar structure array.')
    end
else
    error('''data'' argument must either be a struct or a cell of strings.')
end


%% check names are legal biips variable names
names = fieldnames(data);
for i = 1:numel(names)
    if ~is_legal_vname(names{i})
        stop(['data with name ', names{i}, ' is not allowed in Biips'])
    end
end

%% remove those not numeric data
isnum = structfun(@isnumeric, data);
if ~all(isnum)
    v = fieldnames(data);
    ignored = v(~isnum);
    data = rmfield(data, ignored);
    warning('Ignored non numericvariables in data: %s ', sprintf('%s ', ignored{:}));
end


end

