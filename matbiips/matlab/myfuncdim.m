function [ret]=myfuncdim(x,v)
if (size(x)~=[4,1])
   fprintf('%d',size(x));
   error('myfuncdim x must have 4 rows')
end
ret=[4,1];
