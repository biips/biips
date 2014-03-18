function varnames = biips_get_variable_names(p)

%--------------------------------------------------------------------------
% BIIPS_GET_VARIABLE_NAMES returns the variable names of the current model
%  variable_names = biips_get_variable_names(p)
%
%   INPUT
%   -p: number of the current console
%
%   OUTPUT
%   -variable_names: cell containing the current variable names 
%
%   See also BIIPS_MODEL
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, François Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

varnames= inter_biips('get_variable_names', p);
