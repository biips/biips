function [out]=mydistsample(x, t, prec)

mu = 0.5*x+25*x./(1+x.^2)+8*cos(1.2.*(t-1));

out = mu+1./sqrt(prec)*randn(1);
