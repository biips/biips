% Test Matlab function Lotka-Volterra
close all
clear all
x(:,1)=[100;100];

delta = .5;
c1 = .5*delta;
c2 = .0025*delta;
c3 = .3*delta;
dt = 1;
n_part = 100;
T = 20;

profile on
tic
for i=1:n_part
    for t=2:T
        x(:, t) = lotka_volterra_gillespie(x(:,t-1), c1, c2, c3, dt);
    end
end
toc
profile off

figure
plot(x')