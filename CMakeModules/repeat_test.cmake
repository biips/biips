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
if(${sortie1} MATCHES ".*failed.*")
   message(WARNING "premier essai foire")
   execute_process(COMMAND ${name_test} ${config} --particles=100
       RESULT_VARIABLE test_failed_two OUTPUT_VARIABLE sortie2)
   if(${sortie2} MATCHES ".*failed.*")
       message(SEND_ERROR "Ayaye planté 2 fois")
   endif() 
endif()
