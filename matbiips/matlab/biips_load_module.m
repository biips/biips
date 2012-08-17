function ok = biips_load_module()
%BIIPS_LOAD_MODULE load the module used by BIIPS
ok = inter_biips('load_module', 'basemod');
if (~ok)
    error('problem loading module'); 
end
return;
