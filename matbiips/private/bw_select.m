function [ bw ] = bw_select( samples, weights, type )
%BW_SELECT Computes bandwidth for kernel density estimates
% [bw] = bw_select(sample, weights, type)
%   INPUT
%   - samples:  1D samples vector
%   - weigths:  weights vector
%   - type:     character string. The type of bandwidth selector. Possible
%               values:
%                  * 'nrd0' (default): Silverman's rule of thumb.
%                  * 'nrd': variation of Silverman's rule of thumb.
%                  * 'matlab': matlab rule from stat toolbox ksdensity function
%
%   OUTPUT
%   - bw:  postive scalar. the selected bandwidth
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------


if numel(samples) < 2
    error('need at least 2 data points')
end

samples = samples(:);
if isempty(weights)
    weights = ones(numel(samples),1);
else
    weights = numel(weights)*weights(:);
end


switch type
    
    case 'nrd0' % from the bw.nrd0 R function, known as Silverman's ‘rule of thumb’
        % Ref: Silverman, B. W. (1986) Density Estimation. London: Chapman and Hall
        % (page 48, eqn (3.31))
        m2 = sum(samples.^2 .* weights);
        sd = sqrt(m2 - sum(samples .* weights).^2);
        h = diff(matbiips('wtd_quantile', samples, weights, [0.25, 0.75]))/1.34;
        
        lo = [min(sd, h), sd, abs(samples(1)), 1];
        lo = lo(lo~=0);
        
        bw = 0.9 * lo(1) * numel(samples)^(-0.2);
        
    case 'nrd' % from the bw.nrd R function, variation of Silverman with factor 1.06
        % Ref: Scott, D. W. (1992) Multivariate Density Estimation: Theory,
        % Practice, and Visualization. Wiley.
        
        if numel(samples) < 2
            error('need at least 2 data points')
        end
        m2 = sum(samples.^2 .* weights);
        sd = sqrt(m2 - sum(samples .* weights).^2);
        h = diff(matbiips('wtd_quantile', samples, weights, [0.25, 0.75]))/1.34;
        lo = [min(sd, h), sd, abs(samples(1)), 1];
        lo = lo(lo~=0);
        bw = 1.06 * lo * numel(samples)^(-0.2);
        
    case 'matlab' % from the ksdensity function of matlab statistical toolbox
        % Get a robust estimate of sigma
        med = matbiips('wtd_quantile', samples, weights, 0.5);
        sig = matbiips('wtd_quantile', abs(samples-med), weights, 0.5) / 0.6745;
        if sig<=0
            sig = max(samples)-min(samples);
        end
        if sig>0
            % Default window parameter is optimal for normal distribution)
            bw =  sig * (4/(3*numel(samples)))^(1/5);
        else
            bw = 1;
        end
    otherwise
        error('unrecognized type')
end


end

