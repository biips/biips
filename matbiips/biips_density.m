function [dens] = biips_density(samples, varargin)

%
% BIIPS_DENSITY computes 1D kernel density estimates
% dens = biips_density(samples, 'PropertyName', PropertyValue, ...)
%   INPUT
%   - samples:  input structure containing either the output of a SMC algorithm
%               or of a PIMH/PMMH algorithm.
%   Optional inputs
%   - type:             string containing the characters 'f', 's' and/or 'b'.
%                       default is '' for all present fields in particles
%   - adjust:           time factor for the bw. default is 1
%   - bw:               bandwidth. default is estimated from the samples
%   - n:                nb of points of evaluation. default is 100
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

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 17-03-2014
%--------------------------------------------------------------------------

%% PROCESS AND CHECK INPUTS
optarg_names = {'type', 'adjust', 'bw', 'n'};
optarg_default = {'fsb', 1, [], 100};
optarg_valid = {{'f', 's', 'b', 'fs', 'fb', 'sb', 'fsb'}, [0, 10],...
    [0,intmax], [0,intmax]};
optarg_type = {'char', 'numeric', 'numeric', 'numeric'};
[type, adjust, bw, npoints] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

%%
is_mcmc = isnumeric(samples);
is_smc = ~is_mcmc && is_smc_array(samples);
is_smc_fsb = ~is_mcmc && ~is_smc && has_fsb_fields(samples);

if is_mcmc
    %% samples corresponds to the output of a MCMC algorithm
    dim = size(samples);
    nsamples = dim(end);
    weights = 1/nsamples * ones(dim);
    d = ndims(samples);
    if isempty(bw)
        bw = cellfun(@(x,w) bw_select(x, w), num2cell(samples, d), num2cell(weights, d), 'uniformoutput', false);
    end
    if iscell(bw)
        dens = cellfun(@(x,w,b) kde(x, w, adjust*b, npoints), num2cell(samples, d), num2cell(weights, d), bw);
        [dens.bw] = bw{:};
    else
        dens = cellfun(@(x,w) kde(x, w, adjust*bw, npoints), num2cell(samples, d), num2cell(weights, d));
        bw = num2cell(repmat(bw, numel(dens), 1));
        [dens.bw] = bw{:};
    end
elseif is_smc
    %% samples corresponds to the output of a SMC algorithm
    d = ndims(samples.values);
    if isempty(bw)
        bw = cellfun(@(x,w) bw_select(x, w), num2cell(samples.values, d), num2cell(samples.weights, d), 'uniformoutput', false);
    end
    if iscell(bw)
        dens = cellfun(@(x,w,b) kde(x, w, adjust*b, npoints), num2cell(samples.values, d), num2cell(samples.weights, d), bw);
        [dens.bw] = bw{:};
    else
        dens = cellfun(@(x,w) kde(x, w, adjust*bw, npoints), num2cell(samples.values, d), num2cell(samples.weights, d));
        bw = num2cell(repmat(bw, numel(dens), 1));
        [dens.bw] = bw{:};
    end
elseif is_smc_fsb
    %% samples corresponds to the output of a SMC algorithm with f,s,b fields
    names = fieldnames(samples);
    dens = struct();
    
    if isempty(bw)
        % select bandwidth with filtering particles
        d = ndims(samples.f.values);
        bw = cellfun(@(x,w) bw_select(x, w), num2cell(samples.f.values, d), num2cell(samples.f.weights, d), 'uniformoutput', false);
    end
        
    for i=1:numel(names)
        fsb = names{i};
        if isempty(strfind(type, fsb))
            continue
        end
        s = getfield(samples, fsb);
        d = ndims(s.values);
        if iscell(bw)
            dens_s = cellfun(@(x,w,b) kde(x, w, adjust*b, npoints), num2cell(s.values, d), num2cell(s.weights, d), bw);
            [dens_s.bw] = bw{:};
        else
            dens_s = cellfun(@(x,w) kde(x, w, adjust*bw, npoints), num2cell(s.values, d), num2cell(s.weights, d));
            bw = num2cell(repmat(bw, numel(dens_s), 1));
            [dens_s.bw] = bw{:};
        end
        dens = setfield(dens, fsb, dens_s);
    end
else
    if ~isstruct(samples)
        error('samples must either be a numeric array or a struct')
    end
    %% samples contains several variables
    varnames = fieldnames(samples);
    dens = cell(size(varnames));
    for i=1:numel(varnames)
        s = getfield(samples, varnames{i});
        dens{i} = biips_density(s, varargin{:});
    end
    dens = cell2struct_weaknames(dens, varnames);
end
