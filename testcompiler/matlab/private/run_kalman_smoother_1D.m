%                                               -*- matlab -*-
%
%  BiiPS software is a set of C++ libraries for
%  Bayesian inference with interacting Particle Systems.
%  Copyright (C) Inria, 2012
%  Authors: Adrien Todeschini, Francois Caron
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
%  \file     run_kalman_smoother_1D.m
%  \brief    
%
%  \author   $LastChangedBy$
%  \date     $LastChangedDate$
%  \version  $LastChangedRevision$
%  Id:       $Id$
%

function [x_est_KS, var_KS] = run_kalman_smoother_1D(x_est_KF, var_KF, x_pred_KF, var_pred_KF, t_final, a)

x_est_KS = zeros(size(x_est_KF));
var_KS = zeros(size(var_KF));

x_est_KS(:,t_final+1) = x_est_KF(:,t_final+1);
var_KS(:,t_final+1) = var_KF(:,t_final+1);

for t = t_final : -1 : 1
    C_t = a .* var_KF(:,t) ./ var_pred_KF(:,t+1);
    var_KS(:,t) = var_KF(:,t) + C_t.^2 .* (var_KS(:,t+1) - var_pred_KF(:,t+1));
    x_est_KS(:,t) = x_est_KF(:,t) + C_t .* (x_est_KS(:,t+1) - x_pred_KF(:, t+1));
end
end
