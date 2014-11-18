function biips_add_function(name, n_param, fun_dim, fun_eval, varargin)
% BIIPS_ADD_FUNCTION Add a custom function to the BUGS language.
%   biips_add_function(name, n_param, fun_dim, fun_eval , ...
%                       'PropertyName', PropertyValue, ...)
%   INPUT:
%   - name:    string. Name of the custom function that will be used in the BUGS model.
%              must be a valid BUGS language function name.
%   - n_param: integer. Number of arguments of the custom function
%   - fun_dim: string. Name of the custom Matlab function returning the size 
%              vector of the output. It will be called when compiling the
%              model. Its arguments are the dimension vectors of the
%              inputs.
%   - fun_eval: string. Name of the custom Matlab function which evaluates the
%   function. Its arguments are the parameters values.
%
%   Optional inputs
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
%   See also BIIPS_ADD_DISTRIBUTION, BIIPS_MODEL

%--------------------------------------------------------------------------
% EXAMPLE:
% %% Add custom function f to BUGS language
% type('f_dim.m');
% type('f_eval.m');
% biips_add_function('f', 2, 'f_dim', 'f_eval');
% 
% %% Compile model
% modelfile = 'hmm_f.bug';
% type(modelfile);
% 
% data = struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1), 'logtau', log(1));
% model = biips_model(modelfile, data);
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
    error('biips_add_function: 1-st argument�must be a string')
end
if (~isa(fun_dim,'char'))
    error('biips_add_function: 3-d argument�must be a string')
end
if (~isa(fun_eval,'char'))
    error('biips_add_function: 4-th argument�must be a string')
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
matbiips('add_function', name, n_param, fun_dim, fun_eval, fun_check_param, fun_is_discrete);
fprintf('* Added function ''%s''\n', name)
