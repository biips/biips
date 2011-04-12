function [x_t] = generate_state(dimx, t_final, initiate_particles, evolution_model)
%GENERATE_STATE Summary of this function goes here
%   Detailed explanation goes here

x_t = zeros(dimx, t_final+1);
x_t(:,1) = initiate_particles(1);
for t = 1 : t_final
    x_t(:,t+1) = evolution_model(x_t(:,t), t);
end

end
