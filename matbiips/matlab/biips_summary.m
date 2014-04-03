function [summ] = biips_summary(samples, varargin)

%
% BIIPS_SUMMARY computes some statistics on selected variables
%  summ = biips_summary(parts, 'Propertyname', propertyvalue, ...)
%
%   INPUT
%   - samples:      input structure containing either the output of a SMC algorithm
%                   or of a PIMH/PMMH algorithm.
%   Optional inputs
%   - variable_names: cell of strings. subset of the fields of particles struct
%                   argument. Default is all.
%   - type:         string containing the characters 'f', 's' and/or 'b'
%   - probs:        vector of reals in ]0,1[. probability levels for quantiles.
%                   default is [] for no quantile
%   - order:        integer. Moment statistics of order below or equal to the
%                   value are returned. Default is 2.
%   OUTPUT
%   - summ:         output structure
%
%   See also BIIPS_SMC_SAMPLES, BIIPS_PIMH_SAMPLES, BIIPS_PMMH_SAMPLES
%--------------------------------------------------------------------------
% EXAMPLE:
% data = struct('var1', 0, 'var2', 1.2);
% model_id = biips_model('model.bug', data)
%
% n_part = 100; variables = {'x'}; 
% out_smc = biips_smc_samples(model_id, variables, n_part);
% summ = biips_summary(out_smc, 'probs', [.025, .975]);
%
% n_iter = 100;
% out_pimh = biips_pimh_samples(model_id, variables, n_iter, n_part)
% summ_pimh = biips_summary(out_pimh, 'probs', [.025, .975]);
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, François Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

%% PROCESS AND CHECK INPUTS
optarg_names = {'variable_names', 'type', 'probs', 'order'};
optarg_default = {{}, '', [], 2};
optarg_valid = {{}, {'', 'f', 's', 'b', 'fs', 'fb', 'sb', 'fsb'}, [0, 1],...
    [1,4]};
optarg_type = {'char', 'char', 'numeric', 'numeric'};
[variable_names, type, probs, order] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

if (isempty(variable_names))
   variable_names = fieldnames(samples); % vars = {}, take all fields
end

%% Summary statistics
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
    s = cell2struct_weaknames(cellfun(@(x) getfield(samples, x), variable_names,'UniformOutput',0), variable_names);
    cell_fsb = num2cell(type);
    cell_sum = cell(size(variable_names));

    for i=1:length(variable_names)
      ctemp = cell(size(type));
      for j=1:length(type)
          ctemp{j} = summary(getfield(getfield(getfield(s, variable_names{i}), type(j)), 'values'), ...
             getfield(getfield(getfield(s,variable_names{i}), type(j)), 'weights'), probs, order);
      end
      cell_sum{i} = cell2struct_weaknames(ctemp, cell_fsb);
    end
    summ = cell2struct_weaknames(cell_sum, variable_names);
    
else % samples corresponds to the output of a MCMC algorithm
    % select only the wanted variables
    s = cell2struct_weaknames(cellfun(@(x) getfield(samples, x), variable_names, 'UniformOutput',0), variable_names);    
    nsamples = size(getfield(s, variable_names{1}), ndims(getfield(s, variable_names{1})));
    cell_sum = cell(size(variable_names));
    for i=1:length(variable_names)  
        weights = 1/nsamples * ones(size(getfield(s, variable_names{i})));
        cell_sum{i} = summary(getfield(s, variable_names{i}), weights, probs, order);
    end
    summ = cell2struct_weaknames(cell_sum, variable_names);
end
