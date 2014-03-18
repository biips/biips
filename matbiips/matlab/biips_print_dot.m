function biips_print_dot(p, filename)

%--------------------------------------------------------------------------
% BIIPS_PRINT_DOT prints the graph in a file in dot format
%  biips_print_dot(p, filename)
%
%   INPUT
%   - p:        number of the current console
%   - filename: string 
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, François Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

inter_biips('print_graphviz', p, filename);