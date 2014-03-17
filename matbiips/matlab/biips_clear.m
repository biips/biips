function biips_clear(console)

%--------------------------------------------------------------------------
% BIIPS_CLEAR  clears one or several biips consoles
%
%   biips_smc_samples(console) clears the biips consoles given in the vector console
% 
%   biips_clear()  will clear all the consoles and reset the Id counter to 0.
%
%   See also BIIPS_MODEL
%--------------------------------------------------------------------------
% EXAMPLE
% biips_clear(1)
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, François Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 17-03-2014
%--------------------------------------------------------------------------

if nargin
    if ~isnumeric(console)
        error('The 1-st argument must be a numeric vector')
    else
        for i =1:length(console)
            inter_biips('clear_console', console(i));            
        end
    end    
else
    inter_biips('clear_all_console');
end
