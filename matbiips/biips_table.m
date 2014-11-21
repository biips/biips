function [tab] = biips_table(samples, varargin)
% BIIPS_TABLE Univariate frequency tables or probability mass estimates of discrete variables.
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
%   See also biips_density

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
% out_smc_c = getfield(out_smc, 'c[2:10]')
% table_smc_c = biips_table(out_smc_c)
% 
% biips_table(out_smc_c.f)
% biips_table(out_smc_c.s)
% 
% figure; hold on
% bar(table_smc_c.f(t-1).x, table_smc_c.f(t-1).f, 'b', 'barwidth', .1)
% bar(table_smc_c.s(t-1).x+.1, table_smc_c.s(t-1).f, 'r', 'barwidth', .1)
% plot(model.data.c_true(t), 0, 'g^', 'markerfacecolor', 'g')
% xlabel(sprintf('c[%d]', t))
% ylabel('posterior probability mass')
% 
% %% PIMH algorithm
% n_part = 50;
% obj_pimh = biips_pimh_init(model, {'x', 'c[2:10]'}); % Initialize
% [obj_pimh, lml_pimh_burn] = biips_pimh_update(obj_pimh, 100, n_part); % Burn-in
% [obj_pimh, out_pimh, lml_pimh] = biips_pimh_samples(obj_pimh, 100, n_part); % Samples
% 
% out_pimh_c  = getfield(out_pimh, 'c[2:10]');
% table_pimh_c = biips_table(out_pimh_c)
% 
% figure; hold on
% bar(table_pimh_c(t-1).x, table_pimh_c(t-1).f, 'b', 'barwidth', .1)
% plot(model.data.c_true(t), 0, 'g^', 'markerfacecolor', 'g')
% xlabel(sprintf('c[%d]', t))
% ylabel('posterior probability mass')
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
% out_pmmh_c  = getfield(out_pmmh, 'c[2:10]');
% table_pmmh_c = biips_table(out_pmmh_c)
% 
% figure; hold on
% bar(table_pmmh_c(t-1).x, table_pmmh_c(t-1).f, 'b', 'barwidth', .1)
% plot(model.data.c_true(t), 0, 'g^', 'markerfacecolor', 'g')
% xlabel(sprintf('c[%d]', t))
% ylabel('posterior probability mass')
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
    tab.x = unique(values);
    tab.f = histc(values, tab.x)/numel(values);
else
    [tab.x, ~, ind] = unique(values);
    tab.f = bsxfun(@eq, repmat((1:numel(tab.x))', 1, numel(ind)), ind') * (weights(:)*numel(weights));
    tab.f = tab.f/sum(tab.f);
    
%     tab = matbiips('wtd_table', values, weights);
end
end