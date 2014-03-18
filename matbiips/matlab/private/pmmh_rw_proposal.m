function [rw, prop] = pmmh_rw_proposal(rw, sample_param)

%--------------------------------------------------------------------------
% PMMH_RW_PROPOSAL samples from the proposal distribution
% [rw, prop] = pmmh_rw_proposal(rw, sample_param)
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, François Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

% Concatenate all variables in a row vector
sample_vec = cell2mat(cellfun(@(x) x(:) , sample_param(:),...
    'UniformOutput', false));

% Check dimension
if length(sample_vec)~=rw.d
    error('Problem with dimensions')
end

rw.niter = rw.niter + 1;
if (isempty(rw.cov) || (rw.niter < rw.n_rescale) || (rand<(rw.beta)))
    % Proposal with diagonal covariance
    prop_vec = sample_vec + exp(rw.lstep) .* randn(size(sample_vec));
else
    % proposal with learnt covariance
    epsilon = 1e-5; % For numerical stability
    [cov_chol, tag] = chol(rw.cov + epsilon*eye(rw.d));
    if tag>1
        error('Matrix is not positive semi-definite');
    end
    prop_vec = sample_vec + 2.38/sqrt(rw.d) * cov_chol * randn(rw.d, 1);
end

% Rearrange vectorized parameter to cell of arrays with appropriate
% dimensions
prop = cell(1, length(sample_param));
k=1;
for i=1:length(sample_param)
    length_i = prod(rw.dim{i});
    prop{i} = reshape(prop_vec(k:k+length_i-1), rw.dim{i});
    k = k + length_i;
end