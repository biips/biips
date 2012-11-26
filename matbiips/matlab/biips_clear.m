function biips_clear(varargin)
% BIIPS_CLEAR clears memory occupied by a console
% biips_smc_samples([console, ...])
% INPUT : 
% - console : integers. Ids of the console containing the model, returned by the 'biips_model' function
% 
% biips_clear called with no argument will clear all the consoles and reset
% the Id counter to 0.

opt_argin = length(varargin);

if opt_argin
    for i =1:opt_argin
        if ~isa(varargin{i},'numeric') || length(varargin{i})~=1
            error([num2str(i), '-th argument of ''biips_clear'' must be a scalar numeric'])
        end
        inter_biips('clear_console', varargin{i});
    end
else
    inter_biips('clear_all_console');
end