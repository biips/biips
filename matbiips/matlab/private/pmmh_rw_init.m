function rw = pmmh_rw_init(sample_param)

sampledim = cellfun(@size,sample_param, 'UniformOutput', false);

% FC: I Skipped some part of the R code - need to check if ok
% TODO: ADD something to check dimension?

rw.dim = sampledim;
rw.niter = 0;
rw.pmean = 0;
rw.povertarget = false;
rw.ncrosstarget = 10;
rw.alpha = .99;
rw.beta = .05;
rw.n_rescale = 100;

rw.d = sum(cellfun(@prod,sampledim, 'UniformOutput', true));
if rw.d==1
    rw.targetprob  = 0.44;
else
    rw.targetprob = 0.234;
end
rw.lstep = log(.1/sqrt(rw.d));

% Covariance matrix
% rw.count = 0;
% rw.buffer = [];
% rw.buff_count = [];
rw.mean = [];
rw.cov = [];