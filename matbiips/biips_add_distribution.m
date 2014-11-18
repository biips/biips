function biips_add_distribution(name, n_param, fun_dim, fun_sample, varargin)
% BIIPS_ADD_DISTRIBUTION Add a custom distribution to the BUGS language.
%   In the current version, the custom distributions can only be used for 
%   unobserved nodes.
% 
%   biips_add_distribution(name, n_param, fun_dim, fun_sample, ...
%                       'PropertyName', PropertyValue, ...)
%   INPUT:
%   - name:    string. Name of the custom function that will be used in the BUGS model.
%              must be a valid BUGS language distribution name.
%   - n_param: integer. Number of arguments of the custom distribution.
%   - fun_dim: string. Name of the custom Matlab function returning the size 
%              vector of the output. It will be called when compiling the
%              model. Its arguments are the dimension vectors of the
%              inputs.
%   - fun_sample: string. Name of the custom Matlab function which samples from
%   the distribution. Its arguments are the parameters values. 
%
%   Optional inputs:
%   - fun_check_param: string. Name of the custom Matlab function which checks if
%                      the argument values are valid. Its arguments are the parameters values. 
%                      Returns a logical. (default returns true)
%   - fun_is_discrete: string. Name of the custom Matlab function returning a logical that is true if the
%                      output is discrete. Its arguments are logicals
%                      indicating if the arguments are discrete.
%                      (default returns false)
% 
%   All the given Matlab functions must be .m files. They have the same number of input
%   arguments, and they must return a single output.
% 
%   See also BIIPS_ADD_FUNCTION, BIIPS_MODEL

%--------------------------------------------------------------------------
% EXAMPLE:
% %% Add custom sampling distribution dMN to BUGS language
% type('dMN_dim.m');
% type('dMN_sample.m');
% biips_add_distribution('dMN', 2, 'dMN_dim', 'dMN_sample');
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 21-10-2014
%--------------------------------------------------------------------------


%% PROCESS AND CHECK INPUTS
optarg_names = {'fun_check_param', 'fun_is_discrete'};
optarg_default = {'', ''};
optarg_valid = {{}, {}};
optarg_type = {'char', 'char'};
[fun_check_param, fun_is_discrete] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);
% Check inputs
if (~isa(name,'char'))
    error('biips_add_distribution: 1-st argument�must be a string')
end
if (~isa(fun_dim,'char'))
    error('biips_add_distribution: 3-d argument�must be a string')
end
if (~isa(fun_sample,'char'))
    error('biips_add_distribution: 4-th argument�must be a string')
end
if (~any(exist(fun_dim)==[2,3,5]))
    error('the function ''%s'' does not exist',fun_dim);
end
if (~any(exist(fun_sample)==[2,3,5]))
    error('the function ''%s'' does not exist',fun_sample);
end
if ~isempty(fun_check_param)
    if (~any(exist(fun_check_param)==[2,3,5]))
        error('the function ''%s'' does not exist',fun_check_param);
    end
end
if ~isempty(fun_is_discrete)
    if (~any(exist(fun_is_discrete)==[2,3,5]))
        error('the function ''%s'' does not exist',fun_is_discrete);
    end
end

%% Add distribution sampler
matbiips('add_distribution', name, n_param, fun_dim, fun_sample, fun_check_param, fun_is_discrete);
fprintf('* Added distribution ''%s''\n', name)
