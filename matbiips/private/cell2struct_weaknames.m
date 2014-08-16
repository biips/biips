function s= cell2struct_weaknames(fields, weak_names)

%--------------------------------------------------------------------------
% CELL2STRUCT_WEAKNAMES builds a structure with "illegal" names
% of the form "x[1:5]"
% s= cell2struct_weaknames(fields, weak_names) 
%
%   INPUT 
%   - fields : cell containing of the field to put into the structure
%   - weak_names : cell of strings
%
%   OUTPUT
%   - s : the structure with "illegal" names
%--------------------------------------------------------------------------

% Biips Project - Bayesian Inference with interacting Particle Systems
% Matbiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 18-03-2014
%--------------------------------------------------------------------------

if (~isa(fields,'cell'))
    error('cell2struct_weaknames: 1-st argument must be a cell of strings')
end

if (~isa(weak_names,'cell'))
    error('cell2struct_weaknames: 2-st argument must be a cell of strings')
end

if (~all(cellfun(@(x) ischar(x), weak_names)))
    error('cell2struct_weaknames: 2-st argument must be a cell of strings')
end


s = matbiips('cell2struct_weak_names', fields, weak_names);
