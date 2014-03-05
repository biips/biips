function obj = biips_pmmh_object(console, param_names, varargin)

% Create a biips pmmh object

%%% Process and check optional arguments
optarg_names = {'inits', 'rw_step', 'n_rescale', 'beta','alpha'};
optarg_default = {{}, [], 100, .05, .99};
optarg_valid = {{}, [], [0,intmax], [0,1], [0,1]};
optarg_type = {'numeric', 'numeric', 'numeric', 'numeric', 'numeric'};
[inits, rw_step, n_rescale, beta, alpha] = parsevar(varargin, optarg_names,...
    optarg_type, optarg_valid, optarg_default);

% Check param_names
for i=1:length(param_names)
    if ~ischar(param_names{i})
        error('Invalid parameter name %s',param_names{i})        
    end
end
% Remove duplicate entries
param_names = unique(param_names);
n_param = length(param_names);

% Check the init values
if ~isempty(inits)
    if length(inits)~=length(param_names)
        error('Inits must be a cell with the same length as param_names')
    end
end


%% Stops biips verbosity
inter_biips('verbosity', 0);
cleanupObj = onCleanup(@() inter_biips('verbosity', 1));% set verbosity on again when function terminates


% Console and parameter names
obj.console = console;
obj.param_names = param_names;

% Init the parameters of the random walk
pn_param = cellfun(@parse_varname, param_names);
sample_param = set_param(console, pn_param, inits);

sampledim = cellfun(@size,sample_param, 'UniformOutput', false);

obj.dim = sampledim;
obj.niter = 0;
obj.pmean = 0;
obj.alpha = alpha;
obj.beta = beta;
obj.n_rescale = n_rescale;

obj.d = sum(cellfun(@prod,sampledim, 'UniformOutput', true));
if obj.d==1
    obj.targetprob  = 0.44;
else
    obj.targetprob = 0.234;
end


% Init rw_stepsize for the part with diagonal covariance matrix
if isempty(rw_step)
    for i=1:n_param
        rw_step{i} = .1/sqrt(obj.d);
    end
else
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
end
% Concatenate all log value in a vector
obj.lstep = cell2mat(cellfun(@log, rw_step, 'UniformOutput', false))';

% Covariance matrix
obj.mean = [];
obj.cov = [];  


function sample_param = set_param(console, pn_param, inits)

sample_param = cell(length(pn_param), 1);
% Set init values in Biips
if ~isempty(inits)
    for i=1:length(pn_param)
        % Take init value in inits parameters
        tag = inter_biips('change_data', console, pn_param(i).name, ...
            pn_param(i).lower, pn_param(i).upper, inits{i}, true);        
        if ~tag
            error('Data change failed: invalid initial value for variable %s', pn_param(i).name);
        end
        sample_param{i} = inits{i};
    end
else
    for i=1:length(pn_param)
        try
            sample_param{i} = inter_biips('sample_data', console, pn_param(i).name,...
                pn_param(i).lower, pn_param(i).upper, get_seed());
        catch
            warning('CANNOT SAMPLE VARIABLE: BUG TO BE FIXED')
        end
    end
end