function biips_clear(varargin)

%
% BIIPS_CLEAR  clears one or several biips consoles
%
%   biips_clear(model) clears the console associated to the biips model
% 
%   biips_clear()  clears all the consoles and reset the Id counter to 0.
%
%   See also BIIPS_MODEL
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

if nargin>0
    for k=1:numel(varargin)
        obj = varargin{k};
        if ~isstruct(obj) || ~isfield(obj, 'class') || ~ischar(obj.class)
            warning('skipping unrecognized argument nb %d', k)
            continue
        end
        %%% TODO check model obj
        switch obj.class
            case 'biips'
                console = obj.id;
            case 'pimh'
                % pimh does not clone the console
                continue
            case 'pmmh'
                console = obj.model.id;
            otherwise
                warning('skipping unrecognized argument nb %s', k)
        end
        matbiips('clear_console', console); 
    end
else
    clear matbiips
%     matbiips('clear_all_console');
end
