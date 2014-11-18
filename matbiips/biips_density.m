function [dens] = biips_density(samples, varargin)
% BIIPS_DENSITY Univariate marginal kernel density estimates.
%   dens = biips_density(samples, 'PropertyName', PropertyValue, ...)
%   INPUT:
%   - samples: structure containing either the output of a SMC algorithm
%              as returned by BIIPS_SMC_SAMPLES or the output of a MCMC algorithm
%              as returned by BIIPS_PIMH_SAMPLES or BIIPS_PMMH_SAMPLES
%   Optional inputs:
%   - type:             string containing the characters 'f' (fitering), 
%                       's' (smoothing) and/or 'b' (backward smoothing).
%                       Select fields of the input to be processed.
%                       (default = 'fsb').
%   - adjust:           scale factor for the bandwidth. (default = 1)
%   - bw:               positive real. The kernel bandwidth. default is estimated from the samples
%   - bw_type:          string. The type of bandwidth selector 
%                       used in case 'bw' value is not given. Possible values:
%                           * 'nrd0' (default): Silverman's rule of thumb [1]
%                           * 'nrd': Scott's variation of Silverman with factor 1.06 [2]
%                           * 'matlab': Matlab's rule from the stat toolbox KSDENSITY function
%   - n:                integer. nb of points of evaluation. (default = 100)
%   OUTPUTOUTPUT
%   - dens:             structure with the same nested fields as the input
%                       'samples' structure. Contains the following subfields:
%                           * x: points of the density (n points in the range [min-4*bw, max+4*bw]
%                           * f: values of the density at x
%   
%   See also BIIPS_SMC_SAMPLES, BIIPS_PIMH_SAMPLES, BIIPS_PMMH_SAMPLES
%
%   References:
%   [1] Silverman, B. W. (1986) Density Estimation. London: Chapman and Hall
%   (page 48, eqn (3.31))
%   [2] Scott, D. W. (1992) Multivariate Density Estimation: Theory,
%   Practice, and Visualization. Wiley.
 
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
% dens_smc_x = biips_density(out_smc.x, 'bw_type', 'nrd0', 'adjust', 1, 'n', 100)
% 
% dens_smc_x_f = biips_density(out_smc.x.f)
% 
% t = 5;
% figure; hold on
% plot(model.data.x_true(t), 0, 'g^', 'markerfacecolor', 'g')
% plot(dens_smc_x.f(t).x, dens_smc_x.f(t).f, 'b')
% plot(dens_smc_x.s(t).x, dens_smc_x.s(t).f, 'r')
% xlabel(sprintf('x[%d]', t))
% ylabel('posterior density')
% 
% %% PIMH algorithm
% n_part = 50;
% obj_pimh = biips_pimh_init(model, {'x', 'c[2:10]'}); % Initialize
% [obj_pimh, lml_pimh_burn] = biips_pimh_update(obj_pimh, 100, n_part); % Burn-in
% [obj_pimh, out_pimh, lml_pimh] = biips_pimh_samples(obj_pimh, 100, n_part); % Samples
% 
% dens_pimh_x = biips_density(out_pimh.x)
% 
% figure; hold on
% plot(model.data.x_true(t), 0, 'g^', 'markerfacecolor', 'g')
% plot(dens_pimh_x(t).x, dens_pimh_x(t).f, 'b')
% xlabel(sprintf('x[%d]', t))
% ylabel('posterior density')
% 
% %% PMMH algorithm
% data = struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1));
% model = biips_model(modelfile, data);
% 
% n_part = 50;
% obj_pmmh = biips_pmmh_init(model, {'logtau'}, 'latent_names', {'x', 'c[2:10]'}, 'inits', {-2}); % Initialize
% [obj_pmmh, plml_pmmh_burn] = biips_pmmh_update(obj_pmmh, 100, n_part); % Burn-in
% [obj_pmmh, out_pmmh, plml_pmmh] = biips_pmmh_samples(obj_pmmh, 100, n_part, 'thin', 1); % Samples
% 
% dens_pmmh_lt = biips_density(out_pmmh.logtau)
% 
% dens_pmmh_x = biips_density(out_pmmh.x)
% 
% figure; hold on
% plot(model.data.logtau_true, 0, '^g', 'markerfacecolor', 'g')
% plot(dens_pmmh_lt.x, dens_pmmh_lt.f, 'b')
% xlabel('logtau')
% ylabel('posterior density')
% 
% t = 5;
% figure; hold on
% plot(model.data.x_true(t), 0, 'g^', 'markerfacecolor', 'g')
% plot(dens_pmmh_x(t).x, dens_pmmh_x(t).f, 'b')
% xlabel(sprintf('x[%d]', t))
% ylabel('posterior density')
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 21-10-2014
%--------------------------------------------------------------------------

%% PROCESS AND CHECK INPUTS
optarg_names = {'type', 'adjust', 'bw', 'bw_type', 'n'};
optarg_default = {'fsb', 1, [], 'nrd0', 500};
optarg_valid = {{'f', 's', 'b', 'fs', 'fb', 'sb', 'fsb'}, [0, 10],...
    [0,intmax], {'nrd0', 'nrd', 'matlab'}, [0,intmax]};
optarg_type = {'char', 'numeric', 'numeric', 'char', 'numeric'};
[type, adjust, bw, bw_type, npoints] = parsevar(varargin, optarg_names, optarg_type,...
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
%         bw = cellfun(@(x,w) bw_select(x, w, bw_type), num2cell(samples, d), num2cell(weights, d), 'uniformoutput', false);
        bw = cellfun(@(x) bw_select(x, [], bw_type), num2cell(samples, d), 'uniformoutput', false);
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
%         bw = cellfun(@(x,w) bw_select(x, w, bw_type), num2cell(samples.values, d), num2cell(samples.weights, d), 'uniformoutput', false);
        bw = cellfun(@(x) bw_select(x, [], bw_type), num2cell(samples.values, d), 'uniformoutput', false);
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
    
    % same bandwidth for all f,s,b estimates
    if isempty(bw)
        if isfield(samples, 'f') % use filtering particles first
            s = getfield(samples, 'f');
        elseif isfield(samples, 'f') % then backward smoothing
            s = getfield(samples, 'b');
        else % otherwise smoothing
            s = getfield(samples, 's');
        end
        d = ndims(s.values);
%         bw = cellfun(@(x,w) bw_select(x, w, bw_type), num2cell(s.values, d), num2cell(s.weights, d), 'uniformoutput', false);
        bw = cellfun(@(x) bw_select(x, [], bw_type), num2cell(s.values, d), 'uniformoutput', false);
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
