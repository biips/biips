function [summ] = biips_summary(parts, varargin)
% BIIPS_SUMMARY compute some statistics on selected variables
% summ = biips_summary(particles, [variables, fsb, probas])
% INPUT
% -particles : input structure containing the particles of different variables.
%              usually returned by biips_smc_samples function
% -variables : cell of strings. subset of the fields of particles struct
%              argument
% -fsb : string containing the characters 'f', 's' and/or 'b'
% -probas : vector of reals in ]0,1[. probability levels for quantiles.
%           default is [] for no quantile
% -order : integer. The maximum wanted order for the moment statistics.
%          default is 2
% OUTPUT
% -summ : output structure
opt_argin = length(varargin);
vars = {};
if opt_argin >=1
   vars = varargin{1}; 
end
fsb = '';
if opt_argin >=2
   fsb = varargin{2};
end
more_argin = {};
if opt_argin >=3
   more_argin = varargin{3:end};
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
   ctemp{j} =  summary(s.(vars{i}).(fsb(j)), more_argin{:});
  end
  cell_sum{i} = inter_biips('cell2struct_weak_names', ctemp, cell_fsb);
end
summ = inter_biips('cell2struct_weak_names', cell_sum, vars);
