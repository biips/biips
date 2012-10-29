function [summ] = biips_summary(parts, vars, varargin)
% BIIPS_SUMMARY compute some statistics on selected variables
opt_argin = length(varargin);
fsb = '';
if opt_argin >=1
   fsb = varargin{1}; 
end
if (isempty(vars))
   vars = names % vars = {}, take all fields
end   
if (isempty(fsb)) % retrieve only the field presents in the first variable
   presents = fieldnames(names{1});
   chaine='fsb';
   indices=arrayfun(@(x) strfind(strcat(presents{:}),x), chaine, 'UniformOutput', 0);
   indices=horzcat(indices{:});
   fsb=chaine(sort(indices));
end
% select only the wanted variables
s = inter_biips('cell2struct_weak_names', cellfun(@(x) parts.(x), vars,'UniformOutput',0), vars);
cell_fsb = num2cell(fsb);
cell_sum = cell(size(vars));

for i=1:length(vars)
  ctemp = cell(size(fsb));
  for j=1:length(fsb)
   ctemp{j} =  summary(s.(vars{i}).(fsb(j)),[0.025, 0.975]);
  end
  csum{i} = inter_biips('cell2struct_weak_names', ctemp, cell_fsb);
end
summ = inter_biips('cell2struct_weak_names', csum, vars);
