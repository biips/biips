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
- [x][ ] biips_add_function
- [x][ ] biips_add_distribution

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
- [x][ ] biips_model

biips-object.R
---------------
- [x][ ] is.biips
- [v][ ] print.biips
- [x][ ] variable.names
- [x][ ] nodes
- [x][ ] print_dot
- [x][ ] build_sampler
- [v][ ] monitor
- [v][ ] is_monitored
- [v][ ] clear_monitors
- [v][ ] clone_model

smc.R
---------------
- [v][ ] smc_samples
- [v][ ] smc_forward_algo

smcarray-object.R
---------------
- [x][ ] is.smcarray
- [v][ ] print.smcarray
- [ ][ ] summary
- [ ][ ] print.summary
- [ ][ ] mean
- [x][ ] print.diagnosis
- [x][ ] diagnosis
- [ ][ ] get_index
- [ ][ ] density

pimh.R
--------
- [ ][ ] pimh_init
- [ ][ ] is.pimh
- [ ][ ] pimh_algo
- [ ][ ] pimh_update
- [ ][ ] pimh_samples

mcmcarray-object.R
-------------------
- [x][ ] is.mcmcarray
- [ ][ ] summary
- [ ][ ] print.summary
- [ ][ ] mean
- [ ][ ] density
- [ ][ ] plot.density

sensitivity.R
---------------
- [ ][ ] smc_sensitivity

pmmh.R
----------
- [ ][ ] pmmh_set_param
- [ ][ ] pmmh_init
- [ ][ ] pmmh_rw_proposal
- [ ][ ] pmmh_rw_rescale
- [ ][ ] pmmh_rw_learn_cov
- [x][ ] is.pmmh
- [ ][ ] pmmh_one_update
- [ ][ ] pmmh_algo
- [ ][ ] pmmh_update
- [ ][ ] pmmh_samples

