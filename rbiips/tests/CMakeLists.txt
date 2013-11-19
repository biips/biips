file(GLOB Rfiles *.R)
file(GLOB bfiles *.bug)
set(test_files ${Rfiles} ${bfiles})
file(COPY ${test_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
set(rtests funcmat)
foreach(_test ${rtests})
    set(header  "dyn.load(\"rbiips_shlib.so\");"
                "source(\"RBiips.R\");"
                "source(\"biips.object.R\");"
                "source(\"particles.R\");"
                "source(\"pmmh.R\");")
    add_test(NAME ${_test}-R COMMAND ${R_EXECUTABLE} --vanilla < "${header};
    source(\"${rtests}\")")
    #set_tests_properties(${_test}-R PROPERTIES TIMEOUT 30)
endforeach()
