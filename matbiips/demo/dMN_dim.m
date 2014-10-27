function mu_dim = dMN_dim( mu_dim, Sig_dim )
% DMN_DIM Dimension of distribution dMN output
%   Check dimensions of the input and return
%   dimension of the output of distribution dMN
if prod(mu_dim)~=mu_dim(1) || numel(Sig_dim)~=2 || any(mu_dim(1)~=Sig_dim)
    error('invalid parameters dimensions in distribution dMN')
end
