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
