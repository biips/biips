function model = biips_model(file, data, varargin)
% BIIPS_MODEL Create a Biips model described in BUGS language.
% model = biips_model(file, data, 'Propertyname', propertyvalue, ...)
%
%   INPUT: 
%   - file:         string. path of the BUGS file which defines the stochastic model
%   - data:         either a struct containing constants and observed values
%                   or a cell of strings containing names of variables
%                   existing in the 'base' workspace. Any numeric objects
%                   in 'data' corresponding to node arrays used in 'file' are taken
%                   to represent the values of observed nodes in the model.
%   Optional inputs:
%   - sample_data:  logical. Toggle the evaluation of the 'data' block of the
%                   BUGS model that generates or transforms data. (default = true).
%   - quiet:        logical. Deactivate verbosity. (default = false).
%
%   OUTPUT:
%   - model: A Biips model structure with the following fields:
%                   * id: integer. index of the compiled model object
%                         in the internal table of models.
%                   * file: string. Filename of the BUGS model
%                   * model: string. the BUGS model definition
%                   * data: structure. observed data of the model.
%
%   See also BIIPS_VARIABLE_NAMES, BIIPS_NODES, BIIPS_PRINT_DOT,
%   BIIPS_CLEAR, BIIPS_ADD_FUNCTION, BIIPS_ADD_DISTRIBUTION

%--------------------------------------------------------------------------
% EXAMPLE:
% modelfile = 'hmm.bug';
% type(modelfile);
% 
% data = struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1), 'logtau', log(1));
% model = biips_model(modelfile, data, 'sample_data', true);
% 
% biips_clear(model)
% 
% tmax = 10;
% p = [.5; .5];
% logtau_true = log(1);
% logtau = log(1);
% datanames = {'tmax', 'p', 'logtau_true', 'logtau'};
% model = biips_model(modelfile, datanames, 'sample_data', true);
% 
% model.data
% 
% biips_variable_names(model)
% 
% biips_nodes(model)
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


%% PROCESS AND CHECK INPUTS
%%% Check filename
if ~ischar(file)
    error('Invalid file name: must be a character array.');
elseif ~exist(file, 'file')
    error('Cannot find BUGS file ''%s''', file);
end

%%% Process and check optional arguments
optarg_names = {'sample_data', 'quiet'};
optarg_default = {true, false};
optarg_valid = {{true, false}, {true, false}};
optarg_type = {'logical', 'logical'};
[sample_data, quiet] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);

if quiet
  verb = matbiips('verbosity', 0);
  cleanupObj = onCleanup(@() matbiips('verbosity', verb));
end

%%% Process and check data argument
data = data_preprocess(data); 

%% make console and check model
id = matbiips('make_console');
matbiips('check_model', id, file)

%% discard unused data
varnames = matbiips('get_variable_names', id);
v = fieldnames(data);
unused = setdiff(v, varnames);
data = rmfield(data, unused);
if (numel(unused) > 0)
    warning('Unused variables in data: %s ', sprintf('%s ', unused{:}));
end

%% compile model
matbiips('compile_model', id, data, sample_data, get_seed(), false);

%% data after possible sampling (from 'data' block in the BUGS language model)
model_data = matbiips('get_data', id);

% Biips model structure
model.id = id;
model.file = file;
model.model = fileread(file);
model.data = model_data;
% model.bak.data = data;
% model.bak.sample_data = sample_data;

model.class = 'biips';
