function sample_param = pmmh_set_param(console, pn_param, inits)

n_param = numel(pn_param);
sample_param = cell(n_param, 1);

% Set init values in Biips
if ~isempty(inits)
    for i=1:n_param
        % Take init value in inits parameters
        ok = matbiips('change_data', console, pn_param(i).name, ...
            pn_param(i).lower, pn_param(i).upper, inits{i}, true);
        if ~ok
            error('Data change failed: invalid initial value for variable %s', pn_param(i).name);
            %%% TODO more precise error message
        end
        sample_param{i} = inits{i};
    end
else
    for i=1:n_param
        try
            sample_param{i} = matbiips('sample_data', console, pn_param(i).name,...
                pn_param(i).lower, pn_param(i).upper, get_seed());
        catch
            error('CANNOT SAMPLE VARIABLE: BUG TO BE FIXED')
        end
    end
end