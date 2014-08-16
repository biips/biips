function [summ] = biips_summary(samples, varargin)

%
% BIIPS_SUMMARY computes some statistics on selected variables
%  summ = biips_summary(samples, 'Propertyname', propertyvalue, ...)
%
%   INPUT
%   - samples:      input structure containing either the output of a SMC algorithm
%                   or of a PIMH/PMMH algorithm.
%   Optional inputs
%   - type:         string containing the characters 'f', 's' and/or 'b'
%   - probs:        vector of reals in ]0,1[. probability levels for quantiles.
%                   default is [] for no quantile
%   - order:        integer. Moment statistics of order below or equal to the
%                   value are returned. Default is 1.
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

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

order_default = 1;
is_mcmc = ~isstruct(samples);
is_smc = ~is_mcmc && has_fsb_fields(samples);
if is_mcmc
    mode = all(floor(samples) == samples);
elseif is_smc
    names = fieldnames(samples);
    s = getfield(samples, names{1});
    mode = all(s.discrete(:));
    % by default, do not return the mean if all the components are discrete
    if mode
        order_default = 0;
    end
end

%% PROCESS AND CHECK INPUTS
    
optarg_names = {'type', 'probs', 'order'};
optarg_default = {'fsb', [], order_default};
optarg_valid = {{'f', 's', 'b', 'fs', 'fb', 'sb', 'fsb'}, [0, 1],...
    [0,4]};
optarg_type = {'char', 'numeric', 'numeric'};
[type, probs, order] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

if is_mcmc
    %% corresponds to the output of a MCMC algorithm
    dim = size(samples);
    nsamples = dim(end);
    weights = 1/nsamples * ones(dim);
    summ = wtd_stat(samples, weights, probs, order, mode);
elseif is_smc
    %% corresponds to the output of a SMC algorithm
    names = fieldnames(samples);
    summ = struct();
    
    for i=1:numel(names)
        fsb = names{i};
        if ~strfind(type, fsb)
            continue
        end
        s = getfield(samples, fsb);
        summ_s = wtd_stat(s.values, s.weights, probs, order, mode);
        summ = setfield(summ, fsb, summ_s);
    end
else
    %% contains several variables
    varnames = fieldnames(samples);
    summ = cell(size(varnames));
    for i=1:numel(varnames)
        s = getfield(samples, varnames{i});
        summ{i} = biips_summary(s, varargin{:});
    end
    summ = cell2struct_weaknames(summ, varnames);
end

end
