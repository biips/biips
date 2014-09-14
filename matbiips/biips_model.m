function model = biips_model(file, data, varargin)

%
% BIIPS_MODEL instantiates a stochastic model under a DAG form 
% model = biips_model(file, data, 'Propertyname', propertyvalue, ...)
%
%   INPUT: 
%   - file:         string. path of the BUGS file which defines the stochastic model
%   - data:         either a struct containing constants and observed values
%                   or a cell of strings containing names of variables
%                   existing in the 'base' workspace
%   Optional inputs:
%   - sample_data:  boolean to toggle the evaluation of the 'data' block of the
%                   BUGS model that generates data. default is 'true'
%   - quiet:        boolean to deactivate verbosity. default is 'false'
%
%   OUTPUT:
%   - model: A biips model structure with the following fields:
%                   * id: integer. index of the compiled model object
%                         in the internal table of models.
%                   * file: string. filename of the BUGS model
%                   * model: string containing the BUGS model definition
%                   * data: structure containing the data.
%
%   See also BIIPS_NODES, BIIPS_VARIABLE_NAMES, BIIPS_PRINT_DOT
%--------------------------------------------------------------------------
% EXAMPLES:
% data = struct('var1', 0, 'var2', 1.2);
% model = biips_model('model.bug', data, 'sample_data', true);
%
% var1 = 0; var2 = 1.2;
% data_names = {'var1', 'var2'};
% model = biips_model('model.bug', data_names, 'sample_data', true);
% 
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
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
