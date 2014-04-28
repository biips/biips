function summ = summary_stat(values, weights, probas, order)

%--------------------------------------------------------------------------
% SUMMARY_STAT computes some statistics on data
% summ = summary_stat(part, probas, order)
% INPUT:
% - values:     multidimensional array with last dimension corresponding to
%               particles
% - weights:    corresponding multidimensional array for the weigths
% - probs:      array containing the probabilities used to compute quantiles
% -order:       integer. The maximum wanted order for the moment statistics.
%
% OUTPUT:
% - summary : structure containing some statistics
%             * mean
%             * variance
%             * momX (with X in {2,3,4}) : moment
%             * kurt : kurtosis
%             * skew : skewness
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


size_part = size(values);
d = numel(size_part);
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
cv = num2cell(values, d);
cs = num2cell(weights, d);
size_q = size_part(1:d-1);
if numel(size_q) == 1
    size_q = [size_q, 1];
end
if (~isempty(probas))
%     q = matbiips('weighted_quantiles', part.values, part.weights, probas)    
    quantiles = cellfun(@(x,w) matbiips('weighted_quantiles', x, numel(w)*w, probas), cv, cs, 'UniformOutput',0);
    summ.quant = reshape(cat(2, quantiles{:}), [numel(probas), size_q]);
end
med = cellfun(@(x,w) matbiips('weighted_quantiles', x, numel(w)*w, 0.5), cv, cs, 'UniformOutput',0);

summ.med = reshape(cat(2, med{:}), size_q);
end