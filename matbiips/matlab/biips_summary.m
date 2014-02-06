function [summ] = biips_summary(samples, varargin)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% BIIPS_SUMMARY compute some statistics on selected variables
% summ = biips_summary(parts, 'Propertyname', propertyvalue, ...)
%
% INPUT
% -samples:     input structure containing either the output of a SMC algorithm
%               or of a PIMH/PMMH algorithm.
% Optional inputs
% -variable_names:cell of strings. subset of the fields of particles struct
%               argument. Default is all.
% -type:         string containing the characters 'f', 's' and/or 'b'
% -probs:       vector of reals in ]0,1[. probability levels for quantiles.
%               default is [] for no quantile
% -order:       integer. Moment statistics of order below or equal to the
%               value are returned. Default is 2.
% OUTPUT
% -summ :       output structure
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
probs = [];
order = 2;
parsevar; % Process options

if (isempty(variable_names))
   variable_names = fieldnames(samples); % vars = {}, take all fields
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
    s = cell2struct_weaknames(cellfun(@(x) getfield(samples, x), variable_names,'UniformOutput',0), variable_names);
    cell_fsb = num2cell(type);
    cell_sum = cell(size(variable_names));

    for i=1:length(variable_names)
      ctemp = cell(size(type));
      for j=1:length(type)
          ctemp{j} = summary(getfield(getfield(getfield(s, variable_names{i}), type(j)), values), ...
             getfield(getfield(getfield(s,variable_names{i}), type(j)), weights), probs, order);
      end
      cell_sum{i} = cell2struct_weaknames(ctemp, cell_fsb);
    end
    summ = cell2struct_weaknames(cell_sum, variable_names);
    
else % samples corresponds to the output of a MCMC algorithm
    % select only the wanted variables
    s = cell2struct_weaknames(cellfun(@(x) getfield(samples, x), variable_names,'UniformOutput',0), variable_names);    
    nsamples = size(getfield(s, variable_names{1}), ndims(getfield(s, variable_names{1})));
    cell_sum = cell(size(variable_names));
    for i=1:length(variable_names)  
        weights = 1/nsamples * ones(size(getfield(s, variable_names{i})));
        cell_sum{i} = summary(getfield(s, variable_names{i}), weights, probs, order);
    end
    summ = cell2struct_weaknames(cell_sum, variable_names);
end
