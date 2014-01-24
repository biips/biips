function []= biips_add_function(name, nb_param, fun_dim, fun_eval, varargin)
% BIIPS_ADD_FUNCTION add a matlab function to the Biips workspace 
% biips_add_function(name, nb_param, fun_dim, fun_eval , [fun_check_param, fun_is_discrete])
%  INPUT :
%  - name : string. name of the function that will be used in the bug file.
%           must be a valid BUGS language function name
%  - nb_param : integer. number of arguments of the function
%  - fun_dim : string. name of the Matlab function returning the size 
%              vector of the output. The model compiler will request it to build
%              the graph and compute the dimension of the node
%  - fun_eval : string. name of the Matlab function which realizes the evaluation
%  - fun_check_param : string. name of the Matlab function which checks if
%                      the argument values are valid.
%                      This function must return a boolean.
%                      The default implementation returns always true
%  - fun_is_discrete : string. name of the Matlab function indicating if the
%                      output is discrete or not, depending on booleans
%                      indicating if the arguments are discrete.
%                      This function must return a boolean.
%                      The default implementation returns false
% 
%  All the given Matlab functions must have the same number of input
%  arguments, and they must return a single output.

if (~isa(name,'char'))
    error('biips_add_function : 1-st argument must be a string')
end

if (~isa(fun_dim,'char'))
    error('biips_add_function : 3-d argument must be a string')
end

if (~isa(fun_eval,'char'))
    error('biips_add_function : 4-th argument must be a string')
end

if (~any(exist(fun_dim)==[2,3,5]))
    error('the function ''%s'' does not exist',fun_dim);
end
if (~any(exist(fun_eval)==[2,3,5]))
    error('the function ''%s'' does not exist',fun_eval);
end

% check for optional options
opt_argin = length(varargin);
% defauts values
fun_check_param = '';
fun_is_discrete = '';
if opt_argin >= 1
    fun_check_param = varargin{1};
    if (~isa(fun_check_param,'char'))
        error('biips_add_function : 5-th argument must be a string')
    end
    if (~any(exist(fun_check_param)==[2,3,5]))
        error('the function ''%s'' does not exist',fun_check_param);
    end
end
if opt_argin >=2
    fun_is_discrete = varargin{2};
    if (~isa(fun_is_discrete,'char'))
        error('biips_add_function : 6-th argument must be a string')
    end
    if (~any(exist(fun_is_discrete)==[2,3,5]))
        error('the function ''%s'' does not exist',fun_is_discrete);
    end
end
inter_biips('add_function', name, nb_param, fun_dim, fun_eval, fun_check_param, fun_is_discrete);
