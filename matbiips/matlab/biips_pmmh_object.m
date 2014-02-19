function obj = biips_pmmh_object(console, param_names)

% Create a biips pmmh object

% Check param_names
for i=1:length(param_names)
    if ~ischar(param_names{i})
        error('Invalid param name')
        
    end
end
% Remove duplicate entries
param_names = unique(param_names);



obj.console = console;
obj.param_names = param_names;

% Init the parameters of the random walk
pn_param = cellfun(@parse_varname, param_names);
for i=1:length(param_names)
    sample_param{i} = inter_biips('sample_data', console, pn_param(i).name,...
        pn_param(i).lower, pn_param(i).upper, get_seed());
end

sampledim = cellfun(@size,sample_param, 'UniformOutput', false);

obj.dim = sampledim;
obj.niter = 0;
obj.pmean = 0;
obj.alpha = .99;
obj.beta = .05;
obj.n_rescale = 100;

obj.d = sum(cellfun(@prod,sampledim, 'UniformOutput', true));
if obj.d==1
    obj.targetprob  = 0.44;
else
    obj.targetprob = 0.234;
end
obj.lstep = log(.1/sqrt(obj.d));

% Covariance matrix
obj.mean = [];
obj.cov = [];            