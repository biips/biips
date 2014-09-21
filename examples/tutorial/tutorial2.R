#' ---
#' title: "Rbiips: Tutorial 2"
#' author: NULL
#' date: NULL
#' output:
#'   html_document:
#'     toc: TRUE
#'     fig_caption: TRUE
#'     highlight: tango
#'     theme: cerulean
#'   pdf_document:
#'     toc: TRUE
#'     fig_caption: TRUE
#'     highlight: tango
#' ---
#' In this tutorial, we consider applying sequential Monte Carlo methods for
#' sensitivity analysis and parameter estimation in a nonlinear non-Gaussian hidden Markov model.

#+ setup, include=FALSE
knitr::opts_chunk$set(cache=TRUE, comment=NA, warning=FALSE, fig.show='hold', fig.align='center', pars=TRUE)
knitr::knit_hooks$set(pars = function(before, options, envir) {
  if (before) par(bty='l')
})

#' # Statistical model
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
#' where $\mathcal N\left (m, S\right )$ denotes the Gaussian distribution
#' of mean $m$ and covariance matrix $S$, $h(x)=x^2/20$, $f(x,t-1)=0.5
#' x+25 x/(1+x^2)+8 \cos(1.2 (t-1))$, $\mu_0=0$, $\lambda_0 = 5$, $\lambda_x
#' = 0.1$.  The precision of the observation noise
#' $\lambda_y$ is also assumed to be unknown. We will assume a uniform prior
#' for $\log(\lambda_y)$:
#'
#' $$ \log(\lambda_y) \sim Unif(-3,3) $$

#+
#' # Statistical model in BUGS language
#' We describe the model in BUGS language in the file `'hmm_1d_nonlin_param.bug'`:
model_file = 'hmm_1d_nonlin_param.bug' # BUGS model filename
cat(readLines(model_file), sep = "\n")

#+
#' # Installation of Rbiips package
#' 1. [Download](https://alea.bordeaux.inria.fr/biips/doku.php?id=download) the
#'     latest version of Rbiips package depending on your system:
#'
#'     - `Rbiips_x.x.x.zip` for Windows
#'     - `Rbiips_x.x.x.tgz` for Mac OS X
#'     - `Rbiips_x.x.x.tar.gz` for Linux
#'
#'     where `x.x.x` is the version of the package and `ext` is the
#'     archive extension for your system.
#' 2. Install the package from R

#+ eval=FALSE
install.packages('path/to/Rbiips_x.x.x.ext', repos=NULL)

#' 3. Load Rbiips package
require(Rbiips)

#' **Note:** Linux installation needs a previous installation of
#' Biips libraries and headers as well as Boost.

#+
#' # General settings
par(bty='l')
light_blue = rgb(.7, .7, 1)
light_red = rgb(1, .7, .7)

#' Set the random numbers generator seed for reproducibility
set.seed(2)

#' # Load model and data

#' #### Model parameters
t_max = 20
mean_x_init = 0
prec_x_init = 1/5
prec_x = 1/10
log_prec_y_true = log(1) # True value used to sample the data
data = list(t_max=t_max, prec_x_init=prec_x_init,
            prec_x=prec_x, log_prec_y_true=log_prec_y_true,
            mean_x_init=mean_x_init)

#' #### Compile BUGS model and sample data
sample_data = TRUE # Boolean
model = biips_model(model_file, data, sample_data=sample_data) # Create Biips model and sample data
data = model$data()

#' # Biips Sensitivity analysis with Sequential Monte Carlo
#' Let now use Biips to provide estimates of the marginal log-likelihood and
#' penalized marginal log-likelihood given various values of the
#' log-precision parameters $\log(\lambda_y)$.

#+
#' #### Parameters of the algorithm
n_part = 100 # Number of particles
# Range of values of the parameter for which we want to study sensitivity
param_values = list(log_prec_y = seq(-5,3,.2))

#' #### Run sensitivity analysis with SMC
out_sens = biips_smc_sensitivity(model, param_values, n_part)

#' #### Plot log-marginal likelihood and penalized log-marginal likelihood
#+ fig.cap='Log-marginal likelihood'
plot(param_values[[1]], out_sens$log_marg_like, col='blue', pch=20,
     xlab ='Parameter log_prec_y',
     ylab = 'Log-marginal likelihood',
     ylim=c(-110, max(out_sens$log_marg_like)))

#+ fig.cap='Penalized log-marginal likelihood'
plot(param_values[[1]], out_sens$log_marg_like_pen, col='blue', pch=20,
     xlab ='Parameter log_prec_y',
     ylab = 'Penalized log-marginal likelihood',
     ylim=c(-110, max(out_sens$log_marg_like_pen)))

#' # Biips Particle Marginal Metropolis-Hastings
#' We now use Biips to run a Particle Marginal Metropolis-Hastings in order
#' to obtain posterior MCMC samples of the parameter and the variables $x$.

#+
#' #### Parameters of the PMMH
#' `param_names` indicates the parameters to be sampled using a random walk
#' Metroplis-Hastings step. For all the other variables, Biips will use a
#' sequential Monte Carlo as proposal.
n_burn = 2000 # nb of burn-in/adaptation iterations
n_iter = 2000 # nb of iterations after burn-in
thin = 1 # thinning of MCMC outputs
n_part = 50 # nb of particles for the SMC
var_name = 'log_prec_y'
param_names = c(var_name) # name of the variables updated with MCMC (others are updated with SMC)
latent_names = c('x') # name of the variables updated with SMC and that need to be monitored

#' #### Init PMMH
obj_pmmh = biips_pmmh_init(model, param_names, inits=list(log_prec_y=-2),
                     latent_names=latent_names) # creates a pmmh object

