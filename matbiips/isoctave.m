function res = isoctave()
% ISOCTAVE Check if octave is running.
%  res = isoctave()
%  OUTPUT: logical. true if octave is running, false otherwise.

%--------------------------------------------------------------------------
% EXAMPLE:
% % Set the random numbers generator seed for reproducibility
% if isoctave() || verLessThan('matlab', '7.12')
%     rand('state', 0)
% else
%     rng('default')
% end
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
