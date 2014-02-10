function test_parse_varname()
nom = 'test_parse_var_name';

out1 = parse_varname('x[1:2]')
if ((out1.name ~= 'x') || (out1.lower ~= 1) || (out1.upper ~=2))
   error ([nom, ' test 1 failed']);
end

out2 = parse_varname('x');
if ((out2.name ~= 'x') || (~isempty(out2.lower) ) || (~isempty(out2.upper)))
   error ([nom, ' test 2 failed']);
end

out3 = parse_varname('xy[1,2:3]');
if ((any(out3.name ~= 'xy')) || (~all(out3.lower == [1;2])) || (~all(out3.upper == [1;3])))
   error ([nom, ' test 3 failed']);
end
