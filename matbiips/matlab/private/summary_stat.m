function summ = summary_stat(values, weights, proba, order)

%--------------------------------------------------------------------------
% SUMMARY_STAT computes some statistics on data
% summ = summary_stat(part, proba, order)
% INPUT:
% - values:     multidimensional array with last dimension corresponding to
%               particles
% - weights:    corresponding multidimensional array for the weigths
% - proba:      array containing the probabilities used to compute quantiles
% - order:       integer. The maximum wanted order for the moment statistics.
%
% OUTPUT:
% - summary : structure containing some statistics
%             * mean
%             * var
%             * momX (with X in {2,3,4}) : moment
%             * kurt : kurtosis
%             * skew : skewness
%             * proba : probabilities
%             * quant : quantiles
%             * med : median 
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

d = ndims(values);
summ.mean = sum(values .* weights, d);
err = bsxfun(@minus, values, summ.mean);
if order>=2
    summ.var = sum(err.^2 .* weights, d);
    summ.mom2 =  sum(values.^2 .* weights, d);
end
if order>=3
    summ.mom3 =  sum(values.^3 .* weights, d);
    summ.skew = (summ.mom3 - 3 .* summ.mom2 .* summ.mean + 2 .* summ.mean.^3)./summ.var.^(3/2);
end
if order>=4
    summ.mom4 =  sum(values.^4 .* weights, d);
    summ.kurt = (summ.mom4 - 4 .* summ.mom3 .* summ.mean + 6 .* summ.mom2.*summ.mean.^2 - 3 * summ.mean.^4)./summ.var.^2 -3;
end

values = num2cell(values, d);
weights = num2cell(weights, d);
dim = size(values);

if ~isempty(proba)
    summ.proba = proba(:);
    quantiles = cellfun(@(x,w) matbiips('weighted_quantiles', x(:), numel(w)*w(:), proba), values, weights, 'UniformOutput',0);
    summ.quant = reshape([quantiles{:}], [numel(proba), dim]);
    summ.quant = shiftdim(summ.quant, 1);
    summ.quant = reshape(summ.quant, [dim, numel(proba)]);
    summ.quant = squeeze(num2cell(summ.quant, 1:d-1));
end

med = cellfun(@(x,w) matbiips('weighted_quantiles', x, numel(w)*w, 0.5), values, weights, 'UniformOutput',0);
summ.med = reshape([med{:}], dim);
end