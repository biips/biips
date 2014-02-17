function [rw] = pmmh_rw_step(rw, rw_step)

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
    % Convert to a vector
    rw_step{i} = rw_step{i}(:)';
end

% Concatenate all log value in a vector
rw.lstep = cell2mat(cellfun(@log, rw_step));

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
rw.cov = [];