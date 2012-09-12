N=100;
ids=zeros(N,1);
for i=1:N;
  ids(i)=inter_biips('make_console');
end
p=randperm(N);
for i=1:N
   inter_biips('clear_console',ids(p(i)));
end
