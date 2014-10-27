Todo list
=================

Merging changes of matbiips to Rbiips

Legend
-------
- [code][doc] function

- [x] = conformity ok
- [v] = validity temporary

RBiips-package.R
-----------------
- [x][ ] .onLoad
- [x][ ] Rbiips
- [x][x] biips_add_function
- [x][x] biips_add_distribution

utils.R
-----------------
- [x][ ] is_legal_vname
- [x][ ] to_biips_vname
- [v][ ] parse_varname
- [v][ ] parse_varnames
- [x][ ] deparse_varname
- [x][ ] data_preprocess
- [x][ ] mklist
- [x][ ] get_seed
- [v][ ] check_type

biips_model.R
---------------
- [x][x] biips_model

biips-object.R
---------------
- [x][x] is.biips
- [v][x] print.biips
- [x][x] biips_variable_names
- [x][x] biips_nodes
- [x][x] biips_print_dot
- [x][x] biips_build_sampler
- [v][ ] monitor
- [v][ ] is_monitored
- [v][ ] clear_monitors
- [v][ ] clone_model

smc.R
---------------
- [v][ ] biips_smc_samples
- [v][ ] smc_forward_algo

smcarray-object.R
---------------
- [x][ ] is.smcarray
- [v][ ] print.smcarray
- [v][ ] biips_summary
- [v][ ] print.biips_summary
- [v][ ] mean
- [x][ ] biips_diagnosis
- [x][ ] print.biips_diagnosis
- [v][ ] get_index
- [v][ ] biips_density
- [v][ ] biips_table
- [v][ ] biips_hist

pimh.R
--------
- [v][ ] pimh_init
- [v][ ] is.pimh
- [v][ ] pimh_algo
- [v][ ] pimh_update
- [v][ ] pimh_samples

mcmcarray-object.R
-------------------
- [x][ ] is.mcmcarray
- [v][ ] summary
- [v][ ] print.summary
- [v][ ] mean
- [v][ ] density
- [v][ ] plot.density

sensitivity.R
---------------
- [v][ ] smc_sensitivity

pmmh.R
----------
- [v][ ] pmmh_set_param
- [v][ ] pmmh_init
- [v][ ] pmmh_rw_proposal
- [v][ ] pmmh_rw_rescale
- [v][ ] pmmh_rw_learn_cov
- [x][ ] is.pmmh
- [v][ ] pmmh_one_update
- [v][ ] pmmh_algo
- [v][ ] pmmh_update
- [v][ ] pmmh_samples

