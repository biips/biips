function data = biips_get_data(p)

%
% BIIPS_GET_DATA returns the data of the current model
% data = biips_get_data(p)
%
%   INPUT
%   - p: number of the current console
%
%   OUTPUT
%   - data: cell containing the current data 
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

data= inter_biips('get_data', p);
