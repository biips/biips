Todo list
=================

Merging changes of matbiips to Rbiips

Legend
-------
- [code][doc] function

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
- [x][ ] data_preprocess
- [ ][ ] mklist
- [ ][ ] get_seed
- [ ][ ] deparse_varname
- [ ][ ] parse_varname
- [ ][ ] parse_varnames
- [ ][ ] check_type

biips_model.R
---------------
- [ ][ ] biips_model

biips-object.R
---------------
- [x][ ] is.biips
- [ ][ ] print.biips
- [ ][ ] variable.names
- [ ][ ] nodes
- [ ][ ] print_dot
- [ ][ ] build_sampler
- [ ][ ] monitor
- [ ][ ] is_monitored
- [ ][ ] clear_monitors
- [ ][ ] clone_model

smc.R
---------------
- [ ][ ] smc_samples
- [ ][ ] smc_forward_algo

smcarray-object.R
---------------
- [x][ ] is.smcarray
- [ ][ ] print.smcarray
- [ ][ ] summary
- [ ][ ] print.summary
- [ ][ ] plot.summary
- [ ][ ] mean
- [x][ ] print.diagnosis
- [x][ ] diagnosis
- [ ][ ] get_index
- [ ][ ] density
- [ ][ ] plot.density

pimh.R
--------
- [ ][ ] pimh_init
- [x][ ] is.pimh
- [ ][ ] pimh_algo
- [ ][ ] pimh_update
- [ ][ ] pimh_samples

mcmcarray-object.R
-------------------
- [x][ ] is.mcmcarray
- [ ][ ] summary
- [ ][ ] print.summary
- [ ][ ] plot.summary
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

