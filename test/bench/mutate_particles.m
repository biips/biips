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
%  \file     mutate_particles.m
%  \brief    
%
%  \author   $LastChangedBy$
%  \date     $LastChangedDate$
%  \version  $LastChangedRevision$
%  Id:       $Id$
%

function [x_tplus1, log_w_tplus1, w_tplus1, x_t_est, ESS_t, log_norm_const_tplus1, varargout] = mutate_particles(x_t, log_w_t, y_tplus1, mutation, t, log_norm_const_t, varargin)

dimx = size(x_t, 1);

% Mutation
if nargin == 6
    [x_tplus1, log_w_update] = mutation(x_t, y_tplus1, t);
elseif nargin == 7
    var_z_t = varargin{1};
    
    [x_tplus1, log_w_update, var_z_tplus1] = mutation(x_t, y_tplus1, var_z_t, t);
    
    varargout = {var_z_tplus1};
end

log_w_tplus1 = log_w_t + log_w_update;

w_tplus1 = exp(log_w_tplus1);
W = sum(w_tplus1);
w_tplus1 = w_tplus1 / W;
log_w_tplus1 = log_w_tplus1 - log(W);

x_t_est = zeros(dimx,1);
% State estimation
for i = 1 : dimx
    x_t_est(i) = sum(w_tplus1 .* x_tplus1(i,1,:));
end
% Quality of the approximation : Effective Sample Size (ESS)
ESS_t = 1 / sum(w_tplus1.^2);

% normalizing constant
log_norm_const_tplus1 = log_norm_const_t + log(W);

end
