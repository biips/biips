function ok = biips_load_module(str)
%BIIPS_LOAD_MODULE load a Biips module
ok = inter_biips('load_module', str);
if (~ok)
    error('problem loading module'); 
end
return;
