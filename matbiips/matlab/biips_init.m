function biips_init(varargin)

%--------------------------------------------------------------------------
% BIIPS_INIT loads the biips module
% biips_init(verb) loads the biips basemod module and set the verbosity
%
% Optional input
%   verb: integer which sets the verbosity level
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, François Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

verb = 1;
if (~isempty(varargin))
    verb = varargin{1};
end
inter_biips('verbosity', verb);

% load module
ok = inter_biips('load_module', 'basemod');
if (~ok)
    error('problem loading module'); 
end
