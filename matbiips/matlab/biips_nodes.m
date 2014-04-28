function nodes = biips_nodes(model)

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
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

check_biips(model); % Checks if the structure model is valid

nodes = matbiips('get_sorted_nodes', model.id);
