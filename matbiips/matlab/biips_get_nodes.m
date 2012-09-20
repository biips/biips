function nodes = biips_get_nodes(p)
% BIIPS_GET_NODES returns the nodes of the current model
%  nodes = biips_get_nodes(p)
% INPUT
%  -p : number of the current console
% OUTPUT
%  - nodes : cell containing the nodes: 
nodes= inter_biips('get_sorted_nodes', p);
