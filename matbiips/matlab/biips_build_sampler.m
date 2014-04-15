function biips_build_sampler(model, varargin)

%
% BIIPS_BUILD_SAMPLER assigns a sampler to every node of the graph
%   biips_build_sampler(console, 'PropertyName', PropertyValue)
%   INPUT: 
%   - model:        structure containing the model, 
%                   returned by the 'biips_model' function
%   Optional Input:
%   - proposal:string. Keyword defining the type of proposal desired.
%              Possible values are 'auto' and 'prior'. 'auto' selects the best sampler
%              among available ones automatically. 'prior' forces asignment of the prior
%              sampler to every node. 'prior' switches off lots of instructions and can
%              speed up the startup of the SMC for big models.
%              default is 'auto'
%
%   See also BIIPS_MODEL
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 17-03-2014
%--------------------------------------------------------------------------

%% PROCESS AND CHECK INPUTS
optarg_names = {'proposal'};
optarg_default = {'auto'};
optarg_valid = {{'auto', 'prior'}};
optarg_type = {'char'};
[proposal] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

%% Build sampler
prior=false;
if (strcmp(proposal, 'prior'))
    prior = true;
end
inter_biips('build_smc_sampler', model.id, prior);
