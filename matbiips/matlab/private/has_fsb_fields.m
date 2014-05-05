function has_fsb = has_fsb_fields( S )
%HAS_FSB_FIELDS check is structure S has f, s or b fields
%   Detailed explanation goes here

varnames = fieldnames(S);

has_fsb = false;
if all(ismember(varnames, {'f', 's', 'b'}))
    has_fsb = true;
    % check subfields
    for i=1:numel(varnames)
        s = getfield(S, varnames{i});
        if ~isstruct(s) || ~isfield(s, 'values') || ~isfield(s, 'weights') || ~isfield(s, 'ess')
            has_fsb = false;
            break
        end
    end
end

end

