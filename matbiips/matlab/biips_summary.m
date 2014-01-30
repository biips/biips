function [summ] = biips_summary(parts, varargin)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% BIIPS_SUMMARY compute some statistics on selected variables
% summ = biips_summary(parts, 'Propertyname', propertyvalue, ...)
%
% INPUT
% -parts:       input structure containing the particles of different variables.
%               usually returned by biips_smc_samples function
% Optional inputs
% -variables:   cell of strings. subset of the fields of particles struct
%               argument. Default is all.
% -fsb:         string containing the characters 'f', 's' and/or 'b'
% -probs:      vector of reals in ]0,1[. probability levels for quantiles.
%               default is [] for no quantile
% -order:       integer. Moment statistics of order below or equal to the
%               value are returned. Default is 2.
% OUTPUT
% -summ :       output structure
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% EXAMPLE:
% data = struct('var1', 0, 'var2', 1.2);
% model_id = biips_model('model.bug', data)
% npart = 100; variables = {'x'}; 
% out_smc = biips_smc_samples(model_id, variables, npart);
% summ = biips_summary(out_smc, 'probs', [.025, .975]);
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
vars = {};
fsb = '';
probs = [];
order = 2;
parsevar; % Process options

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
cell_sum = cell(size(vars));

for i=1:length(vars)
  ctemp = cell(size(fsb));
  for j=1:length(fsb)
   ctemp{j} =  summary(getfield(getfield(s, vars{i}), fsb(j)), probs, order);
  end
  cell_sum{i} = biips_cell2struct(ctemp, cell_fsb);
end
summ = biips_cell2struct(cell_sum, vars);
