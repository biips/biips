function sample_param = pmmh_set_param(console, param_names, pn_param, values)

n_param = numel(pn_param);
sample_param = cell(n_param, 1);

% Set init values in Biips
if ~isempty(values)
    if numel(values) ~= n_param
        error('invalid number of values')
    end
    %% Take sample values in values parameter
    for i=1:n_param
        ok = matbiips('change_data', console, pn_param(i).name, ...
            pn_param(i).lower, pn_param(i).upper, values{i}, true);
        if ~ok
            error('Data change failed: invalid initial value: %s = %g', param_names{i}, values{i});
        end
        sample_param{i} = values{i};
    end
else
    %% or sample init value
    for i=1:n_param
%        try
            sample_param{i} = matbiips('sample_data', console, pn_param(i).name,...
                pn_param(i).lower, pn_param(i).upper, get_seed());
%        catch
%            error('CANNOT SAMPLE VARIABLE %s: BUG TO BE FIXED', param_names{i})
%        end
    end
    %%% FIXME what if the variable is observed ?
end
