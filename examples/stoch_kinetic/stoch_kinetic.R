#' ---
#' title: "Rbiips example: Stochastic kinetic predator-prey model"
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
#' Reference: R.J. Boys, D.J. Wilkinson and T.B.L. Kirkwood. Bayesian
#' inference for a discretely observed stochastic kinetic model. Statistics
#' and Computing (2008) 18:125-135.

#+ setup, include=FALSE
knitr::opts_chunk$set(cache=TRUE, comment=NA, warning=FALSE, fig.show='hold', fig.align='center', pars=TRUE)
knitr::knit_hooks$set(pars = function(before, options, envir) {
  if (before) par(bty='l')
}, fig.mfrow = function(before, options, envir) {
  if (before) par(mfrow=options$fig.mfrow)
})

#' # Statistical model
#' Let $\delta_t=1/m$ where $m$ is an integer, and $T$ a multiple of $m$.
#' For $t=1,\ldots,T$
#' $$ x_t|x_{t-1}\sim \mathcal N(x_{t-1}+\alpha(x_{t-1},c)\delta_t,\beta(x_{t-1},c)\delta_t) $$
#'
#' where $$ \alpha(x,c) = \left(
#'                   \begin{array}{c}
#'                     c_1x_1-c_2x_1x_2 \\
#'                     c_2x_1x_2-c_3x_2 \\
#'                   \end{array}
#'                 \right) $$
#' and $$ \beta(x,c) = \left(
#'                   \begin{array}{cc}
#'                     c_1x_1+c_2x_1x_2 & -c_2x_1x_2 \\
#'                     -c_2x_1x_2 & c_2x_1x_2 + c_3x_2 \\
#'                   \end{array}
#'                 \right) $$
#'
#' For $t=m,2m,3m,\ldots,T$,
#' $$ y_t|x_t\sim \mathcal N(x_{1t},\sigma^2) $$
#'
#' and for $i=1,\ldots,3$
#'
#' $$ \log(c_i)\sim Unif(-7,2) $$
#'
#' $x_{t1}$ and $x_{t2}$ respectively correspond to the number of preys and predators and $y_t$ is the approximated number of preys. The model is the approximation of the Lotka-Volterra model.

#'
#' # Statistical model in BUGS language
model_file = 'stoch_kinetic_cle.bug' # BUGS model filename
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

#' **Note**: Linux installation needs a previous installation of
#' Biips libraries and headers as well as Boost.

#'
#' # General settings
par(bty='l')
light_blue = rgb(.7, .7, 1)
light_red = rgb(1, .7, .7)
dark_blue = rgb(0, 0, .5)
dark_red = rgb(.5, 0, 0)

#' Set the random numbers generator seed for reproducibility
set.seed(0)

#' # Load model and data

#' #### Model parameters
t_max = 20
dt = .2
x_init_mean = c(100, 100)
x_init_var = diag(10, 2)
c_true = c(.5, .0025, .3)
prec_y = 1/10
data = list(t_max=t_max, dt=dt, c_true=c_true,
            x_init_mean=x_init_mean, x_init_var=x_init_var, prec_y=prec_y)

#' #### Compile BUGS model and sample data
sample_data = TRUE # Boolean
model = biips_model(model_file, data, sample_data=sample_data) # Create Biips model and sample data
data = model$data()

#' #### Plot data
#+ fig.cap='Data'
t_vec <- seq(dt, t_max, dt)
plot(t_vec, data$x_true[1,],
     type='l', col='blue', lwd=2,
     xlab='Time',
     ylab='Number of individuals',
     ylim=c(50,350))
lines(t_vec, data$x_true[2,], col='red', lwd=2)
points(t_vec, data$y, pch=8, col='green', lwd=2)
legend('topright', leg=c('Prey', 'Predator', 'Measurements'),
       col=c('blue', 'red', 'green'), lwd=2, lty=c(1,1,NA), pch=c(NA,NA,8),
       bty='n')

#' # Biips Sensitivity analysis with Sequential Monte Carlo

#'
#' #### Parameters of the algorithm
n_part = 100 # Number of particles
n_grid = 20
param_values = list('logc[1]' = seq(-7, 1, len=n_grid),
                    'logc[2]' = rep(log(c_true[2]), n_grid),
                    'logc[3]' = rep(log(c_true[3]), n_grid)) # Range of values

#' #### Run sensitivity analysis with SMC
out_sens = biips_smc_sensitivity(model, param_values, n_part)

#' #### Plot penalized log-marginal likelihood
#+ fig.cap='Sensitivity: Penalized log-marginal likelihood'
plot(param_values[[1]], out_sens$log_marg_like_pen, col='blue', pch=20,
     xlab=expression(log(c[1])),
     ylab='Penalized log-marginal likelihood',
     ylim=c(-15000, 0))

