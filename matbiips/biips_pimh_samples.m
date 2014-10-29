function [obj_pimh, samples_pimh, varargout] = biips_pimh_samples(obj_pimh, n_iter, n_part, varargin)
% BIIPS_PIMH_SAMPLES Perform iterations for the PIMH algorithm and returns samples.
% [obj_pimh, samples_pimh, log_marg_like] = biips_pimh_samples(obj_pimh,...
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
%   - log_marg_like: vector of log marginal likelihood over iterations
%
%   See also BIIPS_MODEL, BIIPS_PIMH_INIT, BIIPS_PIMH_UPDATE
%--------------------------------------------------------------------------
% EXAMPLE:
% modelfile = 'hmm.bug';
% type(modelfile);
% 
% data = struct('tmax', 10, 'logtau', log(10));
% model = biips_model(modelfile, data, 'sample_data', true);
% n_part = 50;
% obj_pimh = biips_pimh_init(model, {'x'}); % Initialize
% [obj_pimh, lml_pimh_burn] = biips_pimh_update(obj_pimh, 200, n_part); % Burn-in
% [obj_pimh, out_pimh, lml_pimh] = biips_pimh_samples(obj_pimh, 200, n_part); % Samples
% 
% out_pimh
% summ_pimh = biips_summary(out_pimh)
% dens_pimh = biips_density(out_pimh)
% 
% out_pimh.x
% summ_pimh = biips_summary(out_pimh.x)
% dens_pimh = biips_density(out_pimh.x)
% 
% figure
% subplot(2,2,1); hold on
% plot([lml_pimh_burn, lml_pimh])
% xlabel('PIMH iteration')
% ylabel('log marginal likelihood')
% 
% subplot(2,2,2); hold on
% plot(0, model.data.x_true(1), 'g>', 'markerfacecolor', 'g')
% plot(out_pimh.x(1,:))
% xlabel('PIMH iteration')
% ylabel('x[1]')
% 
% summ_pimh = biips_summary(out_pimh, 'order', 2, 'probs', [.025, .975]);
% 
% subplot(2,2,3); hold on
% plot(model.data.x_true, 'g')
% plot(summ_pimh.x.mean, 'b')
% plot(summ_pimh.x.quant{1}, '--b')
% plot(summ_pimh.x.quant{2}, '--b')
% xlabel('t')
% ylabel('x[t]')
% legend('true', 'PIMH estimate')
% legend boxoff
% 
% dens_pimh = biips_density(out_pimh);
% 
% subplot(2,2,4); hold on
% plot(model.data.x_true(1), 0, 'g^', 'markerfacecolor', 'g')
% plot(dens_pimh.x(1).x, dens_pimh.x(1).f, 'b')
% xlabel('x[1]')
% ylabel('posterior density')
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
