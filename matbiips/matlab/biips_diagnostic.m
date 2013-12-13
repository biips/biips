function [diagn] = biips_diagnostic(parts, varargin)
% BIIPS_DIAGNOSTIC returns a diagnostic structure
% diagn = biips_diagnostic(particles, [variables, fsb, ess_thres, quiet])
% INPUT
% -particles : input structure containing the particles of different variables.
%              usually returned by biips_smc_samples function
% -variables : cell of strings. subset of the fields of particles struct
%              argument
% -fsb : string containing the characters 'f', 's' and/or 'b'
% -ess_thres : integer. Threshold on the Effective Sample Size (ESS) of the
%              examined particles. If all the ESS components are over the
%              threshold, the diagnostic is valid, otherwise it is not
%              valid.
%              default is 30
% -quiet : flag. deactivate message display. default is 0
% OUTPUT
% -diagn : output structure
opt_argin = length(varargin);
vars = {};
if opt_argin >=1
    vars = varargin{1};
end
fsb = '';
if opt_argin >=2
    fsb = varargin{2};
end
ess_thres = 30;
if opt_argin >=3
    ess_thres = varargin{3};
end
quiet = 0;
if opt_argin >=4
    quiet = varargin{4};
end

if (isempty(vars))
    vars = fieldnames(parts); % vars = {}, take all fields
end
presents = fieldnames(getfield(parts, vars{1}));
if (isempty(fsb)) % retrieve only the field presents in the first variable
    chaine='fsb';
    indices=arrayfun(@(x) strfind(strcat(presents{:}),x), chaine, 'UniformOutput', 0);
    indices=horzcat(indices{:});
    fsb=chaine(sort(indices));
end
% select only the wanted variables
s = biips_cell2struct(cellfun(@(x) getfield(parts, x), vars,'UniformOutput',0), vars);
cell_fsb = num2cell(fsb);
cell_diagn = cell(size(vars));

for i=1:length(vars)
    if ~quiet
        disp(['* Diagnosing variable: ' , vars{i}]);
    end
    ctemp = cell(size(fsb));
    for j=1:length(fsb)
        ctemp{j} =  diagnostic_biips(getfield(getfield(s, vars{i}), fsb(j)), ess_thres, quiet, fsb(j));
    end
    cell_diagn{i} = biips_cell2struct(ctemp, cell_fsb);
end
diagn = biips_cell2struct(cell_diagn, vars);
