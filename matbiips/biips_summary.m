function [summ] = biips_summary(samples, varargin)
% BIIPS_SUMMARY Univariate marginal summary statistics.
%  summ = biips_summary(samples, 'Propertyname', propertyvalue, ...)
%
%   INPUT
%   - samples: structure containing either the output of a SMC algorithm
%              as returned by BIIPS_SMC_SAMPLES or the output of a MCMC algorithm
%              as returned by BIIPS_PIMH_SAMPLES or BIIPS_PMMH_SAMPLES
%   Optional inputs
%   - type:             string containing the characters 'f' (fitering), 
%                       's' (smoothing) and/or 'b' (backward smoothing).
%                       Select fields of the input to be processed.
%                       (default = 'fsb').
%   - probs:        vector of reals. probability levels in ]0,1[ for quantiles.
%                   (default = [])
%   - order:        integer. Moment statistics of order below or equal to the
%                   value are returned. (default = 0 if all the components are
%                   discrete variables and 1 otherwise)
%   - mode:         logical. Activate computation of the mode, i.e. the most
%                   frequent value among the particles. (default = true if all 
%                   the components are discrete variables and false otherwise)
%   OUTPUT
%   - summ:   structure with the same nested fields as the input
%             'samples' structure. Contains univariate marginal statistics.
%             The subfields can be:
%             * mean: if order>=1
%             * var: variance, if order>=2
%             * skew: skewness, if order>=3
%             * kurt: kurtosis, if order>=4
%             * probs: vector of quantile probabilities
%             * quant: cell of quantile values, if probs is not empty
%             * mode: most frequent values for discrete components
% 
%   See also BIIPS_SMC_SAMPLES, BIIPS_PIMH_SAMPLES, BIIPS_PMMH_SAMPLES,
%   BIIPS_DENSITY, BIIPS_TABLE
%--------------------------------------------------------------------------
% EXAMPLE:
% modelfile = 'hmm.bug';
% type(modelfile);
% 
% data = struct('tmax', 10, 'logtau', log(10));
% model = biips_model(modelfile, data, 'sample_data', true);
% 
% %% SMC algorithm
% n_part = 50;
% 
% [out_smc, lml] = biips_smc_samples(model, {'x[1]', 'x[8:10]'}, n_part, 'type', 'fs', 'rs_thres', .5, 'rs_type', 'stratified');
% summ_smc = biips_summary(out_smc)
% out_smc2 = getfield(out_smc, 'x[8:10]')
% summ_smc = biips_summary(out_smc2)
% summ_smc = biips_summary(out_smc2.f)
% 
% [out_smc, lml] = biips_smc_samples(model, {'x'}, n_part);
% summ_smc = biips_summary(out_smc, 'order', 2, 'probs', [.025, .975]);
% 
% hold on
% plot(model.data.x_true, 'g')
% plot(summ_smc.x.f.mean, 'b')
% plot(summ_smc.x.s.mean, 'r')
% plot(summ_smc.x.f.quant{1}, '--b')
% plot(summ_smc.x.f.quant{2}, '--b')
% plot(summ_smc.x.s.quant{1}, '--r')
% plot(summ_smc.x.s.quant{2}, '--r')
% xlabel('t')
% ylabel('x[t]')
% legend('true', 'SMC filtering estimate', 'SMC smoothing estimate')
% legend boxoff
% 
% %% PIMH algorithm
% n_part = 50;
% obj_pimh = biips_pimh_init(model, {'x'}); % Initialize
% [obj_pimh, lml_pimh_burn] = biips_pimh_update(obj_pimh, 100, n_part); % Burn-in
% [obj_pimh, out_pimh, lml_pimh] = biips_pimh_samples(obj_pimh, 100, n_part); % Samples
% 
% summ_pimh = biips_summary(out_pimh.x)
% summ_pimh = biips_summary(out_pimh, 'order', 2, 'probs', [.025, .975]);
% 
% subplot(2,2,3); hold on
% plot(model.data.x_true, 'g')
% plot(summ_pimh.x.mean, 'b')
% plot(summ_pimh.x.quant{1}, '--b')
% plot(summ_pimh.x.quant{2}, '--b')
% xlabel('t')
% ylabel('x[t]')
% legend('true', 'PIMH estimate')
% legend boxoff
% 
% %% PMMH algorithm
% modelfile = 'hmm.bug';
% logtau_true = 10;
% data = struct('tmax', 10);
% model = biips_model(modelfile, data, 'sample_data', true);
% 
% n_part = 50;
% obj_pmmh = biips_pmmh_init(model, {'logtau'}, 'latent_names', {'x'}, 'inits', {-2}); % Initialize
% [obj_pmmh, plml_pmmh_burn] = biips_pmmh_update(obj_pmmh, 100, n_part); % Burn-in
% [obj_pmmh, out_pmmh, plml_pmmh] = biips_pmmh_samples(obj_pmmh, 100, n_part, 'thin', 1); % Samples
% 
% summ_pmmh = biips_summary(out_pmmh.x)
% summ_pmmh = biips_summary(out_pmmh, 'order', 2, 'probs', [.025, .975]);
% 
% subplot(2,2,3); hold on
% plot(model.data.x_true, 'g')
% plot(summ_pmmh.x.mean, 'b')
% plot(summ_pmmh.x.quant{1}, '--b')
% plot(summ_pmmh.x.quant{2}, '--b')
% xlabel('t')
% ylabel('x[t]')
% legend('true', 'PMMH estimate')
% legend boxoff
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 21-10-2014
%--------------------------------------------------------------------------

order_default = 1;
mode_default = false;
is_mcmc = isnumeric(samples);
is_smc = ~is_mcmc && is_smc_array(samples);
is_smc_fsb = ~is_mcmc && ~is_smc && has_fsb_fields(samples);
if is_mcmc
    mode_default = all(floor(samples(:)) == samples(:));
elseif is_smc
    mode_default = all(samples.discrete(:));
    % by default, do not return the mean if all the components are discrete
    if mode_default
        order_default = 0;
    end
elseif is_smc_fsb
    names = fieldnames(samples);
    s = getfield(samples, names{1});
    mode_default = all(s.discrete(:));
    % by default, do not return the mean if all the components are discrete
    if mode_default
        order_default = 0;
    end
elseif ~isstruct(samples)
    error('samples must either be a numeric array or a struct')
end

%% PROCESS AND CHECK INPUTS
    
optarg_names = {'type', 'probs', 'order', 'mode'};
optarg_default = {'fsb', [], order_default, mode_default};
optarg_valid = {{'f', 's', 'b', 'fs', 'fb', 'sb', 'fsb'}, [0, 1],...
    [0,4], {true, false}};
optarg_type = {'char', 'numeric', 'numeric', 'logical'};
[type, probs, order, mode] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

if is_mcmc
    %% corresponds to the output of a MCMC algorithm
    dim = size(samples);
    nsamples = dim(end);
    weights = 1/nsamples * ones(dim);
    summ = wtd_stat(samples, weights, probs, order, mode);
elseif is_smc
    %% corresponds to the output of a SMC algorithm
    summ = wtd_stat(samples.values, samples.weights, probs, order, mode);
elseif is_smc_fsb
    %% corresponds to the output of a SMC algorithm with f,s,b fields
    names = fieldnames(samples);
    summ = struct();
    
    for i=1:numel(names)
        fsb = names{i};
        if isempty(strfind(type, fsb))
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
