function [] = biips_print_dot(p, filename)
% BIIPS_PRINT_DOT returns the data of the current model
%  [] = biips_print_dot(p, filename)
% INPUT
%  -p : number of the current console
%  -filename : string 
inter_biips('print_graphviz', p, filename);
