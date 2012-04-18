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
%  \file     run_kalman_filter_1D.m
%  \brief    
%
%  \author   $LastChangedBy$
%  \date     $LastChangedDate$
%  \version  $LastChangedRevision$
%  Id:       $Id$
%

function [x_est_KF,var, x_pred_KF, var_pred, log_norm_const] = run_kalman_filter_1D(x_0, var_0, t_final, u, y, f_t, c_t, q_t, h_t, r_t)

% Step 1 : Initialization
%========================
x_est_KF = zeros(1,t_final+1);
x_pred_KF = zeros(1,t_final+1);
log_norm_const = zeros(1,t_final+1);

x_est_KF(1) = x_0;
x_pred_KF(1) = x_0;
var = zeros(1,t_final+1);
var_pred = zeros(1,t_final+1);
var(:,1) = var_0;
var_pred(:,1) = var_0;

% Step 2 : Iterations
%====================
for t = 1 : t_final
    [x_est_KF(t+1), var(t+1), x_pred_KF(t+1), var_pred(t+1), y_pred_tplus1, s_tplus1] = kalman_filter_1D(x_est_KF(t), var(t), y(t), u(t), f_t, c_t, q_t, h_t, r_t);
    log_norm_const(t+1) = log_norm_const(t) - 0.5*( log(2*pi*s_tplus1) + (y(t)-y_pred_tplus1)^2/s_tplus1 );
end
