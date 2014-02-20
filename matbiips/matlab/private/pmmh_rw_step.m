function [rw] = pmmh_rw_step(rw, rw_step, rw_cov)

n_param = length(rw_step);

% Check values and dimensions
for i=1:n_param
    if sum(isnan(rw_step{i}(:)))>0
        error('NaN values')        
    end
     if sum(isinf(rw_step{i}(:)))>0
        error('Inf values')        
     end
    if sum((rw_step{i}(:))<=0)
        error('Non-positive values')        
    end
    % Convert to a (row) vector
    rw_step{i} = rw_step{i}(:)';
end

% Concatenate all log value in a (row) vector
rw.lstep = cell2mat(cellfun(@log, rw_step, 'UniformOutput', false));

% Clear rescaled parameters
rw.niter = 1;
rw.pmean = 0;
rw.povertarget = false;
rw.ncrosstarget = 10;

% Clear learnt covariance matrix
rw.count = 0;
rw.buffer = [];
rw.buff_count = [];
rw.mean = [];
rw.cov = rw_cov;