function seed = get_seed()

if (isoctave)
       s = rand('state');
       rand('state',sum(100*clock)); 
       seed = double(randi(intmax));
       rand('state',s);
else
%        s=rng('shuffle');
       seed = randi(intmax);
%        rng(s);
end