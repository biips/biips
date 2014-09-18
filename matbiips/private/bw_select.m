function [ bw ] = bw_select( samples, weights )
%BW_SELECT Computes bandwidth for kernel density estimates
%   Detailed explanation goes here

samples = samples(:);
weights = numel(weights)*weights(:);

% Get a robust estimate of sigma
med = matbiips('wtd_quantile', samples, weights, 0.5);
sig = matbiips('wtd_quantile', abs(samples-med), weights, 0.5) / 0.6745;
if sig<=0
    sig = max(samples)-min(samples);
end

% m2 = sum(samples.^2 .* weights);
% sig = sqrt(m2 - sum(samples .* weights).^2

if sig>0
    % Default window parameter is optimal for normal distribution)
    bw =  sig * (4/(3*numel(samples)))^(1/5);
else
    bw = 1;
end



end

