#' ---
#' title: "Rbiips example: Switching Stochastic volatility"
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
#' In this example, we consider the Markov switching stochastic volatility model.
#'
#' Reference: C.M. Carvalho and H.F. Lopes. Simulation-based sequential analysis of Markov switching
#' stochastic volatility models. Computational Statistics and Data analysis (2007) 4526-4542.

#+ setup, include=FALSE
knitr::opts_chunk$set(cache=TRUE, comment=NA, warning=FALSE, fig.show='hold', fig.align='center', pars=TRUE)
knitr::knit_hooks$set(pars = function(before, options, envir) {
  if (before) par(bty='l')
})

#' # Statistical model
#'
#' Let $y_t$ be the response variable and $x_t$ the unobserved
#' log-volatility of $y_t$. The stochastic volatility model is defined as follows
#' for $t\leq t_{max}$
#'
#' $$x_t|(x_{t-1},\alpha,\phi,\sigma,c_t) \sim \mathcal N (\alpha_{c_t} + \phi x_{t-1} , \sigma^2)$$
#'
#' $$ y_t|x_t \sim \mathcal N (0, \exp(x_t)) $$
#'
#' The regime variables $c_t$ follow a two-state Markov process with
#' transition probabilities
#'
#' $$p_{ij}=Pr(c_t=j|c_{t-1}=i)$$
#'
#' $\mathcal N(m,\sigma^2)$ denotes the normal
#' distribution of mean $m$ and variance $\sigma^2$.
#'
#' # Statistical model in BUGS language
#' Content of the file `'switch_stoch_volatility.bug'`:
model_file = 'switch_stoch_volatility.bug' # BUGS model filename
cat(readLines(model_file), sep = "\n")

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

#'
#' # General settings
par(bty='l')
light_blue = rgb(.7, .7, 1)
light_red = rgb(1, .7, .7)
hot_colors = colorRampPalette(c('black', 'red', 'yellow', 'white'))

#' Set the random numbers generator seed for reproducibility
set.seed(0)

#' # Load model and data

#' #### Model parameters
sigma = .4; alpha = c(-2.5, -1); phi = .5; c0 = 1; x0 = 0; t_max = 100
pi = matrix(c(.9, .1, .1, .9), nrow=2, byrow=TRUE)
data = list(t_max=t_max, sigma=sigma,
            alpha=alpha, phi=phi, pi=pi, c0=c0, x0=x0)

#' #### Parse and compile BUGS model, and sample data
model = biips_model(model_file, data, sample_data=TRUE)
data = model$data()

#' # Biips Sequential Monte Carlo

#'
#' #### Run SMC
n_part = 5000 # Number of particles
variables = c('x') # Variables to be monitored
out_smc = biips_smc_samples(model, variables, n_part)

#' #### Diagnosis of the algorithm
diag_smc = biips_diagnosis(out_smc)

#' #### Plot Smoothing ESS
#+ fig.cap = 'SMC: SESS'
plot(out_smc$x$s$ess, type='l', log='y', col='blue', lwd=2,
     xlab='Time', ylab='SESS',
     ylim=c(10, n_part))
lines(1:t_max, rep(30,t_max), lwd=2, lty=2)
legend('topright', leg='Smoothing effective sample size',
       col='blue', lwd=2, bty='n')

#' #### Plot weighted particles
#+ fig.cap = 'SMC: Particles (smoothing)'
matplot(1:t_max, out_smc$x$s$values, type='n',
        xlab='Time', ylab='Particles (smoothing)')
for (t in 1:t_max) {
  val = unique(out_smc$x$s$values[t,])
  weight = sapply(val, FUN=function(x) {
    ind = out_smc$x$s$values[t,] == x
    return(sum(out_smc$x$s$weights[t,ind]))
  })
  points(rep(t, length(val)), val, col='red', pch=20,
         cex=sapply(weight, FUN=function(x) {min(2, .02*n_part*x)}))
}

#' #### Summary statistics
summ_smc = biips_summary(out_smc, probs=c(.025, .975))

#' #### Plot Filtering estimates
#+ fig.cap='SMC: Filtering estimates'
x_f_mean = summ_smc$x$f$mean
x_f_quant = summ_smc$x$f$quant

xx = c(1:t_max, t_max:1)
yy = c(x_f_quant[[1]], rev(x_f_quant[[2]]))
plot(xx, yy, type='n', xlab='Time', ylab='Estimates')

polygon(xx, yy, col=light_blue, border=NA)
lines(1:t_max, x_f_mean, col='blue', lwd=3)
lines(1:t_max, data$x_true, col='green', lwd=2)
legend('topright', leg=c('95% credible interval', 'Filtering mean estimate', 'True value'),
       col=c(light_blue,'blue','green'), lwd=c(NA,3,2), pch=c(15,NA,NA), pt.cex=c(2,1,1),
       bty='n')

#' #### Plot Smoothing estimates
#+ fig.cap='SMC: Smoothing estimates'
x_s_mean = summ_smc$x$s$mean
x_s_quant = summ_smc$x$s$quant

xx = c(1:t_max, t_max:1)
yy = c(x_s_quant[[1]], rev(x_s_quant[[2]]))
plot(xx, yy, type='n', xlab='Time', ylab='Estimates')

polygon(xx, yy, col=light_red, border=NA)
lines(1:t_max, x_s_mean, col='red', lwd=3)
lines(1:t_max, data$x_true, col='green', lwd=2)
legend('topright', leg=c('95% credible interval', 'Smoothing mean estimate', 'True value'),
       col=c(light_red,'red','green'), lwd=c(NA,3,2), pch=c(15,NA,NA), pt.cex=c(2,1,1),
       bty='n')

