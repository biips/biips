function obj = pmmh_rw_rescale(obj, ar)

%--------------------------------------------------------------------------
% PMMH_RW_RESCALE rescale the randow walk variance
% obj = pmmh_rw_rescale(obj, ar)
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

ar = min(1.0, ar);

obj.ar_mean = obj.ar_mean + 1*(ar-obj.ar_mean)/obj.n_iter;

obj.log_step = obj.log_step + obj.alpha^(obj.n_iter)*(obj.ar_mean-obj.target_prob);


