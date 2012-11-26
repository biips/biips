function test_kde()
x1=4*randn(1,30)+12;
x2=0.5*randn(1,30)-3;
s=kde(x1+x2, ones(size(x1))/length(x1));
figure(1);
plot(s.x,s.f);
[f,x,u]=ksdensity(x1+x2);
figure(2);
plot(x,f);
