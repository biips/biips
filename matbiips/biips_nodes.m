function nodes = biips_nodes(model, varargin)

%
% BIIPS_NODES returns the nodes of the current model
% nodes = biips_nodes(model)
%
%   INPUT
%   - model:    structure containing the model, 
%               returned by the 'biips_model' function
% 
%   Optional input parameters
%   - type:     string belonging to 'const', 'logic', 'stoch'  for
%               indicating which type of node is requested. default returns all types
%               of nodes
%   - observed: boolean for indicating which status observed/unobserved of
%               nodes is requested. default returns all observed status
%
%   OUTPUT
%   - nodes: structure containing the nodes
%
%   See also BIIPS_MODEL
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
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
nodes = matbiips('get_sorted_nodes', model.id);

% transform observed to logical
nodes.observed = nodes.observed==1;

% add samplers and iterations if sampler is built
if matbiips('is_sampler_built', model.id)
    samplers = matbiips('get_node_samplers', model.id);
    
    % merge samplers and nodes
    nodes = [fieldnames(nodes), struct2cell(nodes); fieldnames(samplers), struct2cell(samplers)].';
    nodes = struct(nodes{:});
end

% filter by type
if ~isempty(type)
    ind = cellfun(@(x) strcmp(x,type), nodes.type);
    nodes = structfun(@(x) x(ind), nodes, 'uniformoutput', false);
end

% filter by observed
if ~isempty(observed)
    ind = nodes.observed==observed;
    nodes = structfun(@(x) x(ind), nodes, 'uniformoutput', false);
end

% transform into table (if allowed)
if ~(isoctave() || verLessThan('matlab', '7.12'))
    nodes = struct2table(nodes);
end
