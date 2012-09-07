addpath('../matlab');
%# data
%Prec.x.init <- matrix(0,4,4)
%seq1 <- c(1,0,delta.t,0, 0,1,0,delta.t, 0,0,1,0, 0,0,0,1)
%F <- matrix(seq1,4,4,byrow = T)
%seq2 <- c(delta.t^2/2,0, 0,delta.t^2/2, delta.t,0, 0,delta.t)
%G <- matrix(seq2, 4,2,byrow = T)
%H <- matrix(0,2,4)
%H[1,1] <- 1
%H[2,2] <- 1
%mean.v <- c(0,0)
%Prec.v <- matrix(0,2,2)
%Prec.v[row(Prec.v)==col(Prec.v)] <- 1
%Prec.y <- matrix(0,2,2)
%Prec.y[row(Prec.y)==col(Prec.y)] <- 2

tmax = 10;
meanxinit = [0; 0; 1; 0];
precxinit = diag(100*ones(4,1));
delta_t = 1;
F = [ 1 0 delta_t 0; 0 1 0 delta_t; 0 0 1 0; 0 0 0 1];
G = [ delta_t.^2/2 0; 0 delta_t.^2/2 ; delta_t 0; 0 delta_t];
H = [ diag(ones(2,1)) zeros(2,2) ];
meanv = zeros(2,1);
precv = diag(1*ones(2,1));
precy = diag([2; 2]);

data=struct('tmax', tmax, 'precxinit', precxinit,...
            'precy', precy, 'meanxinit', meanxinit,...
	    'F', F, 'G', G,'H',H, 'meanv', meanv, 'precv', precv);

if (~inter_biips('load_module', 'basemod'))
    error('problem loading module'); 
end
%% intialisation console
biips_init;
inter_biips('verbosity',4);
p=biips_model('hmm_4d_lin_tracking.bug', data);
biips_smc_samples(p, {'x'}, 100);
%% on nettoie la console
inter_biips('clear_console', p); 
exit;
