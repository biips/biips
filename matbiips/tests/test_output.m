clear
close all

bool_et_un = matbiips('load_module', 'basemod'); 
p0=matbiips('make_console'); 
matbiips('verbosity',2);
matbiips('message', 'HHHAHAHHHA');
pb1=matbiips('make_progress_bar', 100, '*', 'zut');
for i=1:100
   matbiips('advance_progress_bar', pb1, 1);
end
matbiips('clear_progress_bar', pb1);
matbiips('clear_console',p0); 
