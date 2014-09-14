function seed = get_seed()

seed = double(randi(intmax));

% if isoctave() || verLessThan('matlab', '7.12')
%        s = rand('state');
%        rand('state',sum(100*clock)); 
%        seed = double(randi(intmax));
%        rand('state',s);
% else
%        s=rng('shuffle');
%        seed = randi(intmax);
%        rng(s);
% end
