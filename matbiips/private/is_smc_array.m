function [ is_smc ] = is_smc_array( S )
%IS_SMC_ARRAY check if S is a structure with fields 'values', 'weights',
%'ess', 'discrete'
%   Detailed explanation goes here

names = {'values', 'weights', 'ess', 'discrete', 'name', 'lower', 'upper', 'type'};

is_smc = isstruct(S) && all(cellfun(@(x) isfield(S, x), names));
end
