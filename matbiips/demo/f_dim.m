function x_dim = f_dim( x_dim, t_dim )
% F_DIM Dimension of function f output
%   Check dimensions of the input and return
%   dimension of the output of function f
if prod(x_dim)~=1 || prod(t_dim)~=1
    error('invalid parameters dimensions in function f')
end
