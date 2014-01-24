function biips_build_sampler(console, varargin)
% BIIPS_BUILD_SAMPLER assign a sampler to every node of the graph
% biips_build_sampler(console, [proposal])
% INPUT : 
% - console : integer. Id of the console containing the model, returned by the 'biips_model' function
% - proposal : string. Keyword defining the type of proposal desired.
%              Possible values are 'auto' and 'prior. 'auto' selects the best sampler
%              among available ones automatically. 'prior' forces asignment of the prior
%              sampler to every node. 'prior' switches off lots of instructions and can
%              speed up the startup of the SMC for big models.
%              default is 'auto'

opt_argin = length(varargin);

proposal = 'auto';
if opt_argin >= 1
   proposal = varargin{1};
end

if ~isa(proposal, 'char')
    error('2-d argument in ''biips_build_sampler'' must be a string');
end

prior=false;
if (strcmp(proposal, 'prior'))
    prior = true;
elseif (~strcmp(proposal, 'auto'))
    error('2-d argument allowed values are: ''auto'' and ''prior''')
end

inter_biips('build_smc_sampler', console, prior);
