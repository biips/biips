function test_kde()
x1=4*randn(1,30)+12;
x2=0.5*randn(1,30)-3;
[fm,xm,bw]=kde(x1+x2, ones(size(x1))/length(x1));
bw
figure(1);
plot(xm,fm);
[f,x,u]=ksdensity(x1+x2);
figure(2);
plot(x,f);

