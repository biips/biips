% Matbiips: Matlab interface to Biips
%
% Files
%   biips_add_function       - adds a Matlab function to the Biips workspace 
%   biips_build_sampler      - assigns a sampler to every node of the graph
%   biips_clear              - clears one or several biips consoles
%   biips_density            - computes 1D kernel density estimates
%   biips_diagnostic         - returns a diagnostic structure on the SMC algorithm
%   biips_get_data           - returns the data of the current model
%   biips_get_nodes          - returns the nodes of the current model
%   biips_get_variable_names - returns the variable names of the current model
%   biips_model              - instantiates a stochastic model under a DAG form 
%   biips_pimh_samples       - performs iterations for the PIMH algorithm and returns samples
%   biips_pimh_update        - performs burn-in iterations for the PIMH algorithm
%   biips_pmmh               - performs iterations for the PMMH algorithm
%   biips_pmmh_object        - creates a PMMH object
%   biips_pmmh_samples       - performs iterations for the PMMH algorithm and returns samples
%   biips_pmmh_update        - performs adaptation and burn-in iterations for the PMMH algorithm
%   biips_print_dot          - prints the graph in a file in dot format
%   biips_smc_samples        - runs a sequential Monte Carlo algorithm
%   biips_smc_sensitivity    - performs sensitivity analysis of parameters with SMC
%   biips_summary            - computes some statistics on selected variables
%   isoctave                 - checks if octave is running
