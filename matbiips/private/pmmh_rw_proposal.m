function [prop_tr] = pmmh_rw_proposal(obj)

%--------------------------------------------------------------------------
% PMMH_RW_PROPOSAL samples from the proposal distribution
% [prop_tr] = pmmh_rw_proposal(obj)
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

sample_param_tr = obj.sample_param_tr;
n_param = numel(sample_param_tr);

% Concatenate all variables in a row vector
sample_vec_tr = cell2mat(cellfun(@(x) x(:) , sample_param_tr(:),...
    'UniformOutput', false));

% Check dimension
if numel(sample_vec_tr)~=obj.len
    error('Problem with dimensions')
end

if (isempty(obj.rw_cov) || (obj.n_iter < obj.n_rescale) || (rand<(obj.beta)))
    % Proposal with diagonal covariance
    prop_vec_tr = sample_vec_tr + exp(obj.log_step) .* randn(size(sample_vec_tr));
else
    % proposal with learnt covariance
    epsilon = 1e-5; % For numerical stability
    [cov_chol, tag] = chol(obj.rw_cov + epsilon*eye(obj.len), 'lower');
    if tag>1
        error('Covariance matrix is not positive semi-definite');
    end
    prop_vec_tr = sample_vec_tr + 2.38/sqrt(obj.len) * cov_chol * randn(obj.len, 1);
end

% Rearrange vectorized parameter to cell of arrays with appropriate
% dimensions
prop_tr = cell(1, n_param);
k=1;
for i=1:numel(sample_param_tr)
    len = prod(obj.dim{i});
    prop_tr{i} = reshape(prop_vec_tr(k:k+len-1), obj.dim{i});
    k = k + len;
end
