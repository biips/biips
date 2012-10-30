function [summ] = biips_density(parts, vars, varargin)
% BIIPS_MMARY compute some statistics on selected variables
opt_argin = length(varargin);
fsb = '';
probas = [];
if opt_argin >=1
   fsb = varargin{1}; 
end
if (isempty(vars))
   vars = fieldnames(parts); % vars = {}, take all fields
end   
presents = fieldnames(parts.(vars{1}));
if (isempty(fsb)) % retrieve only the field presents in the first variable
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
   particles = s.(vars{i}).(fsb(j));
   size_curr = size(particules.values));
   d = length(size_curr);
   ctemp{j} =  cell(size_curr(1:d-1));
   cellfun(@(x,w) kde(x,w) 
   ctemp{j} =  summary(s.(vars{i}).(fsb(j)),probas);
  end
  csum{i} = inter_biips('cell2struct_weak_names', ctemp, cell_fsb);
end
summ = inter_biips('cell2struct_weak_names', csum, vars);

