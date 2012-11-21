function [ret]=myfunceval(x,v)
delta_t = 1;
F = [ 1 0 delta_t 0
    0 0 1 0
    delta_t 0 0 1
    0 0 0 1];
G = [ delta_t.^2/2 0
    0 delta_t.^2/2
    delta_t 0
    0 delta_t];
ret=F*x+G*v;
