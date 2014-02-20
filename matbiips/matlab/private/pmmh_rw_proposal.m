function [rw, prop] = pmmh_rw_proposal(rw, sample_param)

% Concatenate all variables in a row vector
sample_vec = cell2mat(cellfun(@(x) x(:)' , sample_param,...
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
    epsilon = .01;
    cov_chol = cholcov(rw.cov + epsilon*eye(rw.d));
    prop_vec = sample_vec + 2.38/sqrt(rw.d) * randn(1,rw.d)* cov_chol ;
end

% Rearrange vectorized parameter to cell of arrays with appropriate
% dimensions
prop = cell(length(sample_param), 1);
k=1;
for i=1:length(sample_param)
    length_i = prod(rw.dim{i});
    prop{i} = reshape(prop_vec(k:k+length_i-1), rw.dim{i});
    k = k + length_i;
end