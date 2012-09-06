addpath('../matlab');
%data
%Prec.x.init <- matrix(0,4,4)
%Prec.x.init[row(Prec.x.init)==col(Prec.x.init)] <- 1000
%delta.t <- 1
%seq1 <- c(1,0,delta.t,0, 0,1,0,delta.t, 0,0,1,0, 0,0,0,1)
%F <- matrix(seq1,4,4,byrow = T)
%seq2 <- c(delta.t^2/2,0, 0,delta.t^2/2, delta.t,0, 0,delta.t)
%G <- matrix(seq2, 4,2,byrow = T)
%x.pos <- c(-10,0)
%mean.v <- c(0,0)
%Prec.v <- matrix(0,2,2)
%Prec.v[row(Prec.v)==col(Prec.v)] <- 1
%Prec.y <- matrix(0,2,2)
%Prec.y[1,1] = 10
%Prec.y[2,2] = 100
%
%data <- list(t.max = t.max,
%             mean.x.init = mean.x.init,
%             Prec.x.init = Prec.x.init,
%             Prec.y = Prec.y,
%             F = F, G = G,
%             mean.v = mean.v,
%             Prec.v = Prec.v,
%             x.pos = x.pos)
tmax = 10;
meanxinit = [0; 0; 1; 0];
precxinit = diag(1000*ones(4,1));
xpos = [-10 ; 0];
meanv = zeros(2,1);
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
biips_smc_samples(p, {'x[1:10,]'}, 100);
%% on nettoie la console
inter_biips('clear_console', p); 
exit;
