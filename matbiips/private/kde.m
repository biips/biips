function [s] = kde(sample, weights, bw, npoints)
%--------------------------------------------------------------------------
% KDE kernel density estimation with gaussian kernels
% [s] = kde(sample, weights, [adjust , bw])
%   INPUT
%   - sample:   vector sample of 1D data
%   - weigths:  weights vector
%   - bw:       bandwidth
%   - npoints:  number of equally-spaced points of evaluation.
%
%   OUTPUT
%   - s:   structure with the following fields :  
%          * x:   points of the density (100 points in the range [min-4*bw, max+4*bw]
%          * f:   values of the density 
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

weights = weights(:)';
sample = sample(:)';
if numel(weights) ~= numel(sample)
   error('kde : obs and weights must have same dims');
end
xmax = max(sample);
xmin = min(sample);
x = linspace(xmin - 4 * bw, xmax + 4 * bw, npoints);
x_mu = bsxfun(@minus, x', sample);
f = exp(-0.5*(x_mu./bw).^2) * weights' ./ (sqrt(2 * pi) * bw);
s = struct('x', x, 'f', f');
