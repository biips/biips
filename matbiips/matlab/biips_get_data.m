function data = biips_get_data(model)

%
% BIIPS_GET_DATA returns the data of the current model
% data = biips_get_data(model)
%
%   INPUT
%   - model:        structure containing the model, 
%                   returned by the 'biips_model' function
%
%   OUTPUT
%   - data: cell containing the current data 
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

data = matbiips('get_data', model);
