function biips_build_sampler(model, varargin)
% BIIPS_BUILD_SAMPLER Assign a sampler to each node of the graph.
%   biips_build_sampler(model, 'PropertyName', PropertyValue)
%   INPUT: 
%   - model:    Biips model as returned by the BIIPS_MODEL function
%   Optional Input:
%   - proposal: string. The type of proposal used by the SMC algorithm.
%               Possible values are 'auto' and 'prior'. 'auto' selects the best sampler
%               among available ones automatically. 'prior' forces asignment of the prior
%               sampler to every node. 'prior' switches off lots of instructions and can
%               speed up the startup of the SMC for large models.
%               (default = 'auto')
%
%   In order to specify the proposal used by the SMC algorithm,
%   this function has to be called before BIIPS_SMC_SAMPLES.
%   Otherwise, it will be automatically called by BIIPS_SMC_SAMPLES 
%   with the default parameters.
% 
%   See also BIIPS_SMC_SAMPLES, BIIPS_NODES

%--------------------------------------------------------------------------
% EXAMPLE:
% modelfile = 'hmm.bug';
% type(modelfile);
% 
% data = struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1), 'logtau', log(1));
% model = biips_model(modelfile, data, 'sample_data', true);
% 
% biips_nodes(model)
% 
% biips_build_sampler(model, 'proposal', 'prior')
% biips_nodes(model, 'type', 'stoch', 'observed', false)
% 
% biips_build_sampler(model, 'proposal', 'auto')
% biips_nodes(model, 'type', 'stoch', 'observed', false)
% 
% n_part = 100;
% [out_smc, lml] = biips_smc_samples(model, {'x', 'c[2:10]'}, n_part, 'type', 'fs', 'rs_thres', .5, 'rs_type', 'stratified');
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 21-10-2014
%--------------------------------------------------------------------------

%% PROCESS AND CHECK INPUTS
optarg_names = {'proposal'};
optarg_default = {'auto'};
optarg_valid = {{'auto', 'prior'}};
optarg_type = {'char'};
[proposal] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

check_struct(model, 'biips');

%% Build sampler
prior=false;
if (strcmp(proposal, 'prior'))
    prior = true;
end
matbiips('build_smc_sampler', model.id, prior);
