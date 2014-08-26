function biips_add_distribution(name, n_param, fun_dim, fun_sample, varargin)

%
% BIIPS_ADD_DISTRIBUTION adds a Matlab distribution to the BUGS language
% Currently, we can only provide the sampling function but not the density
% function. Consequently the added distributions can only define 
% unobserved nodes.
%   biips_add_distribution(name, n_param, fun_dim, fun_sample, ...
%                       'PropertyName', PropertyValue, ...)
%   INPUT:
%   - name:     string. name of the function that will be used in the bug file.
%               must be a valid BUGS language function name
%   - n_param: integer. number of arguments of the function
%   - fun_dim:  string. name of the Matlab function returning the size 
%               vector of the output. The model compiler will request it to build
%               the graph and compute the dimension of the node
%   - fun_sample: string. name of the Matlab function which realizes the
%       sampling
%
%   Optional inputs
%   - fun_check_param: string. name of the Matlab function which checks if
%                      the argument values are valid.
%                      This function must return a boolean.
%                      The default implementation returns always true
%   - fun_is_discrete: string. name of the Matlab function indicating if the
%                      output is discrete or not, depending on booleans
%                      indicating if the arguments are discrete.
%                      This function must return a boolean.
%                      The default implementation returns false
% 
%  All the given Matlab functions must have the same number of input
%  arguments, and they must return a single output.
%--------------------------------------------------------------------------
% EXAMPLE:
% dist_bugs = 'distmat'; fun_dim = 'f_dim'; fun_sample = 'f_sample';
% dist_nb_inputs = 2;
% biips_add_distribution(dist_bugs, dist_nb_inputs, fun_dim, fun_sample);
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 25-03-2014
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
