function varargout = parsevar(opt_param, param_names, param_type, param_valid, param_default)

%--------------------------------------------------------------------------
% PARSEVAR parses the set of optional parameters and returns their values
% parsevar(opt_param, param_names, param_list)
%
%   INPUTS
%   - opt_param:    cell of length 2*n {name_var1, value_var1, namevar2, value_var2, ...}
%   - param_names:  cell of length m containing the names of allowed optional parameters
%                   First value is the default value
%   - param_type:   cell of length m containing the classes class of optional parameters
%                   'char', 'numeric', 'logical', 
%   - param_valid:  cell of length m containing the set/range of valid values
%                   for input of type 'char' or logical, set of values allowed
%                   for input of type 'numeric', range of values allowed
%   - param_default:cell of length m of default values for optional parameters
%
%   OUTPUTS
%   - varargout:    Values of the optional parameters
%--------------------------------------------------------------------------
% EXAMPLE
% param_names = {'var1', 'var2', 'var3'};
% param_type = {'numeric', 'logical', 'char'};
% param_valid = {[0,10],{true, false}, {'a', 'b', 'c'}};
% param_default = {1, true, 'b'};
% params = parsevar({'var1', 5, 'var3', 'c'}, param_names, param_type,...
%                param_valid, param_default);
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------


n_params = numel(param_names);

if (nargout ~=n_params)
    error('The number of output arguments should be the same as the number of optional parameters')
end

% set default values
varargout = cell(n_params,1);
for i=1:n_params
    varargout{i} = param_default{i};
end

found = false(n_params, 1);
for i=1:2:numel(opt_param)
    [tag, ind] = ismember(opt_param{i}, param_names);
  if ~ischar(opt_param{i}) || ~tag % Check if known optional parameter name
      warning(['Unknown optional parameter ' opt_param{i}]);      
  elseif ~iscell(opt_param{i+1}) && ~isa(opt_param{i+1}, param_type{ind}) % Check of valid parameter type
      error('Input parameter ''%s'' should be of type ''%s''', opt_param{i}, param_type{ind});
  elseif iscell(opt_param{i+1}) && any(cellfun(@(x) ~isa(x, param_type{ind}), opt_param{i+1}) ) % If cell, check if all elements are of valid type
      error('Every entry of the parameter ''%s'' in the cell should be of type ''%s''', opt_param{i}, param_type{ind});
  elseif found(ind)==true
      warning(['Value of the optional parameter ' opt_param{i} ' already specified'])
  else
      if isempty(param_valid{ind}) % If no list of valid parameters
          varargout{ind} = opt_param{i+1};
          found(ind) = true;
      else    % Otherwise check if the proposed value is in the set/range of valid values      
          switch(param_type{ind})
              case 'char'
                  if ~ismember(opt_param{i+1}, param_valid{ind})
                      error('Invalid parameter value ''%s'' for input ''%s''',...
                          opt_param{i+1}, opt_param{i});
                  else
                      varargout{ind} = opt_param{i+1};
                      found(ind) = true;
                  end
              case {'numeric', 'integer'}
                  if min(opt_param{i+1})<param_valid{ind}(1) || max(opt_param{i+1})>param_valid{ind}(2)
                      error('Invalid parameter value ''%.1f'' for input ''%s''',...
                          opt_param{i+1}, opt_param{i});
                  else
                      varargout{ind} = opt_param{i+1};
                      found(ind) = true;
                  end 
              case {'logical'}
                  varargout{ind} = opt_param{i+1};
                  found(ind) = true;
              otherwise 
                  error('Unknow parameter type %s for parameter %s',...
                      param_type{ind}, param_names{ind});
          end
      end      
  end 
end
