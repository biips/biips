function [obj, prop] = pmmh_rw_proposal(obj)

%--------------------------------------------------------------------------
% PMMH_RW_PROPOSAL samples from the proposal distribution
% [obj, prop] = pmmh_rw_proposal(obj)
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

sample_param = obj.sample_param;
n_param = length(sample_param);

% Concatenate all variables in a row vector
sample_vec = cell2mat(cellfun(@(x) x(:) , sample_param(:),...
    'UniformOutput', false));

% Check dimension
if length(sample_vec)~=obj.len
    error('Problem with dimensions')
end

obj.n_iter = obj.n_iter + 1;

if (isempty(obj.rw_cov) || (obj.n_iter < obj.n_rescale) || (rand<(obj.beta)))
    % Proposal with diagonal covariance
    prop_vec = sample_vec + exp(obj.log_step) .* randn(size(sample_vec));
else
    % proposal with learnt covariance
    epsilon = 1e-5; % For numerical stability
    [cov_chol, tag] = chol(obj.rw_cov + epsilon*eye(obj.len), 'lower');
    if tag>1
        error('Covariance matrix is not positive semi-definite');
    end
    prop_vec = sample_vec + 2.38/sqrt(obj.len) * cov_chol * randn(obj.len, 1);
end

% Rearrange vectorized parameter to cell of arrays with appropriate
% dimensions
prop = cell(1, n_param);
k=1;
for i=1:length(sample_param)
    len = prod(obj.dim{i});
    prop{i} = reshape(prop_vec(k:k+len-1), obj.dim{i});
    k = k + len;
end
