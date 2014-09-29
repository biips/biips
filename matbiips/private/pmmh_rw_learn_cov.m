function obj = pmmh_rw_learn_cov(obj)

%--------------------------------------------------------------------------
% PMMH_RW_LEARN_COV updates the empirical covariance of the samples
% obj = pmmh_rw_learn_cov(obj)
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

n_iter = obj.n_iter;
n_cov = obj.n_cov; 

% Concatenate all variables in a column vector
sample_vec_tr = cell2mat(cellfun(@(x) x(:), obj.sample_param_tr(:),...
    'UniformOutput', false));

if n_iter == n_cov + 1
    obj.rw_mean = sample_vec_tr;
    obj.rw_cov = sample_vec_tr*sample_vec_tr';    
elseif n_iter > n_cov + 1 % Recursive update of the empirical mean and covariance matrix
    q = (n_iter-n_cov-1)/(n_iter-n_cov);
    q2 = (n_iter-n_cov-1)/(n_iter-n_cov)^2;
    z = sample_vec_tr - obj.rw_mean;
    obj.rw_cov = q * obj.rw_cov + q2 * (z*z');
    obj.rw_mean = q*obj.rw_mean + 1/(n_iter-n_cov) * sample_vec_tr;
end

