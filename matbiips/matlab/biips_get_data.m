function data = biips_get_data(p)
% BIIPS_GET_DATA returns the data of the current model
%  data = biips_get_data(p)
% INPUT
%  -p : number of the current console
% OUTPUT
%  - data : cell containing the current data 
data= inter_biips('get_data', p);
