function rw = pmmh_rw_learn_cov(rw, sample_param, accepted)

if ~rw.learn
    return;
end

niter = rw.niter;

% Concatenate all variables in a column vector
sample_vec = cell2mat(cellfun(@(x) x(:)' , sample_param,...
    'UniformOutput', false))';
if niter==1
    rw.mean = sample_vec;
    rw.cov = sample_vec*sample_vec';
else
    rw.cov = (niter-1)/niter * rw.cov ...
        + (niter-1)/niter^2*(sample_vec - rw.mean)*(sample_vec - rw.mean)';
    rw.mean = (niter-1)/niter*rw.mean + 1/niter * sample_vec;
end


% if (accepted ||  isempty(rw.buffer))
%     % Concatenate all variables in a vector
%     % Concatenate all variables in a row vector
%     sample_vec = cell2mat(cellfun(@(x) x(:)' , sample_param,...
%         'UniformOutput', false));
%     % Push sample back in buffer
%     rw.buffer = 

