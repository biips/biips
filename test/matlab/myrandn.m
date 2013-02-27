function Z=myrandn(id,m,n)
Z=inter_boost('rnorm',id, m, n);
fprintf(1, '%f ',Z);
