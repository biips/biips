%                                               -*- matlab -*-
%
%  BiiPS software is a set of C++ libraries for
%  Bayesian inference with interacting Particle Systems.
%  Copyright (C) Inria, 2012
%  Contributors: Adrien Todeschini, Francois Caron
%  
%  BiiPS is derived software based on:
%  JAGS, Copyright (C) Martyn Plummer, 2002-2010
%  SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
%
%  This file is part of BiiPS.
%
%  BiiPS is free software: you can redistribute it and/or modify
%  it under the terms of the GNU General Public License as published by
%  the Free Software Foundation, either version 3 of the License, or
%  (at your option) any later version.
%
%  This program is distributed in the hope that it will be useful,
%  but WITHOUT ANY WARRANTY; without even the implied warranty of
%  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%  GNU General Public License for more details.
%
%  You should have received a copy of the GNU General Public License
%  along with this program.  If not, see <http://www.gnu.org/licenses/>.
%
%
%  \file     backward_smoother_hmm_1d.m
%  \brief    
%
%  \author   $LastChangedBy$
%  \date     $LastChangedDate$
%  \version  $LastChangedRevision$
%  Id:       $Id$
%

function [x_est_PS_t, w_PS_t, ESS_t] = backward_smoother_hmm_1d(x_PF_t, x_PF_tplus1, w_PF_t, w_PS_tplus1, t, log_prior_pdf)
%BACKWARD_SMOOTHER_HMM_1D Summary of this function goes here
%   Detailed explanation goes here

N = size(x_PF_t, 3);

x_tplus1 = zeros(N,1);
x_tplus1(:,1) = x_PF_tplus1(1,1,:);

x_t = zeros(1,N);
x_t(1,:) = x_PF_t(1,1,:);

% F = exp(bsxfun(@(x_tplus1, x_t) log_prior_pdf(x_tplus1, x_t, t), x_tplus1*ones(1, N), x_t));
% F = exp(bsxfun(@(x_tplus1, x_t) log_prior_pdf(x_tplus1, x_t, t), repmat(x_tplus1, 1, N), x_t));
F = exp(bsxfun(@(x_tplus1, x_t) log_prior_pdf(x_tplus1, x_t, t), x_tplus1, x_t));

W_PF_t = zeros(1,N);
W_PF_t(1,:) = w_PF_t(1,1,:);

W_PS_tplus1 = zeros(1,N);
W_PS_tplus1(1,:) = w_PS_tplus1(1,1,:);

w_PS_t = zeros(size(w_PS_tplus1));
w_PS_t(1,1,:) = W_PF_t .* ((W_PS_tplus1 ./ (W_PF_t * F')) * F);

x_est_PS_t = sum(w_PS_t .* x_PF_t);

ESS_t = 1/sum(w_PS_t.^2);

