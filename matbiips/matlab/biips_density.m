function [dens] = biips_density(samples, varargin)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% BIIPS_DENSITY computes 1D densities 
% dens = biips_density(particles, [variables, fsb, adjust, bw])
% INPUT
% -samples:     input structure containing either the output of a SMC algorithm
%               or of a PIMH/PMMH algorithm.
% Optional inputs
% -variable_names:cell of strings. subset of the fields of particles struct
%               argument. default is {} for all present fields in parts
% -type:         string containing the characters 'f', 's' and/or 'b'.
%               default is '' for all present fields in particles
% -adjust:      time factor for the bw. default is 1
% -bw:          bandwidth. default is estimated from the samples
% OUTPUT
% -dens :       output structure
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% EXAMPLE:
% data = struct('var1', 0, 'var2', 1.2);
% model_id = biips_model('model.bug', data)
% n_part = 100; variables = {'x'}; 
% out_smc = biips_smc_samples(model_id, variables, n_part);
% kde_estimates = biips_density(out_smc); 
%
% n_iter = 100;
% out_pimh = biips_pimh_samples(model_id, variables, n_iter, n_part);
% kde_estimates_pimh = biips_density(out_pimh);
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
% Jan 2014; Last revision: 03-02-2014
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%% Default values
variable_names = {};
type = '';
adjust = 1;
bw = [];
parsevar; % Process options

if (isempty(variable_names))
   variable_names = fieldnames(samples); % take all fields
end


if isstruct(getfield(samples, variable_names{1})) % samples corresponds to the output of a SMC algorithm
    presents = fieldnames(getfield(samples, variable_names{1}));
    if (isempty(type)) % retrieve only the field presents in the first variable
       chaine='fsb';
       indices=arrayfun(@(x) strfind(strcat(presents{:}),x), chaine, 'UniformOutput', 0);
       indices=horzcat(indices{:});
       type=chaine(sort(indices));
    end
    % select only the wanted variables
    s = cell2struct_weaknames(cellfun(@(x) getfield(samples, x), variable_names,...
        'UniformOutput',0), variable_names);
    cell_fsb = num2cell(type);
    cell_sum = cell(size(variable_names));

    for i=1:length(variable_names)
      ctemp = cell(size(type));
      for j=1:length(type)
       particles = getfield(getfield(s, variable_names{i}), type(j));
       size_curr = size(particles.values);
       d = length(size_curr);
       ctemp{j} = cellfun(@(x,w) kde(x, w, adjust, bw), num2cell(particles.values, d), num2cell(particles.weights, d));  
      end
      cell_sum{i} = cell2struct_weaknames(ctemp, cell_fsb);
    end
    dens = cell2struct_weaknames(cell_sum, variable_names);
else % samples corresponds to the output of a MCMC algorithm
    % select only the wanted variables
    s = cell2struct_weaknames(cellfun(@(x) getfield(samples, x), variable_names,...
        'UniformOutput',0), variable_names);
    nsamples = size(getfield(s, variable_names{1}), ndims(getfield(s, variable_names{1})));
    cell_sum = cell(size(variable_names));
    for i=1:length(variable_names)
      ctemp = cell(size(type));
      for j=1:length(type)
       particles = getfield(getfield(s, variable_names{i}), type(j));
       size_curr = size(particles.values);
       d = length(size_curr);
       ctemp{j} = cellfun(@(x,w) kde(x, w, adjust, bw), num2cell(particles.values, d), num2cell(particles.weights, d));  
      end
      samp = getfield(s, variable_names{i});
      weights = 1/nsamples * ones(size(samp));
      size_curr = size(samp);
       d = length(size_curr);
      cell_sum{i} = cellfun(@(x,w) kde(x, w, adjust, bw), num2cell(samp, d), num2cell(weights, d));
    end
    dens = cell2struct_weaknames(cell_sum, variable_names);
end
