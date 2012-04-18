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
%  \file     kalman_filter_1D.m
%  \brief    
%
%  \author   $LastChangedBy$
%  \date     $LastChangedDate$
%  \version  $LastChangedRevision$
%  Id:       $Id$
%

function [x_tplus1, var_tplus1, x_pred_tplus1, var_pred_tplus1, y_pred_tplus1, s_tplus1] = kalman_filter_1D(x_t, var_t, y_t, u_t, f_t, c_t, q_t, h_t, r_t)

% Prediction
x_pred_tplus1 = f_t .* x_t + c_t * u_t;
var_pred_tplus1 = f_t.^2 .* var_t + q_t;
y_pred_tplus1 = h_t .* x_pred_tplus1;

% Update
inn_tplus1 = y_t - y_pred_tplus1;
s_tplus1 = h_t.^2 .* var_pred_tplus1 + r_t;
k_tplus1 = h_t .* var_pred_tplus1 ./ s_tplus1;
x_tplus1 = x_pred_tplus1 + k_tplus1 .* inn_tplus1;
var_tplus1 = (1 - h_t .* k_tplus1) .* var_pred_tplus1;

end