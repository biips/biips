function biips_clear(varargin)

%
% BIIPS_CLEAR  clears one or several biips consoles
%
%   biips_clear(obj) clears the internal console of the biips object
%   biips_clear(obj, obj2) 
%   biips_clear()    clears all created consoles
%
%   See also BIIPS_MODEL, BIIPS_PIMH_INIT, BIIPS_PMMH_INIT
%--------------------------------------------------------------------------
% EXAMPLE
% model = biips_model('model.bug');
% biips_clear(model);
%--------------------------------------------------------------------------

% BiiPS Project - Bayesian Inference with interacting Particle Systems
% MatBiips interface
% Authors: Adrien Todeschini, Marc Fuentes, Fran�ois Caron
% Copyright (C) Inria
% License: GPL-3
% Jan 2014; Last revision: 17-03-2014
%--------------------------------------------------------------------------

if nargin==0
    matbiips('clear_all_console');
    clear matbiips
else
    for k=1:numel(varargin)
        obj = varargin{k};
        if ~isstruct(obj) || ~isfield(obj, 'class') || ~ischar(obj.class)
            warning('skipping unrecognized argument nb %d', k)
            continue
        end
        switch obj.class
            case 'biips'
                check_struct(obj, 'biips');
                %%% TODO check console id
                matbiips('clear_console', obj.id);
            case 'pimh'
                check_struct(obj, 'pimh');
                % pimh does not clone the console
                continue
            case 'pmmh'
                check_struct(obj, 'pmmh');
                biips_clear(obj.model);
            otherwise
                warning('skipping unrecognized argument nb %s', k)
        end
    end
end