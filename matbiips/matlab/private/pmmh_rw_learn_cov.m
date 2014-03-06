function rw = pmmh_rw_learn_cov(rw, sample_param)
% 
% if ~rw.learn
%     return;
% end

niter = rw.niter;

% Concatenate all variables in a column vector
sample_vec = cell2mat(cellfun(@(x) x(:)' , sample_param,...
    'UniformOutput', false));
sample_vec = sample_vec(:);

if niter==1
    rw.mean = sample_vec;
    rw.cov = sample_vec*sample_vec';    
else % Recursive update of the empirical mean and covariance matrix
    rw.cov = (niter-1)/niter * rw.cov ...
        + (niter-1)/niter^2*(sample_vec - rw.mean)*(sample_vec - rw.mean)';
    rw.mean = (niter-1)/niter*rw.mean + 1/niter * sample_vec;
end

