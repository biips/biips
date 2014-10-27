function f = f_eval( x, t )
% F_EVAL Evaluate function f
f = 0.5.*x+25.*x./(1+x.^2)+8*cos(1.2.*t);
