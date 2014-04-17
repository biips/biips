clear
close all

N=100;
ids=zeros(N,1);
for i=1:N;
  ids(i)=matbiips('make_console');
end
p=randperm(N);
for i=1:N
   matbiips('clear_console',ids(p(i)));
end
