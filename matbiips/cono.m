x=load('x.dat');
y=load('y.dat');
data=struct('xtrue', x, 'y', y, 'tmax', 100, 'precxinit', 1,'precx', 100, 'precy', 1, 'meanxinit', 0);
p0=inter_biips('make_console'); 
inter_biips('check_model', p0, 'hmm_1d_lin.bug'); 
inter_biips('compile_model', p0, data, false, 12);
inter_biips('clear_console',p0); 
