function biips_print_dot(model, filename)

%
% BIIPS_PRINT_DOT prints the graph in a file in dot format
%  biips_print_dot(model, filename)
%
%   INPUT
%   - model:        structure containing the model, 
%                   returned by the 'biips_model' function
%   - filename: string 
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

check_struct(model, 'biips');

matbiips('print_graphviz', model.id, filename);