set_output_bindir(out_bindir)

# copy files to binary directory
set(rbiips_bug hmm_4d_lin_tracking 
               stoch_volatility 
               hmm_1d_lin 
               hmm_1d_nonlin
               dyn_beta_reg
               hmm_1d_lin_param
               hmm_4d_nonlin_tracking
               stoch_volatility_param
               hmm_1d_nonlin_param
               )

foreach(bug ${rbiips_bug})
   set (bugs_files ${bugs_files} "${CMAKE_SOURCE_DIR}/examples/${bug}.bug")
endforeach()
file(COPY ${bugs_files} DESTINATION ${out_bindir})

# add dependencies to source files
add_targets_depends(bugs_files)
