function summ = summary(part, probas, order)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% SUMMARY computes some statistics on data
% summ = summary(part, probas, order)
% INPUT:
% - part:   structure containing two fields
%                * values: multidimensional array with last dimension 
%                           corresponding to particules
%                * weights: corresponding multimensional array for the weigths
% - probs:  array containing the probabilities used to compute quantiles
% -order:   integer. The maximum wanted order for the moment statistics.
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% BiiPS Project - Bayesian Inference with interacting Particle Systems
%
% Reference: A. Todeschini, M. Fuentes, F. Caron, P. Legrand, P. Del Moral.
% BiiPS: a software for Bayesian inference with interacting particle
% systems. Technical Report, INRIA. February 2014.
%
% Authors: Adrien Todeschini, Marc Fuentes
% INRIA Bordeaux, France
% email: biips-project@lists.gforge.inria.fr
% Website: https://alea.bordeaux.inria.fr/biips
% Jan 2014; Last revision: 30-01-2014
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


size_part = size(part.values);
d = length(size_part);
summ.mean = sum(part.values .* part.weights, d);
err = bsxfun(@minus, part.values, summ.mean);
if order>=2
    summ.var = sum(err.^2 .* part.weights, d);
    summ.mom2 =  sum(part.values.^2 .* part.weights, d);
end
if order>=3
    summ.mom3 =  sum(part.values.^3 .* part.weights, d);
    summ.skew = (summ.mom3 - 3 .* summ.mom2 .* summ.mean + 2 .* summ.mean.^3)./summ.var.^(3/2);
end
if order>=4
    summ.mom4 =  sum(part.values.^4 .* part.weights, d);
    summ.kurt = (summ.mom4 - 4 .* summ.mom3 .* summ.mean + 6 .* summ.mom2.*summ.mean.^2 - 3 * summ.mean.^4)./summ.var.^2 -3;
end
cv = num2cell(part.values, d);
cs = num2cell(part.weights, d);
size_q = size_part(1:d-1);
if length(size_q) == 1
    size_q = [size_q, 1];
end
if (~isempty(probas))
%     q = inter_biips('weighted_quantiles', part.values, part.weights, probas)    
    quantiles = cellfun(@(x,w) inter_biips('weighted_quantiles', x, w, probas), cv, cs, 'UniformOutput',0);
    summ.quant = reshape(cat(2, quantiles{:}), [length(probas), size_q]);
end
med = cellfun(@(x,w) inter_biips('weighted_quantiles', x, w, 0.5), cv, cs, 'UniformOutput',0);

summ.med = reshape(cat(2, med{:}), size_q);
end

