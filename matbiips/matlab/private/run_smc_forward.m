function ok=run_smc_forward(p, nb_part, rs_thres, rs_type, seed)
% RUN_SMC_FORWARD 
% ok = run_smc_forward(p, nb_part, rs_thres, rs_type, seed)
% INPUT 
% - p : id (integer) of the console
% - nb_part : number (integer) of particules
% - rs_thres : resampling threshold ( if real in  [0,1] ---> percentage of nb_particules
%                                        integer in [1,nb_part] --> number of particules
% - rs_type : string belonging to {'stratified', 'systematic', 'residual', 'multinomial' } indicating 
%             the kind of algorithm used for esampling
% - seed : seed for random number generator
% OUTPUT
% ok : boolean 
if (~inter_biips('is_sampler_built', p))
   inter_biips('build_smc_sampler', p, 0);
end
ok=inter_biips('run_smc_sampler', p, nb_part, seed, rs_thres, rs_type);
