addpath('../matlab');
bool_et_un = inter_biips('load_module', 'basemod'); 
p0=inter_biips('make_console'); 
inter_biips('verbosity',2);
inter_biips('message', 'HHHAHAHHHA');
pb1=inter_biips('make_progress_bar', 100, '*', 'zut')
for i=1:100
   inter_biips('advance_progress_bar', pb1, 1);
end
inter_biips('clear_progress_bar', pb1);
inter_biips('clear_console',p0); 
