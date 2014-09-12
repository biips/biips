function test_kde()
X=4*randn(1,30)+12+0.5*randn(1,30)-3;
w=ones(size(X))/length(X);
bw=bw_select(X, w);
s=kde(X, w, bw, 100);
figure(1);
plot(s.x,s.f);
[f,x,u]=ksdensity(X);
figure(2);
plot(x,f);
