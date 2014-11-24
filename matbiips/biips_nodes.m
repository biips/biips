function nodes = biips_nodes(model, varargin)
% BIIPS_NODES Table of nodes of the Biips model.
% nodes = biips_nodes(model)
%
%   INPUT
%   - model:   Biips model as returned by the BIIPS_MODEL function.
% 
%   Optional input parameters
%   - type:     string. Return only a specific type of node. Possible values are
%               'const', 'logic' or 'stoch'. Default returns all types
%               of nodes.
%   - observed: logical. Return only observed or unobserved nodes.
%               Default returns all.
%
%   OUTPUT
%   - nodes: table containing the nodes with the following variables:
%             * id: integer. node ids
%             * name: string. node names
%             * type: string. node types ('const', 'logic' or 'stoch')
%             * observed: bool. true if the node is observed
%             * discrete: bool. true if the node is discrete
%     If the function is called after the sampler is built (by calling 
%     BIIPS_BUILD_SAMPLER or BIIPS_SMC_SAMPLES), it will also
%     contain the additional variables:
%             * iteration: integer. node sampling iteration of unobserved
%             nodes, starting at 0. 'NaN' if the node if observed
%             * sampler: string. node sampler name for stochastic unobserved
%             nodes. An empty string for other types of nodes
%     
%   The nodes are sorted in a topological order.
%
%   See also BIIPS_MODEL, BIIPS_BUILD_SAMPLER

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
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 21-10-2014
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

nodes.discrete = nodes.discrete==1;

% add samplers and iterations if sampler is built
if matbiips('is_sampler_built', model.id)
    names = fieldnames(nodes);
    fields = struct2cell(nodes);
    
    samplers = matbiips('get_node_samplers', model.id);
    
    names = [names; fieldnames(samplers)];
    fields = [fields; struct2cell(samplers)];
    
    % merge samplers and nodes
    nodes = struct;
    for i = 1:numel(names)
        nodes = setfield(nodes, names{i}, fields{i});
    end
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
if ~(isoctave() || verLessThan('matlab', '8.2'))
    nodes = struct2table(nodes);
end
