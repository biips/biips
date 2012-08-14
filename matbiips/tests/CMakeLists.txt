file(GLOB mfiles *.m)
set(fichier_tests ${mfiles} hmm_1d_lin.bug) 
file(COPY ${fichier_tests} DESTINATION .)
