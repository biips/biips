function varnames = biips_variable_names(model)
% BIIPS_VARIABLE_NAMES Variable names of the Biips model.
%  varnames = biips_variable_names(model)
%
%   INPUT:
%   - model:   Biips model as returned by the BIIPS_MODEL function
% 
%   OUTPUT:
%   - varnames: cell of strings. the list of variable names of the model
%
%   See also BIIPS_MODEL, BIIPS_NODES, BIIPS_PRINT_DOT
%--------------------------------------------------------------------------
% EXAMPLES:
% modelfile = 'hmm.bug';
% type(modelfile);
% 
% data = struct('tmax', 10, 'logtau', log(10));
% model = biips_model(modelfile, data, 'sample_data', true);
% model.model
% model.data
% biips_variable_names(model)
% biips_nodes(model)
% biips_print_dot(model, 'hmm.dot');
% biips_clear(model)
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

check_struct(model, 'biips');

varnames = matbiips('get_variable_names', model.id);
