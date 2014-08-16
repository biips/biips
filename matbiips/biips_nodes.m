function nodes = biips_nodes(model, varargin)

%
% BIIPS_NODES returns the nodes of the current model
% nodes = biips_nodes(model)
%
%   INPUT
%   - model:        structure containing the model, 
%                   returned by the 'biips_model' function
%
%   OUTPUT
%   - nodes: structure containing the nodes
%
%   See also BIIPS_MODEL
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

check_struct(model, 'biips'); % Checks if the structure model is valid

%%% Process and check optional arguments
optarg_names = {'type', 'observed'};
optarg_default = {{}, []};
optarg_valid = {{'const', 'logic', 'stoch'}, {true, false}};
optarg_type = {'char', 'logical'};
[type, observed] = parsevar(varargin, optarg_names,...
    optarg_type, optarg_valid, optarg_default);

% get nodes and transform into table
nodes = struct2table(matbiips('get_sorted_nodes', model.id));

nodes.observed = nodes.observed==1;

% add samplers and iterations if sampler is built
if matbiips('is_sampler_built', model.id)
    samplers = struct2table(matbiips('get_node_samplers', model.id));
    nodes = [nodes, samplers];
end

% filter by type
if ~isempty(type)
    ind = cellfun(@(x) strcmp(x,type), nodes.type);
    nodes = nodes(ind,:);
end

% filter by observed
if ~isempty(observed)
    nodes = nodes(nodes.observed==observed,:);
end

