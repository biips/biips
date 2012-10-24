function [ summ ] = biips_summary( part )
%BIIPS_SUMMARY Summary of this function goes here
%   Detailed explanation goes here

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
cs=  num2cell(part.weights, d);
probas = [0.025 0.0975];
quantiles = cellfun(@(w,x) inter_biips('weighted_quantiles', w, x, probas), cv, cs, 'UniformOutput',0);
summ.quants=reshape(cat(3, quantiles{:}), [length(probas) size_part(1:d-1)]);
end
