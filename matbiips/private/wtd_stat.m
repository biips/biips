function stat = wtd_stat(values, weights, probs, order, mode)

%--------------------------------------------------------------------------
% WTD_STAT computes univariate statistics on weighted samples
% summ = wtd_stat(values, weights, probs, order, mode)
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

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

m = ndims(values);

%% moment based statistics
if order>=1
    stat.mean = sum(values .* weights, m);
end
if order>=2
    stat.var = sum(bsxfun(@minus, values, stat.mean).^2 .* weights, m);
end
if order>=3
    mom2 =  sum(values.^2 .* weights, m);
    mom3 =  sum(values.^3 .* weights, m);
    stat.skew = (mom3 - 3 .* mom2 .* stat.mean + 2 .* stat.mean.^3)./stat.var.^(3/2);
end
if order>=4
    stat.kurt = (sum(values.^4 .* weights, m) - 4 .* mom3 .* stat.mean + ...
        6 .* mom2.*stat.mean.^2 - 3 * stat.mean.^4)./stat.var.^2 -3;
end

if ~isempty(probs) || mode
    values = num2cell(values, m);
    weights = num2cell(weights, m);
end

%% quantiles
if ~isempty(probs)
    dim = size(values);
    stat.probs = probs(:);
    quantiles = cellfun(@(x,w) matbiips('wtd_quantile', x(:), numel(w)*w(:), probs), ...
        values, weights, 'UniformOutput',0);
    
    % reshape quantiles
    stat.quant = reshape([quantiles{:}], [numel(probs), dim]);
    stat.quant = shiftdim(stat.quant, 1);
    stat.quant = reshape(stat.quant, [dim, numel(probs)]);
    stat.quant = squeeze(num2cell(stat.quant, 1:m-1));
    % quant is a cell with as many components as probabilities
end

% med = cellfun(@(x,w) matbiips('wtd_quantile', x, numel(w)*w, 0.5), values, weights, 'UniformOutput',0);
% summ.med = reshape([med{:}], dim);

%% mode
if mode
    % unique values and indices
    [U, ~, Iu] = cellfun(@unique, values, 'uniformoutput', 0);
    % sum of weights for unique values
    W = cellfun(@(w, iu) sum(bsxfun(@times, w(:), bsxfun(@eq, iu(:), 1:max(iu)))), weights, Iu, 'uniformoutput', 0);
    % indices of the maximum sum of weights
    [~, Im] = cellfun(@max, W, 'uniformoutput', 0);
    % values with the maximum sum of weights
    stat.mode = cellfun(@(u, im) u(im), U, Im);
end
