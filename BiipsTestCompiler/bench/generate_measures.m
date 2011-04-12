function [ y_t ] = generate_measures(dimy, t_final, x_t, measure_model)
%GENERATE_MEASURES Summary of this function goes here
%   Detailed explanation goes here

y_t = zeros(dimy, t_final);
for t = 1 : t_final
    y_t(:,t) = measure_model(x_t(:,t+1), t);
end

end
