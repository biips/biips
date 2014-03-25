close all
clear all
addpath('../matlab/')
test = 'all';% smc, pmmh or all

%% Tests SMC
switch(test)
    case {'smc', 'all'}
    % RUNS A SET OF TESTS

    % Test console
    N=10;
    ids=zeros(N,1);
    for i=1:N;
      ids(i)=inter_biips('make_console');
    end
    p=randperm(N);
    for i=1:N
       inter_biips('clear_console',ids(p(i)));
    end

    % Test model
    t_max = 10; mean_x_init = 0;prec_x_init = 1;prec_x = 1;prec_y = 10;
    data = struct('t_max', t_max, 'prec_x_init', prec_x_init,...
        'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);
    %%% Start BiiPS console
    biips_init;
    %%% Compile BUGS model and sample data
    model = 'hmm_1d_lin2.bug'; % BUGS model filename
    sample_data = true; % Boolean
    [model_id, data] = biips_model(model, data, 'sample_data', sample_data);
    biips_clear(model_id)
    
    % Test biips_build_sampler
    [model_id, data] = biips_model(model, data, 'sample_data', sample_data);
    biips_build_sampler(model_id)
    biips_build_sampler(model_id,'proposal', 'prior')
    biips_build_sampler(model_id,'proposal', 'auto')
    biips_clear(model_id)
    

    % Test model with cell data structure
    t_max = 40; mean_x_init = 0;prec_x_init = 1;prec_x = 1;prec_y = 10;
    data = {'t_max', 'prec_x_init',...
        'prec_x', 'prec_y', 'mean_x_init'};
    %%% Start BiiPS console
    biips_init;
    %%% Compile BUGS model and sample data
    % model = 'hmm_1d_lin.bug'; % BUGS model filename
    sample_data = true; % Boolean
    [model_id, data] = biips_model(model, data, 'sample_data', sample_data);
    


    % test model with wrong type for argument
    data = {'t_max', 'prec_x_init',...
        'prec_x', 'prec_y', 'mean_x_init'};
    sample_data = 'tartealacreme';
    err=0;
    try 
        [model_id, data] = biips_model(model, data, 'sample_data', sample_data);
    catch err;
        if ~isoctave()
            fprintf('[\bError: %s]\b\n', err.message)
        else            
            fprintf('[\bError: %s]\b\n', lasterr)
        end
    end
    

    % MAKES THE PROGRAMM CRASH
    % % Test model with cell data structure
    % data = {'t_max', ...
    %     'prec_x', 'prec_y', 'mean_x_init'};
    % %%% Start BiiPS console
    % biips_init;
    % %%% Compile BUGS model and sample data
    % model = 'hmm_1d_lin.bug'; % BUGS model filename
    % sample_data = true; % Boolean
    % [model_id, data] = biips_model(model, data, 'sample_data', sample_data);


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

    %% TESTS BIIPS_SMC_SAMPLES
    n_part = 20; % Number of particles
    variables = {'x', 'x[1:2]', 'unknownvariable'}; % Variables to be monitored
    type = 'fs'; rs_type = 'stratified'; rs_thres = 0.5; % Optional parameters
    % Run SMC
    out_smc = biips_smc_samples(model_id, variables, n_part,...
        'type', type, 'rs_type', rs_type, 'rs_thres', rs_thres);
    nodes = biips_get_nodes(model_id)
    biips_clear(model_id)
end

% pause

%% Test PIMH
switch(test)
    case {'pimh', 'all'}
        
    % Test pimh with vectors
    t_max = 10; mean_x_init = 0;prec_x_init = 1;prec_x = 1;prec_y = 10;
    data = struct('t_max', t_max, 'prec_x_init', prec_x_init,...
        'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);
    %%% Start BiiPS console
    biips_init;
    %%% Compile BUGS model and sample data
    model = 'hmm_1d_lin.bug'; % BUGS model filename
    sample_data = true; % Boolean
    [model_id, data] = biips_model(model, data, 'sample_data', sample_data);
    
    variables = {'x', 'x[1:2]'};
    n_part = 100;
    n_iter = 20;
    biips_pimh_update(model_id, variables, n_iter, n_part);
    out_pimh = biips_pimh_samples(model_id, variables, n_iter, n_part);
    biips_clear(model_id)
    
%     % Test pimh with matrices *** CREATES A BUG ***
%     t_max = 10; mean_x_init = 0;prec_x_init = 1;prec_x = 1;prec_y = 10;
%     data = struct('t_max', t_max, 'prec_x_init', prec_x_init,...
%         'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);
%     %%% Start BiiPS console
%     biips_init;
%     %%% Compile BUGS model and sample data
%     model = 'hmm_1d_lin_mat.bug'; % BUGS model filename
%     sample_data = true; % Boolean
%     [model_id, data] = biips_model(model, data, 'sample_data', sample_data);
%     
%     variables = {'x', 'x[1:2,1]'};
%     n_part = 100;
%     n_iter = 20;
%     biips_pimh_update(model_id, variables, n_iter, n_part);
%     out_pimh = biips_pimh_samples(model_id, variables, n_iter, n_part);
%     biips_clear(model_id)

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
    %%% Start BiiPS console
    biips_init;
    %%% Compile BUGS model and sample data
    model = 'hmm_1d_nonlin_param2.bug'; % BUGS model filename
    sample_data = true; % Boolean
    [model_id, data] = biips_model(model, data, 'sample_data', sample_data);

    param_names = {'log_prec_y[1]', 'log_prec_y[2]','log_prec_y2[1:2]', 'alpha[1:1]'};
    inits = {0, 0, [0;0], 5};
    latent_names = {};
    n_burn = 500;
    n_part = 50;
    n_iter = 520;
    %%
    % *Init PMMH*
    obj_pmmh = biips_pmmh_object(model_id, param_names, 'inits', inits); % creates a pmmh object

    %%
    % *Run PMMH*
    obj_pmmh = biips_pmmh_update(obj_pmmh, n_burn, n_part); % adaptation and burn-in iterations
    [out_pmmh, log_post, log_marg_like, stats_pmmh] = biips_pmmh_samples(obj_pmmh, n_iter, n_part,...
        'thin', 1, 'latent_names', latent_names); % Samples
    biips_clear(model_id)
end