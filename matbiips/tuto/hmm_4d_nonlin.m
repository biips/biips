addpath('../matlab');
nb_particules = 200;
%DATA
tmax = 10;
meanxinit = [0; 0; 1; 0];
precxinit = diag(1000*ones(4,1));
xpos = [-10  0];
meanv = zeros(2, 1);
precv = diag(1*ones(2,1));
precy = diag([10; 100]);
delta_t = 1;
F = [ 1 0 delta_t 0; 0 0 1 0; delta_t 0 0 1; 0 0 0 1];
G = [ delta_t.^2/2 0; 0 delta_t.^2/2 ; delta_t 0; 0 delta_t];


data=struct('tmax', tmax, 'precxinit', precxinit,...
            'precy', precy, 'meanxinit', meanxinit,...
	    'F', F, 'G', G, 'xpos', xpos, 'meanv', meanv, 'precv', precv);

if (~inter_biips('load_module', 'basemod'))
    error('problem loading module'); 
end
%% intialisation console

biips_init;
inter_biips('verbosity',4);
p=biips_model('hmm_4d_nonlin_tracking.bug', data);
biips_smc_samples(p, {['x[2,1:' int2str(tmax) ']'] }, 100);

%% on nettoie la console
inter_biips('clear_console', p); 
