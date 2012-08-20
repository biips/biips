function s=parse_varname(str);
z1=textscan(str, '%s', 'delimiter', '[');
z1=z1{1};
s=struct('name', z1{1}); 
if (length(z1) == 1)
  s=struct(s, 'lower', nan);
  s=struct(s, 'upper', nan);
else
  z2=textscan(z1{2},'%s', 'delimiter',']');
  z2=z2{1}{1};
  chps=textscan(z2, '%s', 'delimiter', ',')
end
  	

