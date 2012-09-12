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
%  \file     run_backward_smoother.m
%  \brief    
%
%  \author   $LastChangedBy$
%  \date     $LastChangedDate$
%  \version  $LastChangedRevision$
%  Id:       $Id$
%

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
