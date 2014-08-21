function s=parse_varname(str)

%--------------------------------------------------------------------------
% PARSE_VARNAME parses a string of the form x[1:5]
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

z1=textscan(str, '%s', 'delimiter', '[');
z1=z1{1};
s=struct('name', z1{1}); 
if (numel(z1) == 1)
  s.lower = [];
  s.upper = [];
else
  z2=textscan(z1{2},'%s', 'delimiter',']');
  z2=z2{1}{1};
  chps=textscan(z2, '%s', 'delimiter', ',');
  res=cellfun(@(x) textscan(x,'%f','delimiter',':'), chps{1});
  s.lower=cellfun(@(x) x(1), res);
  s.upper=cellfun(@(x) x(end), res);
end

%%% TODO check validity