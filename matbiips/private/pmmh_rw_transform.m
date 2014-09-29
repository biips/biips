function [out] = pmmh_rw_transform(sample_param, obj, funtype)

%--------------------------------------------------------------------------
% PMMH_RW_TRANSFORM Applies transformation functions to samples
% [sample_tr_vec] = pmmh_rw_transform(sample_param, obj) % direct transformation
% [sample_vec] = pmmh_rw_transform(sample_param_tr, obj, 'inv') % inverse transformation
% [sample_lderiv_vec] = pmmh_rw_transform(sample_param, obj, 'lderiv') % log derivative
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Sept 2014; Last revision: 28-09-2014
%--------------------------------------------------------------------------

if nargin<3
    funtype = 'transform';
end
switch funtype
    case 'transform'
        funcell = obj.transform;
    case 'inverse'
        funcell = obj.transform_inv;
    case 'lderiv'
        funcell = obj.transform_lderiv;
    otherwise
        error('unrecognized function type')
end

% Concatenate all variables in a row vector
sample_vec = cell2mat(cellfun(@(x) x(:) , sample_param(:),...
    'UniformOutput', false));

out_vec = cellfun(@(x,fun) fun(x), num2cell(sample_vec), funcell);

% Rearrange vectorized output to cell of arrays with appropriate
% dimensions
out = cell(1, numel(sample_param));
k=1;
for i=1:numel(sample_param)
    len = prod(obj.dim{i});
    out{i} = reshape(out_vec(k:k+len-1), obj.dim{i});
    k = k + len;
end
