function biips_print_dot(model, file)
% BIIPS_PRINT_DOT Print the Biips model graph in a file in dot format.
%   biips_print_dot(model, filename)
%
%   INPUT
%   - model:    Biips model as returned by the BIIPS_MODEL function
%   - file: string. Path of the output file
% 
%   See also BIIPS_MODEL, BIIPS_VARIABLE_NAMES, BIIPS_NODES

%--------------------------------------------------------------------------
% EXAMPLE:
% modelfile = 'hmm.bug';
% type(modelfile);
% 
% data = struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1), 'logtau', log(1));
% model = biips_model(modelfile, data, 'sample_data', true);
% 
% dotfile = 'hmm.dot';
% biips_print_dot(model, dotfile);
% %type(dotfile);
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 21-10-2014
%--------------------------------------------------------------------------

check_struct(model, 'biips');

matbiips('print_graphviz', model.id, file);