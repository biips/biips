function s= biips_cell2struct(fields, weak_names)
% BIIPS_CELL2STRUCT build a structure with an "illegal" names
% biips_cell2struct(fields, weak_names) 
% INPUT 
% - fields : cell containing of the field to put into the structure
% - weak_names : cell of strings
% OUTPUT
% - s : the structure with "illegal" names
if (~isa(fields,'cell'))
    error('biips_cell2struct : 1-st argument must be a cell of strings')
end

if (~isa(weak_names,'cell'))
    error('biips_cell2struct : 2-st argument must be a cell of strings')
end

if (~all(cellfun(@(x) ischar(x), weak_names)))
    error('biips_cell2struct : 2-st argument must be a cell of strings')
end


s = inter_biips('cell2struct_weak_names', fields, weak_names);
