function [tab] = biips_table(samples, varargin)
% BIIPS_TABLE Univariate frequency tables or probability mass estimates of discrete variables
% tab = biips_table(samples, 'PropertyName', PropertyValue, ...)
%   INPUT:
%   - samples: structure containing either the output of a SMC algorithm
%              as returned by BIIPS_SMC_SAMPLES or the output of a MCMC algorithm
%              as returned by BIIPS_PIMH_SAMPLES or BIIPS_PMMH_SAMPLES
%   Optional inputs:
%   - type:             string containing the characters 'f' (fitering), 
%                       's' (smoothing) and/or 'b' (backward smoothing).
%                       Select fields of the input to be processed.
%                       (default = 'fsb').
%   OUTPUT
%   - tab:   structure with the same nested fields as the input
%             'samples' structure. Contains the following subfields:
%            * x: locations
%            * f: values of the mass at x
%
%--------------------------------------------------------------------------
% EXAMPLE:
% %%% TODO: add example to demo
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 21-10-2014
%--------------------------------------------------------------------------

%% PROCESS AND CHECK INPUTS
optarg_names = {'type'};
optarg_default = {'fsb'};
optarg_valid = {{'f', 's', 'b', 'fs', 'fb', 'sb', 'fsb'}};
optarg_type = {'char'};
[type] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

%%
is_mcmc = isnumeric(samples);
is_smc = ~is_mcmc && is_smc_array(samples);
is_smc_fsb = ~is_mcmc && ~is_smc && has_fsb_fields(samples);

if is_mcmc
    %% samples corresponds to the output of a MCMC algorithm
    d = ndims(samples);
    
    tab = cellfun(@(x) wtd_table(x), num2cell(samples, d));
elseif is_smc
    %% samples corresponds to the output of a SMC algorithm
    d = ndims(samples.values);
    tab = cellfun(@(x,w) wtd_table(x, w), num2cell(samples.values, d), num2cell(samples.weights, d));
elseif is_smc_fsb
    %% samples corresponds to the output of a SMC algorithm with f,s,b fields
    names = fieldnames(samples);
    tab = struct();
    
    for i=1:numel(names)
        fsb = names{i};
        if isempty(strfind(type, fsb))
            continue
        end
        s = getfield(samples, fsb);
        d = ndims(s.values);
        tab_s = cellfun(@(x,w) wtd_table(x, w), num2cell(s.values, d), num2cell(s.weights, d));
        tab = setfield(tab, fsb, tab_s);
    end
else
    if ~isstruct(samples)
        error('samples must either be a numeric array or a struct')
    end
    %% samples contains several variables
    varnames = fieldnames(samples);
    tab = cell(size(varnames));
    for i=1:numel(varnames)
        s = getfield(samples, varnames{i});
        tab{i} = biips_table(s, varargin{:});
    end
    tab = cell2struct_weaknames(tab, varnames);
end

end

function [tab] = wtd_table(values, weights)
if nargin<2
    tab.x = sort(unique(values));
    tab.f = histc(values, tab.x)/numel(values);
else
    tab = matbiips('wtd_table', values, weights);
end
end