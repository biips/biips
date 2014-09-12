function diagn = biips_diagnosis(S, varargin)

%
% BIIPS_DIAGNOSIS diagnosis of the SMC algorithm
% diagn = biips_diagnosis(parts, 'Propertyname', propertyvalue, ...)
%
%   INPUT
%   - S:    input structure containing the particles of one or several variables.
%           usually returned by biips_smc_samples function
%   Optional Inputs:
%   - type:             string containing the characters 'f', 's' and/or 'b'
%   - ess_thres :       integer. Threshold on the Effective Sample Size (ESS) of the
%                       examined particles. If all the ESS components are over the
%                       threshold, the diagnostic is valid, otherwise it is not
%                       valid. default is 30
%   - quiet:            flag. deactivate message display. default is 0
%
%   OUTPUT
%   - diagn:   output structure providing the minimum value of the
%              effective sample size for each variable
%
%   See also BIIPS_SMC_SAMPLES
%--------------------------------------------------------------------------
% EXAMPLE:
% data = struct('var1', 0, 'var2', 1.2);
% model_id = biips_model('model.bug', data)
% npart = 100; variables = {'x'};
% out_smc = biips_smc_samples(model_id, variables, npart);
% diagn = biips_diagnosis(out_smc);
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------
%
%% PROCESS AND CHECK INPUTS
%%% Process and check optional arguments
optarg_names = {'type', 'ess_thres', 'quiet'};
optarg_default = {'fsb', 30, false};
optarg_valid = {{'f', 's', 'b', 'fs', 'fb', 'sb', 'fsb'}, [0, intmax],...
    {true, false}};
optarg_type = {'char', 'numeric', 'logical'};
[type, ess_thres, quiet] = parsevar(varargin, optarg_names, optarg_type,...
    optarg_valid, optarg_default);


if ~isstruct(S)
    error('S must be a struct')
end

if is_smc_array(S) 
    %% S contains only one variable 
    ess_min = min(S.ess(:));
    valid = (ess_min > ess_thres);
    
    diagn.ess_min = ess_min;
    diagn.valid = valid;
    
    if ~quiet
        varname = deparse_varname(S.name, S.lower, S.upper);
        disp(['* Diagnosis of variable: ' , varname]);
    end
    
    if ~quiet
        switch (S.type)
            case 'filtering'
                name = '  Filtering: ';
            case 'smoothing'
                name = '  Smoothing: ';
            case 'backward_smoothing'
                name = '  Backward smoothing: ';
        end
        if valid
            disp([name, 'GOOD']);
        else
            disp([name, 'POOR'])
            disp(['    The minimum effective sample size is too low: ', num2str(ess_min)])
            disp('    Estimates may be poor for some variables.')
            disp('    You should increase the number of particles.')
        end
    end
elseif has_fsb_fields(S) 
    %% S contains only one variable with f,s,b fields
    names = fieldnames(S);
    s = getfield(S, names{1});
    
    if ~quiet
        varname = deparse_varname(s.name, s.lower, s.upper);
        disp(['* Diagnosis of variable: ' , varname]);
    end
    
    diagn = struct();
    
    for i=1:numel(names)
        fsb = names{i};
        if isempty(strfind(type, fsb))
            continue
        end
        
        s = getfield(S, fsb);
        ess_min = min(s.ess(:));
        valid = (ess_min > ess_thres);
        
        diagn_s.ess_min = ess_min;
        diagn_s.valid = valid;
        
        diagn = setfield(diagn, fsb, diagn_s);
        
        if ~quiet
            switch (fsb)
                case 'f'
                    name = '  Filtering: ';
                case 's'
                    name = '  Smoothing: ';
                case 'b'
                    name = '  Backward smoothing: ';
            end
            if valid
                disp([name, 'GOOD']);
            else
                disp([name, 'POOR'])
                disp(['    The minimum effective sample size is too low: ', num2str(ess_min)])
                disp('    Estimates may be poor for some variables.')
                disp('    You should increase the number of particles.')
            end
        end
        
    end
else
    %% S contains several variables
    varnames = fieldnames(S);
    diagn = cell(size(varnames));
    
    for i=1:numel(varnames)
        s = getfield(S, varnames{i});
        diagn{i} = biips_diagnosis(s, varargin{:});
    end
    
    diagn = cell2struct_weaknames(diagn, varnames);
end


