function s=parse_varname(str)
% PARSE_VARNAME parse a string of the form
z1=textscan(str, '%s', 'delimiter', '[');
z1=z1{1};
s=struct('name', z1{1}); 
if (length(z1) == 1)
  s.lower=cell(0);
  s.upper=cell(0);
else
  z2=textscan(z1{2},'%s', 'delimiter',']');
  z2=z2{1}{1};
  chps=textscan(z2, '%s', 'delimiter', ',');
  res=cellfun(@(x) textscan(x,'%d','delimiter',':'), chps{1});
  s.lower=cellfun(@(x) x(1), res);
  s.upper=cellfun(@(x) x(end), res);
end
