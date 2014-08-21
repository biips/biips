function [s] = kde(obs, weights, adjust, bw)
%--------------------------------------------------------------------------
% KDE kernel gaussian density estimation
% [s] = kde(obs, weights, [adjust , bw])
%   INPUT
%   - obs:      observation 1D data row vector
%   - weigths:  weights row vector
%   - adjust:   scaling factor for the bw
%   - bw:       bandwidth. if empty, bw is estimated from the samples
%
%   OUTPUT
%   - s:   structure with the following fields :  
%          * x:   points of the density (100 points in the range [min-4*bw, max+4*bw]
%          * f:   values of the density 
%          * bw:  bandwidth , useful only if computed
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

weights = weights(:)';
obs = obs(:)';
if (size(weights) ~= size(obs))
   error('kde : obs and weights must have same dims');
end
m2 = sum(obs.^2 .* weights);
sig = sqrt(m2 - sum(obs .* weights).^2);

if isempty(bw)
    bw =  sig * (4/(3*size(obs,2)))^(1/5);
    bw = bw * adjust;
end
xmax = max(obs);
xmin = min(obs);
x = linspace(xmin - 4 * bw, xmax + 4 * bw, 100);
x_mu = bsxfun(@minus, x', obs);
f = exp(-0.5*(x_mu./bw).^2) * weights' ./ (sqrt(2 * pi) * bw);
s = struct('x', x, 'f', f', 'bw', bw);
