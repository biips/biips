function [s] = kde(samples, weights, bw, n)
%--------------------------------------------------------------------------
% KDE kernel density estimation with gaussian kernels
% [s] = kde(samples, weights, bw, n)
%   INPUT
%   - samples:  1D samples vector
%   - weigths:  weights vector
%   - bw:       bandwidth
%   - n:  number of equally-spaced points of evaluation.
% 
%   OUTPUT
%   - s:   structure with the following fields :  
%          * x: points of the density (n points in the range [min-4*bw, max+4*bw]
%          * f: values of the density at x
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

weights = weights(:)';
samples = samples(:)';
if numel(weights) ~= numel(samples)
   error('kde : obs and weights must have same dims');
end
xmax = max(samples);
xmin = min(samples);
x = linspace(xmin - 4 * bw, xmax + 4 * bw, n);
x_mu = bsxfun(@minus, x', samples);
f = exp(-0.5*(x_mu./bw).^2) * weights' ./ (sqrt(2 * pi) * bw);
s = struct('x', x, 'f', f');
