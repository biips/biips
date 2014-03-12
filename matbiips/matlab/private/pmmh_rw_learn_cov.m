function rw = pmmh_rw_learn_cov(rw, sample_param)

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