#' #### Run PMMH
biips_pmmh_update(obj_pmmh, n_burn, n_part) # adaptation and burn-in iterations
out_pmmh = biips_pmmh_samples(obj_pmmh, n_iter, n_part, thin=thin) # samples

#' #### Some summary statistics
summ_pmmh = biips_summary(out_pmmh, probs=c(.025, .975))

#' #### Compute kernel density estimates
kde_pmmh = biips_density(out_pmmh)

#' #### Posterior mean and credible interval of the parameter
summ_param = summ_pmmh[[var_name]]
cat('Posterior mean of ', var_name, ':', summ_param$mean, '\n');
cat('95% credible interval of ', var_name, ': [', summ_param$quant[[1]], ', ', summ_param$quant[[2]],']\n', sep='')

#' #### Trace of MCMC samples of the parameter
#+ fig.cap = 'PMMH: Trace samples parameter'
samples_param = out_pmmh[[var_name]]
plot(samples_param[1,], col='blue', type='l', lwd=1,
     xlab='Iterations',
     ylab='PMMH samples',
     main=var_name)
points(0, log_prec_y_true, col='green', pch=8, lwd=2)
legend('topright', leg=c('PMMH samples', 'True value'),
       col=c('blue', 'green'), pch=c(NA,8), lwd=c(1,2), lty=c(1,NA),
       bty='n')

#' #### Histogram and KDE estimate of the posterior for the parameter
#+ fig.cap = 'PMMH: Histogram posterior parameter'
hist(samples_param, breaks=15, col='blue', border='white',
     xlab=var_name, ylab='Number of samples',
     main=var_name)
points(log_prec_y_true, 0, col='green', pch=8, lwd=2)
legend('topright', leg=c('Posterior samples', 'True value'),
       col=c('blue', 'green'), pch=c(22,8), lwd=c(NA,2), lty=NA, pt.cex=c(2,1), pt.bg=c(4,NA),
       bty='n')

#+ fig.cap = 'PMMH: KDE estimate posterior parameter'
kde_param = kde_pmmh[[var_name]]
plot(kde_param, col='blue', lwd=2,
     xlab=var_name, ylab='Posterior density',
     main=var_name)
points(log_prec_y_true, 0, col='green', pch=8, lwd=2)
legend('topright', leg=c('Posterior density', 'True value'),
       col=c('blue', 'green'), pch=c(NA,8), lty=c(1,NA), lwd=2,
       bty='n')

#' #### Posterior mean and quantiles for x
#+ fig.cap = 'PMMH: Posterior mean and quantiles'
x_pmmh_mean = summ_pmmh$x$mean
x_pmmh_quant = summ_pmmh$x$quant

xx = c(1:t_max, t_max:1)
yy = c(x_pmmh_quant[[1]], rev(x_pmmh_quant[[2]]))
plot(xx, yy, type='n', xlab='Time', ylab='Estimates')

polygon(xx, yy, col=light_blue, border=NA)
lines(1:t_max, x_pmmh_mean, col='blue', lwd=3)
lines(1:t_max, data$x_true, col='green', lwd=2)
legend('topright', leg=c('95% credible interval', 'PMMH mean estimate', 'True value'),
       col=c(light_blue,'blue','green'), lwd=c(NA,3,2), pch=c(15,NA,NA), pt.cex=c(2,1,1),
       bty='n')

#' #### Trace of MCMC samples for x
#+ fig.cap = 'PMMH: Trace samples x'
time_index = c(5, 10, 15)
par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
  tk = time_index[k]
  plot(c(out_pmmh$x[tk,]), col='blue', type='l', lwd=1,
       xlab='Iterations',
       ylab='PMMH samples',
       main=paste('t=', tk, sep=''))
  points(0, data$x_true[tk], col='green', pch=8, lwd=2)
}
plot(0, type='n', bty='n', xaxt='n', yaxt='n', xlab="", ylab="")
legend('center', leg=c('PMMH samples', 'True value'),
       col=c('blue', 'green'), pch=c(NA,8), lty=c(1,NA), lwd=c(1,2),
       bty='n')
par(mfrow=c(1,1))

#' #### Histogram and kernel density estimate of posteriors of x
#+ fig.cap = 'PMMH: Histograms marginal posteriors'
par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
  tk = time_index[k]
  hist(out_pmmh$x[tk,], breaks=30, col='blue', border='white',
       xlab=bquote(x[.(tk)]),
       ylab='Number of samples',
       main=paste('t=', tk, sep=''))
  points(data$x_true[tk], 0, col='green', pch=8, lwd=2)
}
plot(0, type='n', bty='n', xaxt='n', yaxt='n', xlab="", ylab="")
legend('center', leg=c('Posterior samples', 'True value'),
       col=c('blue', 'green'), pch=c(22,8), lwd=c(1,2), lty=NA, pt.cex=c(2,1), pt.bg=c(4,NA),
       bty='n')
par(mfrow=c(1,1))

#+ fig.cap = 'PMMH: KDE estimates marginal posteriors'
par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
  tk = time_index[k]
  plot(kde_pmmh$x[[tk]], col='blue', lwd=2,
       xlab=bquote(x[.(tk)]),
       ylab='Posterior density',
       main=paste('t=', tk, sep=''))
  points(data$x_true[tk], 0, col='green', pch=8, lwd=2)
}
plot(0, type='n', bty='n', xaxt='n', yaxt='n', xlab="", ylab="")
legend('center', leg=c('Posterior density', 'True value'),
       col=c('blue', 'green'), pch=c(NA,8), lwd=2, lty=c(1,NA), pt.cex=c(2,1), pt.bg=c(4,NA),
       bty='n')
par(mfrow=c(1,1))
