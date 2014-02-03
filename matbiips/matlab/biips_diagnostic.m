function [diagn] = biips_diagnostic(parts, varargin)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% BIIPS_DIAGNOSTIC returns a diagnostic structure
% diagn = biips_diagnostic(parts, 'Propertyname', propertyvalue, ...)
%
% INPUT
% -parts:      input structure containing the particles of different variables.
%               usually returned by biips_smc_samples function
% Optional Inputs:
% -variable_names:cell of strings. subset of the fields of particles struct
%               argument
% -type:        string containing the characters 'f', 's' and/or 'b'
% -ess_thres :  integer. Threshold on the Effective Sample Size (ESS) of the
%               examined particles. If all the ESS components are over the
%               threshold, the diagnostic is valid, otherwise it is not
%               valid.
%               default is 30
% -quiet:      flag. deactivate message display. default is 0
%
% OUTPUT
% -diagn:      output structure providing the minimum value of the
%              effective sample size
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% EXAMPLE:
% data = struct('var1', 0, 'var2', 1.2);
% model_id = biips_model('model.bug', data)
% npart = 100; variables = {'x'}; 
% out_smc = biips_smc_samples(model_id, variables, npart);
% diag = biips_diagnostic(out_smc);% 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% BiiPS Project - Bayesian Inference with interacting Particle Systems
%
% Reference: A. Todeschini, M. Fuentes, F. Caron, P. Legrand, P. Del Moral.
% BiiPS: a software for Bayesian inference with interacting particle
% systems. Technical Report, INRIA. February 2014.
%
% Authors: Adrien Todeschini, Marc Fuentes
% INRIA Bordeaux, France
% email: biips-project@lists.gforge.inria.fr
% Website: https://alea.bordeaux.inria.fr/biips
% Jan 2014; Last revision: 24-01-2014
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Default values
variable_names = {};
type = '';
ess_thres = 30;
quiet = 0;
parsevar; % Process options

if (isempty(variable_names))
    variable_names = fieldnames(parts); % vars = {}, take all fields
end
presents = fieldnames(getfield(parts, variable_names{1}));
if (isempty(type)) % retrieve only the field presents in the first variable
    chaine='fsb';
    indices=arrayfun(@(x) strfind(strcat(presents{:}),x), chaine, 'UniformOutput', 0);
    indices=horzcat(indices{:});
    type=chaine(sort(indices));
end
% Select only the wanted variables
s = cell2struct_weaknames(cellfun(@(x) getfield(parts, x), variable_names,'UniformOutput',0), variable_names);
cell_fsb = num2cell(type);
cell_diagn = cell(size(variable_names));

for i=1:length(variable_names)
    if ~quiet
        disp(['* Diagnosing variable: ' , variable_names{i}]);
    end
    ctemp = cell(size(type));
    for j=1:length(type)
        ctemp{j} =  diagnostic_biips(getfield(getfield(s, variable_names{i}), type(j)), ess_thres, quiet, type(j));
    end
    cell_diagn{i} = cell2struct_weaknames(ctemp, cell_fsb);
end
diagn = cell2struct_weaknames(cell_diagn, variable_names);