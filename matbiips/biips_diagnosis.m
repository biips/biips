function diagn = biips_diagnosis(samples, varargin)
% BIIPS_DIAGNOSIS Diagnosis of the SMC algorithm.
% diagn = biips_diagnosis(samples, 'Propertyname', propertyvalue, ...)
%
%   INPUT
%   - samples: structure containing either the output of a SMC algorithm
%              as returned by BIIPS_SMC_SAMPLES or the output of a MCMC algorithm
%              as returned by BIIPS_PIMH_SAMPLES or BIIPS_PMMH_SAMPLES
%   Optional Inputs:
%   - type:             string containing the characters 'f' (fitering), 
%                       's' (smoothing) and/or 'b' (backward smoothing).
%                       Select the corresponding fields of the input to be diagnosed.
%                       (default = 'fsb').
%   - ess_thres :       integer. Threshold on the Effective Sample Size (ESS).
%                       If all the ESS components are over the
%                       threshold, the diagnostic is 'GOOD', otherwise it is
%                       'BAD'. (default = 30).
%   - quiet:            logical. Disable message display. (default = false)
%
%   OUTPUT
%   - diagn:   structure with the same nested fields as the input
%                 'samples' structure. Contains the minimum ESS value.
%
%   See also BIIPS_SMC_SAMPLES

%--------------------------------------------------------------------------
% EXAMPLE:
% modelfile = 'hmm.bug';
% type(modelfile);
% 
% data = struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1), 'logtau', log(1));
% model = biips_model(modelfile, data, 'sample_data', true);
% 
% n_part = 100;
% 
% [out_smc, lml] = biips_smc_samples(model, {'x', 'c[2:10]'}, n_part, 'type', 'fs', 'rs_thres', .5, 'rs_type', 'stratified');
% 
% biips_diagnosis(out_smc);
% 
% biips_diagnosis(out_smc.x);
% 
% out_smc_c = getfield(out_smc, 'c[2:10]')
% biips_diagnosis(out_smc_c);
% 
% biips_diagnosis(out_smc.x.f);
% biips_diagnosis(out_smc.x.s);
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 21-10-2014
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


if ~isstruct(samples)
    error('samples must be a struct')
end

if is_smc_array(samples) 
    %% S contains only one variable 
    ess_min = min(samples.ess(:));
    valid = (ess_min > ess_thres);
    
    diagn.ess_min = ess_min;
    diagn.valid = valid;
    
    if ~quiet
        varname = deparse_varname(samples.name, samples.lower, samples.upper);
        disp(['* Diagnosis of variable: ' , varname]);
    end
    
    if ~quiet
        switch (samples.type)
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
elseif has_fsb_fields(samples) 
    %% S contains only one variable with f,s,b fields
    names = fieldnames(samples);
    s = getfield(samples, names{1});
    
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
        
        s = getfield(samples, fsb);
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
    varnames = fieldnames(samples);
    diagn = cell(size(varnames));
    
    for i=1:numel(varnames)
        s = getfield(samples, varnames{i});
        diagn{i} = biips_diagnosis(s, varargin{:});
    end
    
    diagn = cell2struct_weaknames(diagn, varnames);
end


