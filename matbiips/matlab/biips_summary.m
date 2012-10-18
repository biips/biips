function [ summ ] = biips_summary( part )
%BIIPS_SUMMARY Summary of this function goes here
%   Detailed explanation goes here

d = length(size(part.values));
summ.mean = sum(part.values .* part.weights, d);
summ.mom2 =  sum(part.values.^2 .* part.weights, d); 
summ.mom3 =  sum(part.values.^3 .* part.weights, d); 
summ.mom4 =  sum(part.values.^4 .* part.weights, d); 
summ.var = summ.mom2 - summ.mean.^2;
summ.skew = (summ.mom3 - 3 * summ.mom2 * summ.mean + 2 * summ.mean.^3)./sum.var.^3/2
%summ.kurt = summ.mom4 - 4* summ.mom3 .* summ.mean
end