#' #### Marginal filtering and smoothing densities
#+ fig.cap='SMC: Marginal posteriors'
kde_smc = biips_density(out_smc)
time_index = c(5, 10, 15)
par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
  tk = time_index[k]
  plot(kde_smc$x[[tk]], col=c('blue', 'red'), lwd=2,
       xlab=bquote(x[.(tk)]), ylab='Posterior density',
       main=paste('t=', tk, sep=''))
  points(data$x_true[tk], 0, col='green', pch=8, lwd=2)
}
plot(0, type='n', bty='n', xaxt='n', yaxt='n', xlab="", ylab="")
legend('center', leg=c('Filtering density', 'Smoothing density', 'True value'),
       col=c('blue', 'red', 'green'), pch=c(NA,NA,8), lty=c(1,1,NA), lwd=2,
       bty='n')
par(mfrow=c(1,1))

#' # Biips Particle Independent Metropolis-Hastings

#'
#' #### Parameters of the PIMH
n_burn = 2000
n_iter = 10000
thin = 1
n_part = 50

#' #### Run PIMH
obj_pimh = biips_pimh_init(model, variables)
biips_pimh_update(obj_pimh, n_burn, n_part) # Burn-in iterations
out_pimh = biips_pimh_samples(obj_pimh, n_iter, n_part, thin=thin) # Return samples

#' #### Some summary statistics
summ_pimh = biips_summary(out_pimh, probs=c(.025, .975))

#' #### Posterior mean and quantiles
#+ fig.cap='PIMH: Posterior mean and quantiles'
x_pimh_mean = summ_pimh$x$mean
x_pimh_quant = summ_pimh$x$quant

xx = c(1:t_max, t_max:1)
yy = c(x_pimh_quant[[1]], rev(x_pimh_quant[[2]]))
plot(xx, yy, type='n', xlab='Time', ylab='Estimates')

polygon(xx, yy, col=light_blue, border=NA)
lines(1:t_max, x_pimh_mean, col='blue', lwd=3)
lines(1:t_max, data$x_true, col='green', lwd=2)
legend('topright', leg=c('95% credible interval', 'PIMH mean estimate', 'True value'),
       col=c(light_blue,'blue','green'), lwd=c(NA,3,2), pch=c(15,NA,NA), pt.cex=c(2,1,1),
       bty='n')

#' #### Trace of MCMC samples
#+ fig.cap='PIMH: Trace samples'
time_index = c(5, 10, 15)
par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
  tk = time_index[k]
  plot(out_pimh$x[tk,], col='blue', type='l',
       xlab='Iteration', ylab='PIMH samples',
       main=paste('t=', tk, sep=''))
  points(0, data$x_true[tk], col='green', pch=8, lwd=2)
}
plot(0, type='n', bty='n', xaxt='n', yaxt='n', xlab="", ylab="")
legend('center', leg=c('PIMH samples', 'True value'),
       col=c('blue', 'green'), pch=c(NA,8), lwd=c(1,2), lty=c(1,NA),
       bty='n')

#' #### Histograms of posteriors
#+ fig.cap='PIMH: Histograms marginal posteriors'
par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
  tk = time_index[k]
  hist(out_pimh$x[tk,], breaks=20, col='blue', border='white',
       xlab=bquote(x[.(tk)]), ylab='Number of samples',
       main=paste('t=', tk, sep=''))
  points(data$x_true[tk], 0, col='green', pch=8, lwd=2)
}
plot(0, type='n', bty='n', xaxt='n', yaxt='n', xlab="", ylab="")
legend('center', leg=c('Posterior density', 'True value'),
       col=c('blue', 'green'), pch=c(22,8), lwd=c(NA,2), lty=NA, pt.cex=c(2,1), pt.bg=c(4,NA),
       bty='n')

#' #### Kernel density estimates of posteriors
#+ fig.cap='PIMH: KDE estimates marginal posteriors'
par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
  tk = time_index[k]
  kde_pimh = density(out_pimh$x[tk,])
  plot(kde_pimh, col='blue', lwd=2,
       xlab=bquote(x[.(tk)]), ylab='Posterior density',
       main=paste('t=', tk, sep=''))
  points(data$x_true[tk], 0, col='green', pch=8, lwd=2)
}
plot(0, type='n', bty='n', xaxt='n', yaxt='n', xlab="", ylab="")
legend('center', leg=c('Posterior density', 'True value'),
       col=c('blue', 'green'), pch=c(NA,8), lwd=2, lty=c(1,NA),
       bty='n')
par(mfrow=c(1,1))

#' # Biips Sensitivity analysis
#' We want to study sensitivity to the values of the parameter $\alpha$

#'
#' #### Parameters of the algorithm
n_part = 50 # Number of particles
range <- seq(-5,2,.2) # Range of values for one component
A = rep(range, times=length(range)) # Values of the first component
B = rep(range, each=length(range)) # Values of the second component
param_values = list('alpha' = rbind(A, B))

#' #### Run sensitivity analysis with SMC
out_sens = biips_smc_sensitivity(model, param_values, n_part)

#' #### Plot log-marginal likelihood and penalized log-marginal likelihood
#+ fig.cap='Sensitivity: Log-likelihood'
# Avoid scaling problems by thresholding the values
thres = -40
zz = matrix(pmax(thres, out_sens$log_marg_like), nrow=length(range))

require(lattice)
levelplot(zz, row.values=range, column.values=range, bg='black',
          at=seq(thres, max(zz), len=100), col.regions=hot_colors(100),
          xlim=c(-5,2), ylim=c(-5,2),
          xlab=expression(alpha[1]), ylab=expression(alpha[2]),
          interpolate=TRUE, useRaster=TRUE, bty='l')
