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
% data = struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1), 'logtau', log(1));
% model = biips_model(modelfile, data, 'sample_data', true);
% 
% %% SMC algorithm
% n_part = 100;
% [out_smc, lml] = biips_smc_samples(model, {'x', 'c[2:10]'}, n_part, 'type', 'fs', 'rs_thres', .5, 'rs_type', 'stratified');
% 
% summ_smc = biips_summary(out_smc)
% 
% summ_smc_x = biips_summary(out_smc.x, 'order', 2, 'probs', [.025, .975])
% 
% out_smc_c = getfield(out_smc, 'c[2:10]')
% summ_smc_c  = biips_summary(out_smc_c)
% 
% biips_summary(out_smc.x.f)
% biips_summary(out_smc.x.s)
% 
% figure
% subplot(2,1,1); hold on
% plot(model.data.x_true, 'g')
% plot(summ_smc_x.f.mean, 'b')
% plot(summ_smc_x.s.mean, 'r')
% plot([summ_smc_x.f.quant{:}], '--b')
% plot([summ_smc_x.s.quant{:}], '--r')
% xlabel('t')
% ylabel('x[t]')
% legend('true', 'SMC filtering estimate', 'SMC smoothing estimate')
% legend boxoff
% 
% subplot(2,1,2,'YTick',zeros(1,0)); hold on
% bar(1:data.tmax, 2+.5*(model.data.c_true==1), 'g', 'barwidth', 1, 'basevalue', 2, 'edgecolor', 'none')
% text(2, 2.75, 'true')
% bar(2:data.tmax, 1+.5*(summ_smc_c.f.mode==1), 'b', 'barwidth', 1, 'basevalue', 1, 'edgecolor', 'none')
% text(2, 1.75, 'SMC filtering mode')
% bar(2:data.tmax, .5*(summ_smc_c.s.mode==1), 'r', 'barwidth', 1, 'basevalue', 0, 'edgecolor', 'none')
% text(2, .75, 'SMC smoothing mode')
% xlim([1,data.tmax+1])
% ylim([0,3])
% xlabel('t')
% ylabel('c[t]==1')
% 
% %% PIMH algorithm
% n_part = 50;
% obj_pimh = biips_pimh_init(model, {'x', 'c[2:10]'}); % Initialize
% [obj_pimh, lml_pimh_burn] = biips_pimh_update(obj_pimh, 100, n_part); % Burn-in
% [obj_pimh, out_pimh, lml_pimh] = biips_pimh_samples(obj_pimh, 100, n_part); % Samples
% 
% summ_pimh = biips_summary(out_pimh)
% 
% summ_pimh_x = biips_summary(out_pimh.x, 'order', 2, 'probs', [.025, .975])
% 
% out_pimh_c = getfield(out_pimh, 'c[2:10]');
% summ_pimh_c = biips_summary(out_pimh_c)
% 
% figure
% subplot(2,1,1); hold on
% plot(model.data.x_true, 'g')
% plot(summ_pimh_x.mean, 'b')
% plot([summ_pimh_x.quant{:}], '--b')
% xlabel('t')
% ylabel('x[t]')
% legend('true', 'PIMH estimate')
% legend boxoff
% 
% subplot(2,1,2,'YTick',zeros(1,0)); hold on
% bar(1:data.tmax, 1+.5*(model.data.c_true==1), 'g', 'barwidth', 1, 'basevalue', 1, 'edgecolor', 'none')
% text(2, 1.75, 'true')
% bar(2:data.tmax, .5*(summ_pimh_c.mode==1), 'b', 'barwidth', 1, 'basevalue', 0, 'edgecolor', 'none')
% text(2, .75, 'PIMH mode')
% xlim([1,data.tmax+1])
% ylim([0,2])
% xlabel('t')
% ylabel('c[t]==1')
% 
% %% PMMH algorithm
% model = biips_model(modelfile, struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1)));
% 
% n_part = 50;
% obj_pmmh = biips_pmmh_init(model, {'logtau'}, 'latent_names', {'x', 'c[2:10]'}, 'inits', {-2}); % Initialize
% [obj_pmmh, plml_pmmh_burn] = biips_pmmh_update(obj_pmmh, 100, n_part); % Burn-in
% [obj_pmmh, out_pmmh, plml_pmmh] = biips_pmmh_samples(obj_pmmh, 100, n_part, 'thin', 1); % Samples
% 
% summ_pmmh = biips_summary(out_pmmh)
% 
% summ_pmmh_lt = biips_summary(out_pmmh.logtau, 'order', 2, 'probs', [.025, .975], 'mode', true)
% 
% summ_pmmh_x = biips_summary(out_pmmh.x, 'order', 2, 'probs', [.025, .975], 'mode', true)
% 
% out_pmmh_c  = getfield(out_pmmh, 'c[2:10]');
% summ_pmmh_c = biips_summary(out_pmmh_c)
% 
% figure
% subplot(2,1,1); hold on
% plot(model.data.x_true, 'g')
% plot(summ_pmmh_x.mean, 'b')
% plot([summ_pmmh_x.quant{:}], '--b')
% xlabel('t')
% ylabel('x[t]')
% legend('true', 'PMMH estimate')
% legend boxoff
% 
% subplot(2,1,2,'YTick',zeros(1,0)); hold on
% bar(1:data.tmax, 1+.5*(model.data.c_true==1), 'g', 'barwidth', 1, 'basevalue', 1, 'edgecolor', 'none')
% text(2, 1.75, 'true')
% bar(2:data.tmax, .5*(summ_pmmh_c.mode==1), 'b', 'barwidth', 1, 'basevalue', 0, 'edgecolor', 'none')
% text(2, .75, 'PMMH mode')
% xlim([1,data.tmax+1])
% ylim([0,2])
% xlabel('t')
% ylabel('c[t]==1')
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
