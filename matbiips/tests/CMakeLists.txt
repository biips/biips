set(fichiers_test cono.m hmm_1d_lin.bug x.dat y.dat) 
file(COPY ${fichiers_test} DESTINATION .)
add_test(cono "${MATLAB}" "<" cono.m)
