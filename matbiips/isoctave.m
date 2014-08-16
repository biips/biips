function res = isoctave()

%
% ISOCTAVE checks if octave is running
%  res = isoctave() returns a boolean equal to 1 if octave is running, 0 otherwise
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, François Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

res = exist('OCTAVE_VERSION','builtin') ~= 0;
end
