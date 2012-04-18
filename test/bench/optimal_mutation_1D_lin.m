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
%  \file     optimal_mutation_1D_lin.m
%  \brief    
%
%  \author   $LastChangedBy$
%  \date     $LastChangedDate$
%  \version  $LastChangedRevision$
%  Id:       $Id$
%

function [x_tplus1, log_w_update] = optimal_mutation_1D_lin(x_t, y_tplus1, a, sigma_u, b, sigma_v)

var_p = 1 / (1/sigma_u^2 + b^2/sigma_v^2);
mu_p = var_p * (a*x_t/sigma_u^2 + b*y_tplus1/sigma_v^2);

x_tplus1 = mu_p + sqrt(var_p) * randn(size(x_t));

var_const = sigma_u^2 + sigma_v^2 / b^2;
mu_const = y_tplus1/b;

log_w_update = -0.5 * ( log(2*pi*var_const) + (a*x_t - mu_const).^2 / var_const);

% A = sigma_u^2 + sigma_v^2 / b^2;
% B = 1/sigma_u^2 + b^2/sigma_v^2;
% 
% mu_p     = (a*x_t/sigma_u^2 + b*y_tplus1/sigma_v^2) / B;
% sigma_p  = sqrt(1 / B);
% x_tplus1 = mu_p + sigma_p * randn(size(x_t));
% 
% log_w_update = -1/2 * (log(2*pi) + log(A) + (a*x_t - y_tplus1/b).^2 / A );
end
