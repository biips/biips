function [f, x, bw]=kde(obs, weights, varargin)
% KDE kernel gaussian density estimation
% [f, x, bw] = kde(obs, weights, [adjust , bw])
% INPUT
% -obs : observation 1D data row vector
% -weigths : weights row vector
% -adjust : time factor for the bw
% -bw : bandwidth
% OUTPUT
% - f : values of the density 
% - x : points of the density (100 points in the range [min, max]+4*bw
% - bw : bandwidth , useful only if computed

if (size(weights) ~= size(obs))
   error('kde : obs and weights must have same dims');
end
if (length(size(obs)) > 2) || (size(obs, 1) > 1)
    error('kde : obs must be a row')
end    
m2 = sum(obs.^2 .* weights);
sig = sqrt(m2 - sum(obs .* weights).^2);
bw =  sig * (4/(3*size(obs,2)))^(1/5);
opt_argin = length(varargin);
adjust = 1;
if opt_argin >=1 
   adjust = varargin{1};
end
if opt_argin >=2;
   bw = varargin{2};
end
bw = bw * adjust;
xmax = max(obs);
xmin = min(obs);
x = linspace(xmin - 4 * bw, xmax + 4 * bw, 100);
x_mu = bsxfun(@minus, x', obs);
f = exp(-0.5*(x_mu./bw).^2) * weights' ./ (sqrt(2 * pi) * bw);
