% Matbiips: Matlab interface to Biips
%
% Files
%   biips_add_function     - Add a user-defined function to the BUGS language.
%   biips_add_distribution - Add a user-defined distribution to the BUGS language.
%   biips_model            - Create a Biips model described in BUGS language.
%   biips_clear            - Clear allocated Biips consoles.
%   biips_nodes            - Table of nodes of the Biips model.
%   biips_variable_names   - Variable names of the Biips model.
%   biips_print_dot        - Print the Biips model graph in a file in dot format.
%   biips_build_sampler    - Assign a sampler to each node of the graph.
%   biips_smc_samples      - Run a sequential Monte Carlo algorithm.
%   biips_pimh_init        - Create a PIMH object.
%   biips_pimh_update      - performs burn-in iterations for the PIMH algorithm.
%   biips_pimh_samples     - Perform iterations for the PIMH algorithm and returns samples.
%   biips_pmmh_init        - Create a PMMH object.
%   biips_pmmh_update      - Perform adaptation and burn-in iterations for the PMMH algorithm.
%   biips_pmmh_samples     - Perform iterations for the PMMH algorithm and returns samples.
%   biips_smc_sensitivity  - Perform sensitivity analysis of parameters with SMC.
%   biips_summary          - Univariate marginal summary statistics.
%   biips_density          - Univariate marginal kernel density estimates.
%   isoctave               - Check if octave is running.
%   biips_diagnosis        - Diagnosis of the SMC algorithm.
%   biips_table            - Univariate frequency tables or probability mass estimates of discrete variables.
