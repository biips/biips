#+ setup, include=FALSE
library(knitr)
opts_chunk$set(cache=TRUE, comment=NA, background='white', fig.show='hold',
               tidy=FALSE, pars=TRUE)
knit_hooks$set(pars = function(before, options, envir) {
  if (before) graphics::par(bty='l')
})

#' # RBiips: Tutorial 2
#' In this tutorial, we consider applying sequential Monte Carlo methods for
#' sensitivity analysis and parameter estimation in a nonlinear non-Gaussian hidden Markov model.

#' ## Statistical model
#' The statistical model is defined as follows.
#'
#' $$ x_1\sim \mathcal N\left (\mu_0, \frac{1}{\lambda_0}\right )$$
#'
#' $$ y_1\sim \mathcal N\left (h(x_1), \frac{1}{\lambda_y}\right )$$
#'
#' For $t=2:t_{max}$
#'
#' $$ x_t|x_{t-1} \sim \mathcal N\left ( f(x_{t-1},t-1), \frac{1}{\lambda_x}\right )$$
#'
#' $$ y_t|x_t \sim \mathcal N\left ( h(x_{t}), \frac{1}{\lambda_y}\right )$$
#'
#' with $\mathcal N\left (m, S\right )$ stands for the Gaussian distribution 
#' of mean $m$ and covariance matrix $S$, $h(x)=x^2/20$, $f(x,t-1)=0.5
#' x+25 x/(1+x^2)+8 \cos(1.2 (t-1))$, $\mu_0=0$, $\lambda_0 = 5$, $\lambda_x
#' = 0.1$. The precision of the observation noise
#' $\lambda_y$ is also assumed to be unknown. We will assume a uniform prior
#' for $\log(\lambda_y)$:
#'
#' $$ \log(\lambda_y) \sim Unif(c(-3,3)) $$

#' ## Statistical model in BUGS language
#' One needs to describe the model in BUGS language. We create the file
#'  'hmm_1d_nonlin_param.bug':

#+
#'
#' 
#'         var x_truec(t_max), xc(t_max), yc(t_max)
#' 
#'         data
#'         {
#'           prec_y_true <- exp(log_prec_y_true)
#'           x_truec(1) ~ dnorm(mean_x_init, prec_x_init)
#'           yc(1) ~ dnorm(x_truec(1)^2/20, prec_y_true)
#'           for (t in 2:t_max)
#'           {
#'             x_truec(t) ~ dnorm(0.5*x_truec(t-1)+25*x_truec(t-1)/(1+x_truec(t-1)^2)+8*cos(1.2*(t-1)), prec_x)
#'             yc(t) ~ dnorm(x_truec(t)^2/20, prec_y_true)
#'           }
#'         }
#' 
#'         model
#'         {
#'           log_prec_y ~ dunif(-3, 3)
#'           prec_y <- exp(log_prec_y)
#'           xc(1) ~ dnorm(mean_x_init, prec_x_init)
#'           yc(1) ~ dnorm(xc(1)^2/20, prec_y)
#'           for (t in 2:t_max)
#'           {
#'             xc(t) ~ dnorm(0.5*xc(t-1)+25*xc(t-1)/(1+xc(t-1)^2)+8*cos(1.2*(t-1)), prec_x)
#'             yc(t) ~ dnorm(xc(t)^2/20, prec_y)
#'           }
#'         }

#' ## Installation of RBiips package
#' Install *RBiips* package archive depending on your system:
#' - `RBiips_x.x.x.zip` for Windows
#' - `RBiips_x.x.x.tgz` for Mac OS X
#' - `RBiips_x.x.x.tar.gz` for Linux

#+ eval=FALSE
install.packages('path/to/RBiips_x.x.x.ext')

#' where `x.x.x` is the version of the package and `ext` is the 
#' archive extension for your system.
#'
#' **Note:** Linux installation needs a previous installation of
#' *BiiPS* libraries and headers as well as *Boost*.

#' ## Load model and data
#'

#+
#' **Model parameters**
t_max = 20
mean_x_init = 0
prec_x_init = 1
prec_x = 10
log_prec_y_true = log(1) # True value used to sample the data
data = list(t_max=t_max, prec_x_init=prec_x_init,
    prec_x=prec_x,  log_prec_y_true=log_prec_y_true, mean_x_init=mean_x_init)

#+
#' **Load RBiips package**
require(RBiips)

#+
#' **Compile BUGS model and sample data**
model = 'hmm_1d_nonlin_param$bug' # BUGS model filename
sample_data = TRUE # Boolean
model = biips_model(model, data, sample_data=sample_data) # Create biips model and sample data
data = model$data()


#' ## BiiPS : Sensitivity analysis with Sequential Monte Carlo
#' Let now use BiiPS to provide estimates of the marginal log-likelihood and 
#' log-posterior (up to a normalizing constant) given various values of the
#' log-precision parameters $\log(\lambda_y)$ .

#+
#' **Parameters of the algorithm**. 
n_part = 100 # Number of particles
param_names = c('log_prec_y') # Parameter for which we want to study sensitivity
param_values = {-5:.2:3} # Range of values

#+
#' **Run sensitivity analysis with SMC**
out = biips_smc_sensitivity(model, param_names, param_values, n_part)

#+
#' *Plot log-marginal likelihood and penalized log-marginal likelihood*

lines(param_values{1}, out$log_marg_like, '.')
title(xlab='Parameter log\_prec\_y')
title(ylab='Log-marginal likelihood')


lines(param_values{1}, out$log_marg_like_pen, '.')
title(xlab='Parameter log\_prec\_y')
title(ylab='Penalized log-marginal likelihood')


