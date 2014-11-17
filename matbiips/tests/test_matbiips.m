clear
close all
test = 'all';% smc, pmmh or all

%% Tests SMC
switch(test)
    case {'smc', 'all'}
    % RUNS A SET OF TESTS

    % Test console
    N=10;
    ids=zeros(N,1);
    for i=1:N;
      ids(i)=matbiips('make_console');
    end
    p=randperm(N);
    for i=1:N
       matbiips('clear_console',ids(p(i)));
    end

    % Test model
    t_max = 10; mean_x_init = 0;prec_x_init = 1;prec_x = 1;prec_y = 10;
    data = struct('t_max', t_max, 'prec_x_init', prec_x_init,...
        'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);

    %%% Compile BUGS model and sample data
    model_filename = 'hmm_1d_lin2.bug'; % BUGS model filename
    sample_data = true; % Boolean
    model = biips_model(model_filename, data, 'sample_data', sample_data);
    data = model.data;
    biips_clear(model)
    
    % Test biips_build_sampler
    model = biips_model(model_filename, data, 'sample_data', sample_data);
    data = model.data;    
    biips_build_sampler(model)
    biips_build_sampler(model,'proposal', 'prior')
    biips_build_sampler(model,'proposal', 'auto')
    biips_clear(model)
    

    % Test model with cell data structure
    t_max = 40; mean_x_init = 0;prec_x_init = 1;prec_x = 1;prec_y = 10;
    data = {'t_max', 'prec_x_init',...
        'prec_x', 'prec_y', 'mean_x_init'};
        
    %%% Compile BUGS model and sample data
    model_filename = 'hmm_1d_lin.bug'; % BUGS model filename
    sample_data = true; % Boolean
    model = biips_model(model_filename, data, 'sample_data', sample_data);
    data = model.data;


    % test model with wrong type for argument
    data = {'t_max', 'prec_x_init',...
        'prec_x', 'prec_y', 'mean_x_init'};
    sample_data = 'tartealacreme';
    err=0;
    try 
        model = biips_model(model_filename, data, 'sample_data', sample_data);
        data = model.data;
    catch err;
        if ~isoctave()
            fprintf('[\bError: %s]\b\n', err.message)
        else            
            fprintf('[\bError: %s]\b\n', lasterr)
        end
    end
    

    %MAKES THE PROGRAMM CRASH [SOLVED]
    % Test model with cell data structure
    datanames = {'t_max', ...
        'prec_x', 'prec_y', 'mean_x_init'};
    %%% Compile BUGS model and sample data
    model_filename = 'hmm_1d_lin.bug'; % BUGS model filename
    sample_data = true; % Boolean
    try(biips_model(model_filename, datanames, 'sample_data', sample_data))
    catch err;
        if ~isoctave()
            fprintf('[\bError: %s]\b\n', err.message)
        else
            fprintf('[\bError: %s]\b\n', lasterr)
        end
    end


    % Test model with unknown file
    badmodel = 'an_unknown_bug_file.bug'; % BUGS model filename
    try(biips_model(badmodel, data, 'sample_data', sample_data))
    catch err;
        if ~isoctave()
            fprintf('[\bError: %s]\b\n', err.message)
        else
            fprintf('[\bError: %s]\b\n', lasterr)
        end
    end

    % Test model with bad filename
    badmodel2 = 2; % BUGS model filename
    try(biips_model(badmodel2, data, 'sample_data', sample_data))
    catch err;
        if ~isoctave()
            fprintf('[\bError: %s]\b\n', err.message)
        else
            fprintf('[\bError: %s]\b\n', lasterr)
        end
    end

    %% TESTS BIIPS_SMC_SAMPLES with vectors
    model_filename = 'hmm_1d_lin.bug'; % BUGS model filename
    model = biips_model(model_filename, data, 'sample_data', sample_data);
    n_part = 20; % Number of particles
    variables = {'x', 'x[1:2]'}; % Variables to be monitored
    type = 'fs'; rs_type = 'stratified'; rs_thres = 0.5; % Optional parameters
    % Run SMC
    out_smc = biips_smc_samples(model, variables, n_part,...
        'type', type, 'rs_type', rs_type, 'rs_thres', rs_thres);
    nodes = biips_nodes(model)
    biips_clear(model)
    
    %% TESTS BIIPS_SMC_SAMPLES with matrices *** MAKES THE PROGRAM CRASH *** [SOLVED]
    model_filename = 'hmm_1d_lin2.bug'; % BUGS model filename
    model = biips_model(model_filename, data, 'sample_data', sample_data);
    n_part = 20; % Number of particles
    variables = {'x', 'x[1:2,1]'}; % Variables to be monitored
    type = 'fs'; rs_type = 'stratified'; rs_thres = 0.5; % Optional parameters
    % Run SMC
    out_smc = biips_smc_samples(model, variables, n_part,...
        'type', type, 'rs_type', rs_type, 'rs_thres', rs_thres);
    nodes = biips_nodes(model)
    biips_clear(model)
end

% pause

%% Test PIMH
switch(test)
    case {'pimh', 'all'}
        
    % Test pimh with vectors
    t_max = 10; mean_x_init = 0;prec_x_init = 1;prec_x = 1;prec_y = 10;
    data = struct('t_max', t_max, 'prec_x_init', prec_x_init,...
        'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);
    %%% Compile BUGS model and sample data
    model_filename = 'hmm_1d_lin.bug'; % BUGS model filename
    sample_data = true; % Boolean
    model = biips_model(model_filename, data, 'sample_data', sample_data);
    data = model.data;
    
    variables = {'x', 'x[1:2]'};
    n_part = 100;
    n_iter = 20;
    obj_pimh = biips_pimh_init(model, variables);
    obj_pimh = biips_pimh_update(obj_pimh, n_iter, n_part);
    [obj_pimh, samples_pimh, log_marg_like_pimh] = biips_pimh_samples(obj_pimh, n_iter, n_part);
    biips_clear(model)
    
    % Test pimh with matrices *** MAKES THE PROGRAM CRASH ***
    t_max = 10; mean_x_init = 0;prec_x_init = 1;prec_x = 1;prec_y = 10;
    data = struct('t_max', t_max, 'prec_x_init', prec_x_init,...
        'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);
    %%% Compile BUGS model and sample data
    model_filename = 'hmm_1d_lin2.bug'; % BUGS model filename
    sample_data = true; % Boolean
    model = biips_model(model_filename, data, 'sample_data', sample_data);
    data = model.data;
    
    variables = {'x', 'x[1:2,1]'};
    n_part = 100;
    n_iter = 20;
    obj_pimh = biips_pimh_init(model, variables);
    obj_pimh = biips_pimh_update(obj_pimh, n_iter, n_part);
    [obj_pimh, samples_pimh, log_marg_like_pimh] = biips_pimh_samples(obj_pimh, n_iter, n_part);
    biips_clear(model)

end
% pause

%% Test PMMH
switch(test)
    case {'pmmh', 'all'}
    
    %% TESTS BIIPS_PMMH
    % Test model with cell data structure
    t_max = 20; mean_x_init = 0;prec_x_init = 1;prec_x = 1; mean0 = [0;0]; prec0 = eye(2);
    data = {'t_max', 'prec_x_init',...
        'prec_x', 'mean_x_init', 'mean0', 'prec0'};
    %%% Compile BUGS model and sample data
    model_filename = 'hmm_1d_nonlin_param2.bug'; % BUGS model filename
    sample_data = true; % Boolean
    model = biips_model(model_filename, data, 'sample_data', sample_data);
    data = model.data;

    param_names = {'log_prec_y[1]', 'log_prec_y[2]','log_prec_y2[1:2]', 'alpha[1:1]'};
    inits = {0, 0, [0;0], 5};
    latent_names = {};
    n_burn = 500;
    n_part = 50;
    n_iter = 520;
    %%
    % *Init PMMH*
    obj_pmmh = biips_pmmh_init(model, param_names, 'inits', inits, 'latent_names', latent_names); % creates a pmmh object

    %%
    % *Run PMMH*
    obj_pmmh = biips_pmmh_update(obj_pmmh, n_burn, n_part); % adaptation and burn-in iterations
    [obj_pmmh, out_pmmh, log_marg_like_pen, log_marg_like, stats_pmmh] = biips_pmmh_samples(obj_pmmh, n_iter, n_part,...
        'thin', 1); % Samples
    biips_clear(model)
end
