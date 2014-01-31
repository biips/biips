function biips_init_pimh(console, variables, nb_part, varargin)

%% FRANCOIS: FUNCTION NON TERMINEE - pb a verifier avec is_monitored

%% Default values
rs_type = 'stratified';
rs_thres = 0.5;
if (isoctave)
   s = rand('state');
   rand('state',sum(100*clock)); 
   seed = double(randi(intmax));
   rand('state',s);
else
   s = rng('shuffle');
   seed = randi(intmax);
   rng(s);
end
parsevar; % Process options

%%
monitored = is_monitored(console, variables, 's')
if ~monitored
    % monitor variables
    monitor_biips(console, variables, 's'); 
    is_monitored(console, variables, 's')
end

if (~inter_biips('is_sampler_built', console))
   inter_biips('build_smc_sampler', console, false);
end

atend = inter_biips('is_smc_sampler_at_end', console);
% Get the normalizing constant
if (~monitored || ~atend)
    % Run SMC sampler
    inter_biips('message', 'Initializing PIMH');
    run_smc_forward(console, nb_part, rs_thres, rs_type, seed);
end
log_marg_like = inter_biips('get_log_norm_const', console);

% Get sampled value
sampled_value = inter_biips('get_sampled_gen_tree_smooth_particle', console);
if (isempty(sampled_value))
    % Sample one particle
    inter_biips('sample_gen_tree_smooth_particle', console, seed);
    sampled_value = inter_biips('get_sampled_gen_tree_smooth_particle', console)
end


    
