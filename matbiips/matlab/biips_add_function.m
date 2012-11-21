function []= biips_add_function(name, nb_param, fun_dim, fun_eval , varargin)
% BIIPS_ADD_FUNCTION add a matlab function to the Biips workspace 
% function ret = biips_add_function(name, nb_param, fun_dim, fun_eval , fun_check_param, fun_is_discrete)
%  INPUT : 
%  - name : name (string) of the function in the bug file
%  - nb_param : number of arguments of the function
%  - fun_dim : functor returning a vector of dims of each argument
%  - fun_eval : functor which realize the evaluation of the function
%  - fun_check_param : functor which checks if parameters are valid
%  - fun_is_discrete : functor indicating if the output is discrete or no 
if (~isa(fun_dim,'char'))
   error('add_biips_function : 3-d argument must be a string')
end

if (~isa(fun_eval,'char'))
   error('add_biips_function : 4-th argument must be a string')
end

if (exist(fun_dim)~=2) && (exist(fun_dim)~=3)
   error('the function %s does not exist',fun_dim);
end   
if (exist(fun_eval)~=2) && (exist(fun_eval)~=3)
   error('the function %s does not exist',fun_eval);
end   

% check for optional options
opt_argin = length(varargin);
% defauts values
fun_check_param = '';
fun_is_discrete = '';
if opt_argin >= 1
   fun_check_param = varargin{1};
end
if opt_argin >=2 
   fun_is_discrete = varargin{2};
end   
inter_biips('add_function', name, nb_param, fun_dim, fun_eval, fun_check_param, fun_is_discrete); 
