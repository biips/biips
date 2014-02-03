function [dens] = biips_density(parts, varargin)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% BIIPS_DENSITY computes 1D densities 
% dens = biips_density(particles, [variables, fsb, adjust, bw])
% INPUT
% -particles: input structure containing the particles of different variables.
%              usually returned by biips_smc_samples function
% -variables: cell of strings. subset of the fields of particles struct
%         argument. default is {} for all present fields in particles
% -fsb: string containing the characters 'f', 's' and/or 'b'.
%        default is '' for all present fields in particles
% -adjust: time factor for the bw. default is 1
% -bw: bandwidth. default is estimated from the samples
% OUTPUT
% -dens : output structure
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% EXAMPLE:
% data = struct('var1', 0, 'var2', 1.2);
% model_id = biips_model('model.bug', data)
% npart = 100; variables = {'x'}; 
% out_smc = biips_smc_samples(model_id, variables, npart);
% diag = biips_density(out_smc, variables);% 
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
% Jan 2014; Last revision: 30-01-2014
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%% Default values
vars = {};
fsb = '';
adjust = 1;
bw = [];
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
s = cell2struct_weaknames(cellfun(@(x) getfield(parts, x), vars,'UniformOutput',0), vars);
cell_fsb = num2cell(fsb);
cell_sum = cell(size(vars));

for i=1:length(vars)
  ctemp = cell(size(fsb));
  for j=1:length(fsb)
   particles = getfield(getfield(s, vars{i}), fsb(j));
   size_curr = size(particles.values);
   d = length(size_curr);
   ctemp{j} = cellfun(@(x,w) kde(x, w, adjust, bw), num2cell(particles.values, d), num2cell(particles.weights, d));  
  end
  cell_sum{i} = cell2struct_weaknames(ctemp, cell_fsb);
end
dens = cell2struct_weaknames(cell_sum, vars);
