# script to execute two time a test if the first failed
# due to statisticals variation
if(NOT name_test)
   message(FATAL_ERROR "Variable name_test not defined" )
endif(NOT name_test)
if(NOT config)
   message(FATAL_ERROR "Variable config not defined" )
endif(NOT config)
#if(NOT particles)
#   message(FATAL_ERROR "Variable particles not defined" )
#endif(NOT particles)

execute_process(COMMAND ${name_test} ${config} --particles=100
                RESULT_VARIABLE test_failed OUTPUT_VARIABLE sortie1)
message(STATUS ${sortie1})

if(test_not_failed)
   execute_process(COMMAND ${name_test} ${config} --particles=100
                   RESULT_VARIABLE test_failed_two ${sortie2})
   message(STATUS ${sortie2})
endif(test_not_failed)
