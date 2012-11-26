function [] = biips_print_dot(p, filename)
% BIIPS_PRINT_DOT prints the graph in a file in dot format
%  [] = biips_print_dot(p, filename)
% INPUT
%  -p : number of the current console
%  -filename : string 
inter_biips('print_graphviz', p, filename);
