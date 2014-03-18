function rw = pmmh_rw_rescale(rw, p)

%--------------------------------------------------------------------------
% PMMH_RW_RESCALE rescale the randow walk variance
% rw = pmmh_rw_rescale(rw, p)
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, François Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

p = min(1.0, p);
rw.pmean = rw.pmean + 1*(p-rw.pmean)/rw.niter;


rw.lstep = rw.lstep + rw.alpha^(rw.niter)*(rw.pmean-rw.targetprob);


