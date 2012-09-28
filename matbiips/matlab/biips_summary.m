function [ summ ] = biips_summary( part )
%BIIPS_SUMMARY Summary of this function goes here
%   Detailed explanation goes here

d = length(size(part.values));
summ.mean = sum(part.values .* part.weights, d);
summ.var = sum(part.values.^2 .* part.weights, d) - summ.mean.^2;

end