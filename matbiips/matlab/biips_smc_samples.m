function [particles, log_marg_like] = biips_smc_samples(console, variable_names, n_part, varargin)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% BIIPS_SMC_SAMPLES main routine implementing SMC algorithms
% [particles, log_marg_like] = biips_smc_samples(console, variable_names, 
%        nb_part, 'Propertyname', propertyvalue, ...)
% INPUT: 
% - console :           integer. Id of the console containing the model, 
%                       returned by the 'biips_model' function
% - variable_names :    cell of strings. Contains the names of the 
%                       unobserved variables to monitor.
%                       Possible value: {'var1', 'var2[1]', 'var3[1:10]',
%                                                       'var4[1, 5:10, 3]'}
%                       Dimensions and indices must be a valid subset of 
%                       the variables of the model.
% - n_part :            positive integer. Number of particles used in SMC algorithms
%
% Optional Inputs:
% - type :      string (default = 'fs').
%               Its characters must be in the set {'f', 's', 'b'} for 
%               respectively 'filtering', 'smoothing' and 'backward smoothing'.
%               can use multiple letters, eg: 'fsb' for the three algorithms.
% - rs_thres :  positive real (default = 0.5).
%               Threshold for the resampling step (adaptive SMC).
%               if rs_thres is in [0,1] --> resampling occurs when 
%                                           (ESS > rs_thres * nb_part)
%               if rs_thres is in [2,nb_part] --> resampling occurs when 
%                                               (ESS > rs_thres)
% - rs_type :   string (default = 'stratified')
%               Possible values are 'stratified', 'systematic', 'residual', 'multinomial'
%               Indicates the type of algorithm used for the resampling step.           
% - seed :      integer (default is set randomly). 
%               Seed for random number generator.
% OUTPUT:
% particles:    output structure containing all the SMC information
% log_marg_like: log marginal likelihood
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% EXAMPLE:
% data = struct('var1', 0, 'var2', 1.2);
% model_id = biips_model('model.bug', data)
% npart = 100; variables = {'x'}; 
% type = 'fs'; rs_type = 'multinomial'; rs_thres = 0.5;
% out_smc = biips_smc_samples(model_id, variables, npart, 'type', type,...
%               'rs_type', rs_type, 'rs_thres', rs_thres);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% BiiPS Project - Bayesian Inference with interacting Particle Systems
%
% Reference: A. Todeschini, M. Fuentes, F. Caron, P. Legrand, P. Del Moral.
% BiiPS: a software for Bayesian inference with interacting particle
% systems. Technical Report, INRIA. February 2014.
%
% Authors: Adrien Todeschini, Marc Fuentes
% INRIA Bordeaux, France
% email: biips-project@lists.gforge.inria.fr
% Website: https://alea.bordeaux.inria.fr/biips
% Jan 2014; Last revision: 31-01-2014
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%% Default values
type='fs';
rs_type = 'stratified';
rs_thres = 0.5;
seed = get_seed();
parsevar; % Process options

indices = arrayfun(@(x) strfind(type,x), 'fsb', 'UniformOutput', 0);
filtering = ~isempty(indices{1}); 
smoothing = ~isempty(indices{2}); 
backward = ~isempty(indices{3});

%% Monitor
if (backward)
    inter_biips('set_default_monitors', console);
end
if (~isempty(variable_names))
    monitor_biips(console, variable_names, type); 
end

%% Run smc_sample
ok = run_smc_forward(console, n_part, rs_thres, rs_type, seed);

log_marg_like = inter_biips('get_log_norm_const', console);

cz=cell(0,1);
if filtering % Get filtering output
    mon1 = inter_biips('get_filter_monitors', console);
    noms = fieldnames(mon1);
    cz = struct2cell(mon1);
    if (~backward)
       clear_monitors(console, 'f');
    end   
end

if smoothing % Get smoothing output
    mon2 = inter_biips('get_gen_tree_smooth_monitors', console);
    noms = fieldnames(mon2);
    cz = horzcat(cz, struct2cell(mon2));
    clear_monitors(console, 's');
end

if (backward) % Get backward smoothing output
   inter_biips('run_backward_smoother', console);
   clear_monitors(console, 'f'); 
   mon3 = inter_biips('get_backward_smooth_monitors', console);
   noms = fieldnames(mon3);    
   clear_monitors(console, 'b'); 
   cz = horzcat(cz, struct2cell(mon3));
end

% fsb = {'f', 's', 'b' };
% fsb = {fsb{1:size(cz, 2)} };
for i=1:length(type)
    fsb{i} = type(i);
end

nb_noms = length(noms);
cell_noms = cell(nb_noms, 1);
for i=1:nb_noms
   cell_noms{i} = cell2struct({cz{i, :}}, fsb, 2);
end
particles = cell2struct_weaknames(cell_noms, noms); % Allows to add fields with brackets in the structure
