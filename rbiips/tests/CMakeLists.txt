file(GLOB Rfiles *.R)
file(GLOB bfiles *.bug)
set(test_files ${Rfiles} ${bfiles})
file(COPY ${test_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
set(rtests funcmat)
foreach(_test ${rtests})
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/header.R"  "dyn.load(\"rbiips_shlib.so\");"
                "source(\"RBiips.R\");"
                "source(\"biips.object.R\");"
                "source(\"particles.R\");"
                "source(\"pmmh.R\");"
                "load.biips.module(\"basemod\");"
                )
    add_test(NAME ${_test}-R COMMAND ${R_EXECUTABLE} -e "source(\"header.R\"); source(\"${rtests}.R\")")
endforeach()
