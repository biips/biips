function varnames = biips_get_variable_names(model)

%
% BIIPS_GET_VARIABLE_NAMES returns the variable names of the current model
%  variable_names = biips_get_variable_names(p)
%
%   INPUT
%   - model:        structure containing the model, 
%                   returned by the 'biips_model' function
%
%   OUTPUT
%   -variable_names: cell containing the current variable names 
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

varnames = inter_biips('get_variable_names', model.id);
