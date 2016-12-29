function [x_est_PS, w_PS, ESS_PS] = run_backward_smoother(T, x_PF, w_PF, backward_smoother)

x_est_PS = zeros(1,T+1);
w_PS     = zeros(size(w_PF));
ESS_PS   = zeros(1,T+1);

x_est_PS(1,T+1) = sum(w_PF(1,T+1,:) .* x_PF(:,T+1,:), 3);
w_PS(1,T+1,:)   = w_PF(1,T+1,:);
ESS_PS(1,T+1)   = 1/sum(w_PF(1,T+1,:).^2);

for t = T : -1 : 1
    [x_est_PS(1,t), w_PS(1,t,:), ESS_PS(1,t)] = backward_smoother(x_PF(:,t,:), x_PF(:,t+1,:), w_PF(1,t,:), w_PS(1,t+1,:), t);
end

end