#' ## BiiPS Particle Marginal Metropolis-Hastings
#' We now use BiiPS to run a Particle Marginal Metropolis-Hastings in order
#' to obtain posterior MCMC samples of the parameter and variables x.

#+
#' **Parameters of the PMMH**
#' param_names indicates the parameters to be sampled using a random walk
#' Metroplis-Hastings step. For all the other variables, biips will use a
#' sequential Monte Carlo as proposal.
n_burn = 2000 # nb of burn-in/adaptation iterations
n_iter = 2000 # nb of iterations after burn-in
thin = 1 # thinning of MCMC outputs
n_part = 50 # nb of particles for the SMC
var_name = 'log_prec_y'
param_names = {var_name} # name of the variables updated with MCMC (others are updated with SMC)
latent_names = c('x') # name of the variables updated with SMC and that need to be monitored

#+
#' **Init PMMH**
obj_pmmh = biips_pmmh_init(model, param_names, 'inits', {-2},
    latent_names=latent_names) # creates a pmmh object

#+
#' **Run PMMH**
obj_pmmh = biips_pmmh_update(obj_pmmh, n_burn, n_part) # adaptation and burn-in iterations
c(obj_pmmh, out_pmmh, log_post, log_marg_like, stats_pmmh) = biips_pmmh_samples(obj_pmmh, n_iter, n_part,
    'thin', 1) # Samples
 
#+
#' **Some summary statistics**
summary_pmmh = summary(out_pmmh, probs=c(.025, .975))

#+
#' **Compute kernel density estimates**
kde_estimates_pmmh = density(out_pmmh)

#+
#' **Posterior mean and credibilist interval for the parameter**
sum_var = getfield(summary_pmmh, var_name)
fprintf('Posterior mean of log_prec_y: %.1f\n', sum_var$Mean)
fprintf('95%% credibilist interval for log_prec_y: c(%.1f,%.1f)\n',
    sum_var$quant(1),  sum_var$quant(2))


#+
#' **Trace of MCMC samples for the parameter**
mcmc_samples = getfield(out_pmmh,var_name)

lines(mcmc_samples)

lines(0, data$log_prec_y_true, col='green3', pch=8)
title(xlab='Iterations')
title(ylab='PMMH samples')
title(main='log\_prec\_y')

#+
#' **Histogram and kde estimate of the posterior for the parameter**

hist(mcmc_samples, 15)

lines(data$log_prec_y_true, 0, col='green3', pch=8)
title(xlab='log\_prec\_y')
title(ylab='number of samples')
title(main='log\_prec\_y')

kde_var = getfield(kde_estimates_pmmh, var_name)

lines(kde_var$x, kde_var$f)

lines(data$log_prec_y_true, 0, col='green3', pch=8)
title(xlab='log\_prec\_y')
title(ylab='posterior density')
   

#+
#' **Posterior mean and quantiles for x**
x_pmmh_mean = summary_pmmh$x$Mean
x_pmmh_inf = summary_pmmh$x$'Qu. 0.025'
x_pmmh_sup = summary_pmmh$x$'Qu. 0.925'


lightcols = adjustcolor(palette('default'), alpha.f=.3)
palette(lightcols)
plot(c(1:t_max, t_max:1), c(x_pmmh_inf, rev(x_pmmh_sup)), type='n')
polygon(c(1:t_max, t_max:1), c(x_pmmh_inf, rev(x_pmmh_sup)),
        col='blue', border=NA))
legend('topleft', leg='95 % credible interval', 
       col='blue', pch=15, pt.cex=2, bty='n')
palette('default')


lines(x_pmmh_mean, lwd=3)
title(xlab='Time')
title(ylab='Estimates')
legend('topright', leg=c('PMMH Mean Estimate'),
       col=c('green3','red'), lwd=c(3,2), bty='n')

#+
#' **Trace of MCMC samples for x**
time_index = c(5, 10, 15, 20)

par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
    tk = time_index[k]

    plot(out_pmmh$x(tk, :))

    plot(0, data$x_true[tk], col='green3', pch=8)
    title(xlab='Iterations')
    title(ylab='PMMH samples')
    title(main=paste('t=', tk, sep=''))
}
legend('topright', leg=c('PMMH samples', 'True value'),
       col=c('green3','blue','red'), pch=c(NA,NA,8), lty=c(1,1,NA), bg='white')
par(mfrow=c(1,1))

#+
#' **Histogram and kernel density estimate of posteriors of x**

par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
    tk = time_index[k]

    hist(out_pmmh$x(tk, :), 15)

    points(data$x_true[tk], 0, col='green3', pch=8)
    title(xlab=bquote(x[.(tk)]))
    title(ylab='number of samples')
    title(main=paste('t=', tk, sep=''))
}
legend('topright', leg=c('smoothing density', 'True value'),
       col=c('green3','blue','red'), pch=c(NA,NA,8), lty=c(1,1,NA), bg='white')
par(mfrow=c(1,1))


par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
    tk = time_index[k]

    plot(kde_estimates_pmmh$x[tk]$x, kde_estimates_pmmh$x[tk]$f)

    points(data$x_true[tk], 0, col='green3', pch=8)
    title(xlab=bquote(x[.(tk)]))
    title(ylab='posterior density')
    title(main=paste('t=', tk, sep=''))
}
legend('topright', leg=c('posterior density', 'True value'),
       col=c('green3','blue','red'), pch=c(NA,NA,8), lty=c(1,1,NA), bg='white')
par(mfrow=c(1,1))


#' ## Clear model
#' 

biips_clear(model)
