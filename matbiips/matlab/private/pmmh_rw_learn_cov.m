function rw = pmmh_rw_learn_cov(rw, sample_param)

%--------------------------------------------------------------------------
% PMMH_RW_LEARN_COV updates the empirical covariance of the samples
% rw = pmmh_rw_learn_cov(rw, sample_param)
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, François Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

niter = rw.niter;
ncov = rw.ncov; 

% Concatenate all variables in a column vector
sample_vec = cell2mat(cellfun(@(x) x(:) , sample_param(:),...
    'UniformOutput', false));

if (niter == ncov + 1)
    rw.mean = sample_vec;
    rw.cov = sample_vec*sample_vec';    
elseif (niter > ncov + 1) % Recursive update of the empirical mean and covariance matrix
    rw.cov = (niter-ncov-1)/(niter-ncov) * rw.cov ...
        + (niter-ncov-1)/(niter-ncov)^2*(sample_vec - rw.mean)*(sample_vec - rw.mean)';
    rw.mean = (niter-ncov-1)/(niter-ncov)*rw.mean + 1/(niter-ncov) * sample_vec;
end

