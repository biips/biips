function [ bw ] = bw_select( samples, weights )
%BW_SELECT Computes bandwidth for kernel density estimates
%   Detailed explanation goes here

m2 = sum(samples.^2 .* weights);
sigma = sqrt(m2 - sum(samples .* weights).^2);
bw =  sigma * (4/(3*size(samples,2)))^(1/5);

end

