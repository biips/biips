function [ summ ] = summary(part, varargin)
% SUMMARY computes some statistics on data
% sum = summary(particules, [probas_quantiles]) 
% INPUT : 
% - particules : structure containing two field
%                * values : multidimensional array with last dimension corresponding to particules
%                * weights : corresponding multimensional array for the weigths
% - probas_quantiles : array containing the probabilities used to compute quantiles
% 
% OUTPUT :
% - summary : structure containing some statistics
%             * mean 
%             * variance
%             * momX (with X in {2,3,4}) : moment             
%             * kurt : kurtosis
%             * skew : skewness
%             * quants : quantiles
%             * med : median

opt_argin = length(varargin);
probas = [];
if opt_argin >=1
   probas = varargin{1}; 
end
d = length(size(part.values));
size_part = size(part.values);
summ.mean = sum(part.values .* part.weights, d);
summ.mom2 =  sum(part.values.^2 .* part.weights, d); 
summ.mom3 =  sum(part.values.^3 .* part.weights, d); 
summ.mom4 =  sum(part.values.^4 .* part.weights, d); 
summ.var = summ.mom2 - summ.mean.^2;
summ.skew = (summ.mom3 - 3 .* summ.mom2 .* summ.mean + 2 .* summ.mean.^3)./summ.var.^(3/2);
summ.kurt = (summ.mom4 - 4 .* summ.mom3 .* summ.mean + 6 .* summ.mom2.*summ.mean.^2 - 3 * summ.mean.^4)./summ.var.^2 -3;
cv = num2cell(part.values, d);
cs = num2cell(part.weights, d);
if (~isempty(probas))
 quantiles = cellfun(@(w,x) inter_biips('weighted_quantiles', w, x, probas), cv, cs, 'UniformOutput',0);
 summ.quant=reshape(cat(d, quantiles{:}), [length(probas) size_part(1:d-1)]);
end
med = cellfun(@(w,x) inter_biips('weighted_quantiles', w, x, 0.5), cv, cs, 'UniformOutput',0);
summ.med = reshape(cat(d, med{:}) , [1 size_part(1:d-1)]);
end

