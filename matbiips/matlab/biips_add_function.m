function biips_add_function(name, nb_param, fun_dim, fun_eval, varargin)

%
% BIIPS_ADD_FUNCTION adds a Matlab function to the BUGS language
%   biips_add_function(name, nb_param, fun_dim, fun_eval , ...
%                       'PropertyName', PropertyValue, ...)
%   INPUT:
%   - name:     string. name of the function that will be used in the bug file.
%               must be a valid BUGS language function name
%   - nb_param: integer. number of arguments of the function
%   - fun_dim:  string. name of the Matlab function returning the size 
%               vector of the output. The model compiler will request it to build
%               the graph and compute the dimension of the node
%   - fun_eval: string. name of the Matlab function which realizes the evaluation
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
% fun_bugs = 'funmat'; fun_dim = 'f_dim'; funeval = 'f_eval';
% fun_nb_inputs = 2;
% biips_add_function(fun_bugs, fun_nb_inputs, fun_dim, funeval);
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 17-03-2014
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
    error('biips_add_function : 1-st argument�must be a string')
end
if (~isa(fun_dim,'char'))
    error('biips_add_function : 3-d argument�must be a string')
end
if (~isa(fun_eval,'char'))
    error('biips_add_function : 4-th argument�must be a string')
end
if (~any(exist(fun_dim)==[2,3,5]))
    error('the function ''%s'' does not exist',fun_dim);
end
if (~any(exist(fun_eval)==[2,3,5]))
    error('the function ''%s'' does not exist',fun_eval);
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

%% Add function
try
    inter_biips('add_function', name, nb_param, fun_dim, fun_eval, fun_check_param, fun_is_discrete);
    fprintf('Added function ''%s''\n', name)
catch
    warning('Cannot add function ''%s'' - the function may already exist', name);
end
