function obj = pmmh_rw_learn_cov(obj)

%--------------------------------------------------------------------------
% PMMH_RW_LEARN_COV updates the empirical covariance of the samples
% obj = pmmh_rw_learn_cov(obj)
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

sample_param = obj.sample_param;
n_iter = obj.n_iter;
n_cov = obj.n_cov; 

% Concatenate all variables in a column vector
sample_vec = cell2mat(cellfun(@(x) x(:) , sample_param(:),...
    'UniformOutput', false));

if n_iter == n_cov + 1
    obj.rw_mean = sample_vec;
    obj.rw_cov = sample_vec*sample_vec';    
elseif n_iter > n_cov + 1 % Recursive update of the empirical mean and covariance matrix
    obj.rw_cov = (n_iter-n_cov-1)/(n_iter-n_cov) * obj.rw_cov ...
        + (n_iter-n_cov-1)/(n_iter-n_cov)^2*(sample_vec - obj.rw_mean)*(sample_vec - obj.rw_mean)';
    obj.rw_mean = (n_iter-n_cov-1)/(n_iter-n_cov)*obj.rw_mean + 1/(n_iter-n_cov) * sample_vec;
end

