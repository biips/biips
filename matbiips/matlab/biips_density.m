function [dens] = biips_density(parts, vars, varargin)
% BIIPS_DENSITY computes 1D densities 
% dens = biips_density(particules, variables, [fsb])
% INPUT
% -particules : input structure containing the variables
% -vars : variable subselection cell 
% -fsb : string containing the characteres 'f', 's' and/or 'b'
% OUTPUT
% - dens : output structure
opt_argin = length(varargin);
fsb = '';
if opt_argin >=1
   fsb = varargin{1}; 
end
show = true;
if opt_argin >=2
   show = varargin{2}; 
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
   size_curr = size(particles.values);
   d = length(size_curr);
   ctemp{j} = cellfun(@(x,w) kde(x,w), num2cell(particles.values, d), num2cell(particles.weights, d));  
  end
  csum{i} = inter_biips('cell2struct_weak_names', ctemp, cell_fsb);
end
dens = inter_biips('cell2struct_weak_names', csum, vars);
