function [obj_pimh, samples_pimh, varargout] = biips_pimh_samples(obj_pimh, n_iter, n_part, varargin)
% BIIPS_PIMH_SAMPLES Perform iterations for the PIMH algorithm and returns samples.
% [obj_pimh, samples_pimh, log_marg_like, accept_rate] = biips_pimh_samples(obj_pimh,...
%                           n_iter, n_part, 'PropertyName', PropertyValue, ...)
%
%   INPUT:
%   - obj_pimh:     structure. PIMH object as returned by BIIPS_PIMH_INIT
%   - n_iter:       integer. Number of iterations
%   - n_part:       integer. Number of particles used in SMC algorithms
%   Optional Inputs:
%   - thin :        integer. Thinning interval. Returns samples every 'thin' iterations
%                   (default = 1)
%   - rs_thres, rs_type, ... : Additional arguments to be passed to the SMC
%      algorithm. See BIIPS_SMC_SAMPLES for for details.
%
%   OUTPUT:
%   - obj_pimh:     structure. updated PIMH object
%   - samples_pimh: structure. PIMH samples for each monitored variable
%   Optional Output:
%   - log_marg_like: vector of log marginal likelihood estimates over iterations
%   - accept_rate: vector of acceptance rates over iterations
%
%   See also BIIPS_MODEL, BIIPS_PIMH_INIT, BIIPS_PIMH_UPDATE

%--------------------------------------------------------------------------
% EXAMPLE:
% modelfile = 'hmm.bug';
% type(modelfile);
% 
% data = struct('tmax', 10, 'p', [.5; .5], 'logtau_true', log(1), 'logtau', log(1));
% model = biips_model(modelfile, data);
% 
% n_part = 50;
% obj_pimh = biips_pimh_init(model, {'x', 'c[2:10]'}); % Initialize
% [obj_pimh, lml_pimh_burn] = biips_pimh_update(obj_pimh, 100, n_part); % Burn-in
% [obj_pimh, out_pimh, lml_pimh] = biips_pimh_samples(obj_pimh, 100, n_part); % Samples
% 
% out_pimh
% summ_pimh = biips_summary(out_pimh)
% 
% summ_pimh_x = biips_summary(out_pimh.x, 'order', 2, 'probs', [.025, .975])
% dens_pimh_x = biips_density(out_pimh.x)
% 
% out_pimh_c = getfield(out_pimh, 'c[2:10]');
% summ_pimh_c = biips_summary(out_pimh_c)
% table_pimh_c = biips_table(out_pimh_c)
% 
% figure
% subplot(2,1,1); hold on
% plot([lml_pimh_burn, lml_pimh])
% xlabel('PIMH iteration')
% ylabel('log marginal likelihood')
% 
% t = 5;
% subplot(2,1,2); hold on
% plot(0, model.data.x_true(t), 'g>', 'markerfacecolor', 'g')
% plot(out_pimh.x(t,:))
% xlabel('PIMH iteration')
% ylabel(sprintf('x[%d]', t))
% 
% figure
% subplot(2,2,1); hold on
% plot(model.data.x_true, 'g')
% plot(summ_pimh_x.mean, 'b')
% plot([summ_pimh_x.quant{:}], '--b')
% xlabel('t')
% ylabel('x[t]')
% legend('true', 'PIMH estimate')
% legend boxoff
% 
% subplot(2,2,2,'YTick',zeros(1,0)); hold on
% bar(1:data.tmax, 1+.5*(model.data.c_true==1), 'g', 'barwidth', 1, 'basevalue', 1, 'edgecolor', 'none')
% text(2, 1.75, 'true')
% bar(2:data.tmax, .5*(summ_pimh_c.mode==1), 'b', 'barwidth', 1, 'basevalue', 0, 'edgecolor', 'none')
% text(2, .75, 'PIMH mode')
% xlim([1,data.tmax+1])
% ylim([0,2])
% xlabel('t')
% ylabel('c[t]==1')
% 
% subplot(2,2,3); hold on
% plot(model.data.x_true(t), 0, 'g^', 'markerfacecolor', 'g')
% plot(dens_pimh_x(t).x, dens_pimh_x(t).f, 'b')
% xlabel(sprintf('x[%d]', t))
% ylabel('posterior density')
% 
% subplot(2,2,4); hold on
% bar(table_pimh_c(t-1).x, table_pimh_c(t-1).f, 'b', 'barwidth', .1)
% plot(model.data.c_true(t), 0, 'g^', 'markerfacecolor', 'g')
% xlabel(sprintf('c[%d]', t))
% ylabel('posterior probability mass')
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 21-10-2014
%--------------------------------------------------------------------------

return_samples = true;
varargout = cell(nargout-2,1);
[obj_pimh, samples_pimh, varargout{:}] = pimh_algo(obj_pimh, n_iter, n_part, return_samples, varargin{:});
