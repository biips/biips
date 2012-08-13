file(GLOB mfiles *.m)
set(fichier_tests ${mfiles} hmm_1d_lin.bug x.dat y.dat) 
install(FILES ${fichier_tests} DESTINATION .)
file(COPY ${fichier_tests} DESTINATION .)
