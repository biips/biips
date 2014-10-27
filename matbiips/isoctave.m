function res = isoctave()
% ISOCTAVE Check if octave is running.
%  res = isoctave()
%  OUTPUT: boolean. true if octave is running, false otherwise.
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

res = exist('OCTAVE_VERSION','builtin') ~= 0;
end
