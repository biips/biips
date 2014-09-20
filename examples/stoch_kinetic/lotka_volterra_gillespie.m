function x = lotka_volterra_gillespie(x, c1, c2, c3, dt)

% Simulation from a Lotka-Volterra model with the Gillepsie algorithm
% x1 is the number of prey
% x2 is the number of predator
% R1: (x1,x2) -> (x1+1,x2)      At rate c1x1
% R2: (x1,x2) -> (x1-1,x2+1)    At rate c2x1x2
% R3: (x1,x2) -> (x1,x2-1)      At rate c3xx2

z = [1, -1, 0;
    0, 1, -1];

t = 0;
while true   
    rate = [c1*x(1), c2*x(1)*x(2), c3*x(2)];
    sum_rate = sum(rate);
    t = t - log(rand)/sum_rate; % Sample next event from an exponential distribution
    ind = find((sum_rate*rand)<=cumsum(rate), 1); % Sample the type of event    
    if t>dt
        break
    end
    x = x + z(:, ind);
end