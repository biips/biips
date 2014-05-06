function summ = summary_stat(values, weights, probs, order, mode)

%--------------------------------------------------------------------------
% SUMMARY_STAT computes univariate statistics on weighted samples
% summ = summary_stat(values, weights, probs, order, mode)
% 
% INPUT:
% - values:     array with last dimension corresponding to particles
% - weights:    array of the same size as values
% - probs:      vector of the quantile probabilities
% - order:      integer. The maximum wanted order for the moment based statistics.
% - mode:       logical. activate mode computation
%               WARNING: mode is based onthe frequencies table.
%               It has to be used only for dicrete variables, not for
%               continuous ones.
%
% OUTPUT:
% - summ : structure containing componentwise univariate statistics.
%             fields can be:
%             - mean
%             - var : variance
%             - kurt : kurtosis
%             - skew : skewness
%             - probs : vector of quantile probabilities
%             - quant : cell of quantile values
%             - mode : most frequent values for discrete variables
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

d = ndims(values);

%% moment based statistics
if order>=1
summ.mean = sum(values .* weights, d);
end
if order>=2
    summ.var = sum(bsxfun(@minus, values, summ.mean).^2 .* weights, d);
end
if order>=3
    mom2 =  sum(values.^2 .* weights, d);
    mom3 =  sum(values.^3 .* weights, d);
    summ.skew = (mom3 - 3 .* mom2 .* summ.mean + 2 .* summ.mean.^3)./summ.var.^(3/2);
end
if order>=4
    summ.kurt = (sum(values.^4 .* weights, d) - 4 .* mom3 .* summ.mean + ...
        6 .* mom2.*summ.mean.^2 - 3 * summ.mean.^4)./summ.var.^2 -3;
end

if ~isempty(probs) || mode
    values = num2cell(values, d);
    weights = num2cell(weights, d);
end

%% quantiles
if ~isempty(probs)
    dim = size(values);
    summ.probs = probs(:);
    quantiles = cellfun(@(x,w) matbiips('wtd_quantile', x(:), numel(w)*w(:), probs), ...
        values, weights, 'UniformOutput',0);
    
    % reshape quantiles
    summ.quant = reshape([quantiles{:}], [numel(probs), dim]);
    summ.quant = shiftdim(summ.quant, 1);
    summ.quant = reshape(summ.quant, [dim, numel(probs)]);
    summ.quant = squeeze(num2cell(summ.quant, 1:d-1));
    % quant is a cell with as many components as probabilities
end

% med = cellfun(@(x,w) matbiips('wtd_quantile', x, numel(w)*w, 0.5), values, weights, 'UniformOutput',0);
% summ.med = reshape([med{:}], dim);

%% mode
if mode
    % unique values and indices
    [U, ~, Iu] = cellfun(@unique, values, 'uniformoutput', 0);
    % sum of weights for unique values
    W = cellfun(@(w, iu) sum(bsxfun(@times, w(:), bsxfun(@eq, iu, 1:max(iu)))), weights, Iu, 'uniformoutput', 0);
    % indices of the maximum sum of weights
    [~, Im] = cellfun(@max, W, 'uniformoutput', 0);
    % values with the maximum sum of weights
    summ.mode = cellfun(@(u, im) u(im), U, Im);
end