#' # Biips Particle Marginal Metropolis-Hastings
#' We now use Biips to run a Particle Marginal Metropolis-Hastings in order
#' to obtain posterior MCMC samples of the parameters and variables $x$.

#'
#' #### Parameters of the PMMH
n_burn = 2000 # nb of burn-in/adaptation iterations
n_iter = 20000 # nb of iterations after burn-in
thin = 10 # thinning of MCMC outputs
n_part = 100 # nb of particles for the SMC
param_names = c('logc[1]', 'logc[2]', 'logc[3]') # names of the variables updated with MCMC (others are updated with SMC)
latent_names = c('x') # names of the variables updated with SMC and that need to be monitored

#' #### Init PMMH
obj_pmmh = biips_pmmh_init(model, param_names, inits=list(-1, -5, -1),
                           latent_names=latent_names) # creates a pmmh object

#' #### Run PMMH
biips_pmmh_update(obj_pmmh, n_burn, n_part) # adaptation and burn-in iterations
out_pmmh = biips_pmmh_samples(obj_pmmh, n_iter, n_part, thin=thin) # samples

#' #### Some summary statistics
summ_pmmh = biips_summary(out_pmmh, probs=c(.025, .975))

#' #### Compute kernel density estimates
kde_pmmh = biips_density(out_pmmh)

param_true = log(c_true)
param_lab = expression(log(c[1]), log(c[2]), log(c[3]))

#' #### Posterior mean and credible interval of the parameters
for (k in 1:length(param_names)) {
  summ_param = summ_pmmh[[param_names[k]]]
  cat('Posterior mean of ',  param_names[k], ': ', summ_param$mean, '\n', sep='');
  cat('95% credible interval of ',  param_names[k], ': [', summ_param$quant[[1]], ', ', summ_param$quant[[2]],']\n', sep='')
}

#' #### Trace of MCMC samples of the parameters
#+ fig.cap = 'PMMH: Trace samples parameter', fig.subcap = param_names, fig.mfrow=c(2,2)
for (k in 1:length(param_names)) {
  samples_param = out_pmmh[[param_names[k]]]
  plot(samples_param[1,], type='l', col='blue', lwd=1,
       xlab='Iteration', ylab=param_lab[k],
       main=param_lab[k])
  if (sample_data)
    points(0, param_true[k], col='green', pch=8, lwd=2)
}

#' #### Histogram and KDE estimate of the posterior of the parameters
#+ fig.cap = 'PMMH: Histogram posterior parameter', fig.subcap = param_names, fig.mfrow=c(2,2)
for (k in 1:length(param_names)) {
  samples_param = out_pmmh[[param_names[k]]]
  hist(samples_param, breaks=15, col='blue', border='white',
       xlab=param_lab[k], ylab='Number of samples',
       main=param_lab[k])
  if (sample_data)
    points(param_true[k], 0, col='green', pch=8, lwd=2)
}

#+ fig.cap = 'PMMH: KDE estimate posterior parameter', fig.subcap = param_names, fig.mfrow=c(2,2)
for (k in 1:length(param_names)) {
  kde_param = kde_pmmh[[param_names[k]]]
  plot(kde_param, col='blue', lwd=2,
       xlab=param_lab[k], ylab='Posterior density',
       main=param_lab[k])
  if (sample_data)
    points(param_true[k], 0, col='green', pch=8, lwd=2)
}

#' #### Posterior mean and quantiles for x
#+ fig.cap='SMC: Posterior mean and quantiles'
x_pmmh_mean = summ_pmmh$x$mean
x_pmmh_quant = summ_pmmh$x$quant

xx = c(t_vec, rev(t_vec))
yy = c(x_pmmh_quant[[1]][1,], rev(x_pmmh_quant[[2]][1,]))
plot(xx, yy, type='n', xlab='Time', ylab='Number of individuals',
     ylim=c(0, 750))
polygon(xx, yy, col=light_blue, border=NA)

yy = c(x_pmmh_quant[[1]][2,], rev(x_pmmh_quant[[2]][2,]))
polygon(xx, yy, col=light_red, border=NA)

lines(t_vec, x_pmmh_mean[1,], col='blue', lwd=3)
lines(t_vec, data$x_true[1,], col=dark_blue, lwd=2, lty=2)

lines(t_vec, x_pmmh_mean[2,], col='red', lwd=3)
lines(t_vec, data$x_true[2,], col=dark_red, lwd=2, lty=2)

legend('topright', leg=c('95% credible interval (prey)',
                         'PMMH mean estimate (prey)',
                         'True number of preys',
                         '95% credible interval (predator)',
                         'PMMH mean estimate (predator)',
                         'True number of predators'),
       col=c(light_blue,'blue',dark_blue,light_red,'red',dark_red),
       lwd=c(NA,3,2,NA,3,2),
       pch=c(15,NA,NA,15,NA,NA),
       lty=c(NA,1,2,NA,1,2),
       pt.cex=c(2,1,1,2,1,1), bty='n')
