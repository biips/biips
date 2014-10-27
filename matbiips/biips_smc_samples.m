function [smc_samples, log_marg_like] = biips_smc_samples(model, variable_names, n_part, varargin)
% BIIPS_SMC_SAMPLES Run a sequential Monte Carlo algorithm
%   [smc_samples, log_marg_like] = biips_smc_samples(model, variable_names,...
%        nb_part, 'PropertyName', PropertyValue, ...)
%   INPUT: 
%   - model:    Biips model as returned by the BIIPS_MODEL function
%   - variable_names: cell of strings. Contains the names of the 
%                      unobserved variables to monitor.
%                      Example: {'var1', 'var2[1]', 'var3[1:10]', 'var4[1, 5:10, 3]'}
%                      Subset indices must define a valid subset of 
%                      the variables of the model.
%   - n_part:         integer. Number of particles.
%
%   Optional Inputs:
%   - type:     string containing the characters 'f' (filtering), 's' (smoothing) 
%               and/or 'b' (backward smoothing). Select the type of particles to be monitored.
%               (default = 'fs')
%   - rs_thres:    real. Threshold for the adaptive SMC resampling. (default = 0.5)
%                   * if 'rs_thres' is in [0,1], resampling occurs when
%                     (ESS < rs_thres * n_part)
%                   * if 'rs_thres' is in [2,n_part], resampling occurs when
%                     (ESS < rs_thres)
%   - rs_type:     string. Name of the algorithm used for the SMC resampling.
%                   Possible values are 'stratified', 'systematic',
%                   'residual', 'multinomial'. (default = 'stratified')
%
%   OUTPUT:
%   - smc_samples:  output structure with one field for each monitored variable in
%                   the 'variable_names' argument. 
%   - log_marg_like: estimate of the log marginal likelihood.
%
%   Each field of smc_samples is a structure with one field for each type 
%   of monitoring ('f', 's' and/or 'b') in the 'type' argument.
%   Assuming dim is the dimension of the monitored variable, each type of
%   monitoring subfield is a structure with the fields: 
%     * values: array of size [dim, n_part] with the values of the particles.
%     * weights: array of size [dim, n_part] with the weights of the particles.
%     * ess: array of size dim with Effective Sample Sizes (ESS) of the particles set.
%     * discrete: array of size dim with booleans indicating discreteness of each component.
%     * iterations: array of size dim with sampling iterations of each component.
%     * conditionals: lists of the contitioning
%     variables (observations). Its value is:
%         - for filtering: a cell array of size dim. each element is a cell of strings
%           with the respective conditioning variables of the node array element.
%         - for smoothing/backward_smoothing: a cell of strings, the same for all the 
%           elements of the node array.
%     * name: string with the name of the variable.
%     * lower: vector with the lower bounds of the variable.
%     * upper: vector with the upper bounds of the variable.
%     * type: string with the type of monitor ('filtering', 'smoothing' or 'backward_smoothing').
%   
%   For instance, one can access the values of the smoothing particles for
%   the variable 'x' with: 
%       smc_samples.x.s.values
%
%   See also BIIPS_MODEL, BIIPS_DIAGNOSIS, BIIPS_SUMMARY, BIIPS_DENSITY,
%   BIIPS_TABLE
%--------------------------------------------------------------------------
% EXAMPLE:
% modelfile = 'hmm.bug';
% type(modelfile);
% 
% data = struct('tmax', 10, 'logtau', log(10));
% model = biips_model(modelfile, data, 'sample_data', true);
% 
% n_part = 50;
% 
% [out_smc, lml] = biips_smc_samples(model, {'x[1]', 'x[8:10]'}, n_part, 'type', 'fs', 'rs_thres', .5, 'rs_type', 'stratified');
% 
% out_smc
% biips_diagnosis(out_smc);
% summ_smc = biips_summary(out_smc)
% dens_smc = biips_density(out_smc)
% 
% out_smc2 = getfield(out_smc, 'x[8:10]')
% biips_diagnosis(out_smc2);
% summ_smc = biips_summary(out_smc2)
% dens_smc = biips_density(out_smc2)
% 
% out_smc2.f
% out_smc2.s
% biips_diagnosis(out_smc2.f);
% summ_smc = biips_summary(out_smc2.f)
% dens_smc = biips_density(out_smc2.f)
% 
% 
% [out_smc, lml] = biips_smc_samples(model, {'x'}, n_part);
% 
% out_smc.x.f
% out_smc.x.s
% 
% biips_diagnosis(out_smc);
% summ_smc = biips_summary(out_smc, 'order', 2, 'probs', [.025, .975]);
% 
% figure
% subplot(2,1,1); hold on
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
% dens_smc = biips_density(out_smc, 'bw_type', 'nrd0', 'adjust', 1, 'n', 100);
% 
% subplot(2,1,2); hold on
% plot(model.data.x_true(1), 0, 'g^', 'markerfacecolor', 'g')
% plot(dens_smc.x.f(1).x, dens_smc.x.f(1).f, 'b')
% plot(dens_smc.x.s(1).x, dens_smc.x.s(1).f, 'r')
% xlabel('x[1]')
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
optarg_names = {'type', 'rs_thres', 'rs_type'};
optarg_default = {'fs', .5, 'stratified'};
optarg_valid = {{'f', 's', 'b', 'fs', 'fb', 'sb', 'fsb'}, [0, n_part],...
    {'multinomial', 'stratified', 'residual', 'systematic'}};
optarg_type = {'char', 'numeric', 'char'};
[type, rs_thres, rs_type] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

indices = arrayfun(@(x) strfind(type,x), 'fsb', 'UniformOutput', 0);
filtering = ~isempty(indices{1});
smoothing = ~isempty(indices{2}); 
backward = ~isempty(indices{3});

check_struct(model, 'biips');
console = model.id; % Get the id of the biips console

%% Monitor
if (backward)
    matbiips('set_default_monitors', console);
end
if (~isempty(variable_names))
    monitor(console, variable_names, type); 
end

%% Run smc_sample
smc_forward_algo(console, n_part, rs_thres, rs_type, get_seed());

log_marg_like = matbiips('get_log_norm_const', console);


cz=cell(0,1);
if filtering % Get filtering output
    mon = matbiips('get_filter_monitors', console);
    names = fieldnames(mon);
    cz = struct2cell(mon);
    if (~backward)
       clear_monitors(console, 'f');
    end   
end

if smoothing % Get smoothing output
    mon = matbiips('get_gen_tree_smooth_monitors', console);
    names = fieldnames(mon);
    cz = horzcat(cz, struct2cell(mon));
    clear_monitors(console, 's');
end

if (backward) % Get backward smoothing output
   matbiips('run_backward_smoother', console);
   clear_monitors(console, 'f'); 
   mon = matbiips('get_backward_smooth_monitors', console);
   names = fieldnames(mon);    
   clear_monitors(console, 'b'); 
   cz = horzcat(cz, struct2cell(mon));
end

nb_names = numel(names);
smc_samples = cell(nb_names, 1);
for i=1:nb_names
   smc_samples{i} = cell2struct({cz{i, :}}, num2cell(type), 2);
end
smc_samples = cell2struct_weaknames(smc_samples, names); % Allows to add fields with brackets in the name
