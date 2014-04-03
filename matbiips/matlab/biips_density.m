function [dens] = biips_density(samples, varargin)

%
% BIIPS_DENSITY computes 1D kernel density estimates
% dens = biips_density(samples, 'PropertyName', PropertyValue, ...)
%   INPUT
%   - samples:  input structure containing either the output of a SMC algorithm
%               or of a PIMH/PMMH algorithm.
%   Optional inputs
%   - variable_names:   cell of strings. subset of the fields of particles struct
%                       argument. default is {} for all present fields in parts
%   - type:             string containing the characters 'f', 's' and/or 'b'.
%                       default is '' for all present fields in particles
%   - adjust:           time factor for the bw. default is 1
%   - bw:               bandwidth. default is estimated from the samples
%   OUTPUT
%   - dens:             output structure
%--------------------------------------------------------------------------
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
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, François Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 17-03-2014
%--------------------------------------------------------------------------

%% PROCESS AND CHECK INPUTS
optarg_names = {'variable_names', 'type', 'adjust', 'bw'};
optarg_default = {{}, '', 1, []};
optarg_valid = {{}, {'', 'f', 's', 'b', 'fs', 'fb', 'sb', 'fsb'}, [0, 10],...
    [0,intmax]};
optarg_type = {'char', 'char', 'numeric', 'numeric'};
[variable_names, type, adjust, bw] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

if (isempty(variable_names))
   variable_names = fieldnames(samples); % take all fields
end

%% Density estimates
if isstruct(getfield(samples, variable_names{1})) % samples corresponds to the output of a SMC algorithm

	if (isempty(type))
	    type='fsb';
	end
	
	% retrieve only the field presents in the first variable
	present = fieldnames(getfield(samples, variable_names{1}));
	present = strcat(present{:});
	indices = arrayfun(@(x) strfind(present, x), type, 'UniformOutput', 0);
	indices = sort([indices{:}]);
	type = present(indices);

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
      samp = getfield(s, variable_names{i});
      weights = 1/nsamples * ones(size(samp));
      size_curr = size(samp);
       d = length(size_curr);
      cell_sum{i} = cellfun(@(x,w) kde(x, w, adjust, bw), num2cell(samp, d), num2cell(weights, d));
    end
    dens = cell2struct_weaknames(cell_sum, variable_names);
end
