function biips_clear(model)

%
% BIIPS_CLEAR  clears one or several biips consoles
%
%   biips_clear(model) clears the console associated to the biips model
% 
%   biips_clear()  clears all the consoles and reset the Id counter to 0.
%
%   See also BIIPS_MODEL
%--------------------------------------------------------------------------
% EXAMPLE
% biips_clear(0)
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 17-03-2014
%--------------------------------------------------------------------------



if nargin
    inter_biips('clear_console', model.id); 
else
    clear inter_biips
%     inter_biips('clear_all_console');
end
