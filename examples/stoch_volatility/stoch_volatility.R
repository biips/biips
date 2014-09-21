#' ---
#' title: "Rbiips example: Stochastic volatility"
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
#' In this example, we consider the stochastic volatility model SV0 for
#' application e.g. in finance.
#'
#' Reference: S. Chib, F. Nardari, N. Shepard. Markov chain Monte Carlo methods
#' for stochastic volatility models. Journal of econometrics, vol. 108, pp. 281-316, 2002.

#+ setup, include=FALSE
knitr::opts_chunk$set(cache=TRUE, comment=NA, warning=FALSE, fig.show='hold', fig.align='center', pars=TRUE)
knitr::knit_hooks$set(pars = function(before, options, envir) {
  if (before) par(bty='l')
}, fig.mfrow = function(before, options, envir) {
  if (before) par(mfrow=options$fig.mfrow)
})

#' # Statistical model
#'
#' The stochastic volatility model is defined as follows
#'
#' $$ \alpha\sim \mathcal N (0, .0001),~~~$$
#' $$ logit(\beta) \sim  \mathcal N (0, 10),~~~$$
#' $$ log(\sigma) \sim  \mathcal N (0, 1)$$
#'
#' and for $t\leq t_{max}$
#'
#' $$x_t|(x_{t-1},\alpha,\beta,\sigma) \sim \mathcal N (\alpha + \beta(x_{t-1}
#' -\alpha), \sigma^2)$$
#'
#' $$ y_t|x_t \sim \mathcal N (0, exp(x_t)) $$
#'
#' where $y_t$ is the response variable and $x_t$ is the unobserved
#' log-volatility of $y_t$. $\mathcal N(m,\sigma^2)$ denotes the normal
#' distribution of mean $m$ and variance $\sigma^2$.
#'
#' $\alpha$, $\beta$ and $\sigma$ are unknown
#' parameters that need to be estimated.

#'
#' # Statistical model in BUGS language
#' Content of the file `'stoch_volatility.bug'`:
model_file = 'stoch_volatility.bug' # BUGS model filename
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

#+ eval=FALS
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

#' Set the random numbers generator seed for reproducibility
set.seed(0)

#' # Load model and load or simulate data
#+ fig.cap='Log-returns'
sample_data = TRUE # Simulated data or SP500 data
t_max = 100

if (!sample_data) {
  # Load the data
  tab = read.csv('SP500.csv', sep = ";")
  y = diff(log(rev(tab$Close)))
  SP500_date_str = rev(tab$Date[-1])

  ind = 1:t_max
  y = y[ind]
  SP500_date_str = SP500_date_str[ind]

  SP500_date_num = as.Date(SP500_date_str)

  # Plot the SP500 data
  plot(SP500_date_num, y, type='l', col='blue', lwd=2,
       main='Observed data: S&P 500',
       xlab='Date', ylab='Log-returns', xaxt = 'n')
  axis.Date(1, SP500_date_num, format="%Y-%m-%d")
}

#' #### Model parameters
if (!sample_data) {
  data = list(t_max=t_max, y=y)
} else {
  sigma_true = .4; alpha_true = 0; beta_true=.99;
  data = list(t_max=t_max, sigma_true=sigma_true,
              alpha_true=alpha_true, beta_true=beta_true)
}

#' #### Compile BUGS model and sample data if simulated data
model = biips_model(model_file, data, sample_data=sample_data)
data = model$data()

#' # Biips Particle Marginal Metropolis-Hastings
#' We now use Biips to run a Particle Marginal Metropolis-Hastings in order
#' to obtain posterior MCMC samples of the parameters $\alpha$, $\beta$ and $\sigma$,
#' and of the variables $x$.

#'
#' #### Parameters of the PMMH
n_burn = 5000 # nb of burn-in/adaptation iterations
n_iter = 10000 # nb of iterations after burn-in
thin = 5 # thinning of MCMC outputs
n_part = 50 # nb of particles for the SMC
param_names = c('alpha', 'logit_beta', 'log_sigma') # names of the variables updated with MCMC (others are updated with SMC)
latent_names = c('x') # names of the variables updated with SMC and that need to be monitored

#' #### Init PMMH
inits = list(0, 5, -2)
obj_pmmh = biips_pmmh_init(model, param_names, inits=inits,
                           latent_names=latent_names) # creates a pmmh object

#' #### Run PMMH
biips_pmmh_update(obj_pmmh, n_burn, n_part) # adaptation and burn-in iterations
out_pmmh = biips_pmmh_samples(obj_pmmh, n_iter, n_part, thin=thin) # samples

#' #### Some summary statistics
summ_pmmh = biips_summary(out_pmmh, probs=c(.025, .975))

#' #### Compute kernel density estimates
kde_pmmh = biips_density(out_pmmh)

#' #### Posterior mean and credible interval of the parameters
for (k in 1:length(param_names)) {
  summ_param = summ_pmmh[[param_names[k]]]
  cat('Posterior mean of ',  param_names[k], ': ', summ_param$mean, '\n', sep='');
  cat('95% credible interval of ',  param_names[k], ': [', summ_param$quant[[1]], ', ', summ_param$quant[[2]],']\n', sep='')
}

#' #### Trace of MCMC samples of the parameters
#+ fig.cap = 'PMMH: Trace samples parameter', fig.subcap = param_names, fig.mfrow=c(1,3)
if (sample_data)
  param_true = c(alpha_true, log(data$beta_true/(1-data$beta_true)), log(sigma_true))
param_lab = expression(alpha, logit(beta), log(sigma))

for (k in 1:length(param_names)) {
  samples_param = out_pmmh[[param_names[k]]]
  plot(samples_param[1,], type='l', col='blue', lwd=1,
       xlab='Iterations', ylab='PMMH samples',
       main=param_lab[k])
  if (sample_data)
    points(0, param_true[k], col='green', pch=8, lwd=2)
}

#' #### Histogram and KDE estimate of the posterior of the parameters
#+ fig.cap = 'PMMH: Histogram posterior parameter', fig.subcap = param_names, fig.mfrow=c(1,3)
for (k in 1:length(param_names)) {
  samples_param = out_pmmh[[param_names[k]]]
  hist(samples_param, breaks=15, col='blue', border='white',
       xlab=param_lab[k], ylab='Number of samples',
       main=param_lab[k])
  if (sample_data)
    points(param_true[k], 0, col='green', pch=8, lwd=2)
}

#+ fig.cap = 'PMMH: KDE estimate posterior parameter', fig.subcap = param_names, fig.mfrow=c(1,3)
for (k in 1:length(param_names)) {
  kde_param = kde_pmmh[[param_names[k]]]
  plot(kde_param, col='blue', lwd=2,
       xlab=param_lab[k], ylab='Posterior density',
       main=param_lab[k])
  if (sample_data)
    points(param_true[k], 0, col='green', pch=8, lwd=2)
}

#' #### Posterior mean and quantiles for x
#+ fig.cap = 'PMMH: Posterior mean and quantiles'
x_pmmh_mean = summ_pmmh$x$mean
x_pmmh_quant = summ_pmmh$x$quant

xx = c(1:t_max, t_max:1)
yy = c(x_pmmh_quant[[1]], rev(x_pmmh_quant[[2]]))
plot(xx, yy, type='n', xlab='Time', ylab='Estimates')

polygon(xx, yy, col=light_blue, border=NA)
lines(1:t_max, x_pmmh_mean, col='blue', lwd=3)
if (sample_data) {
  lines(1:t_max, data$x_true, col='green', lwd=2)
  legend('topright', leg=c('95% credible interval', 'PMMH mean estimate', 'True value'),
         col=c(light_blue,'blue','green'), lwd=c(NA,3,2), pch=c(15,NA,NA), pt.cex=c(2,1,1),
         bty='n')
} else
  legend('topright', leg=c('95% credible interval', 'PMMH mean estimate'),
         col=c(light_blue,'blue'), lwd=c(NA,3), pch=c(15,NA), pt.cex=c(2,1),
         bty='n')

#' #### Trace of MCMC samples for x
#+ fig.cap = 'PMMH: Trace samples x'
time_index = c(5, 10, 15)
par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
  tk = time_index[k]
  plot(c(out_pmmh$x[tk,]), col='blue', type='l',
       xlab='Iterations',
       ylab='PMMH samples',
       main=paste('t=', tk, sep=''))
  if (sample_data)
    points(0, data$x_true[tk], col='green', pch=8, lwd=2)
}
if (sample_data) {
  plot(0, type='n', bty='n', xaxt='n', yaxt='n', xlab="", ylab="")
  legend('center', leg=c('PMMH samples', 'True value'),
         col=c('blue', 'green'), pch=c(NA,8), lty=c(1,NA), lwd=c(1,2),
         bty='n')
}

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
  if (sample_data)
    points(data$x_true[tk], 0, col='green', pch=8, lwd=2)
}
if (sample_data) {
  plot(0, type='n', bty='n', xaxt='n', yaxt='n', xlab="", ylab="")
  legend('center', leg=c('Posterior samples', 'True value'),
         col=c('blue', 'green'), pch=c(22,8), lwd=c(1,2), lty=NA, pt.cex=c(2,1), pt.bg=c(4,NA),
         bty='n')

}
par(mfrow=c(1,1))

#+ fig.cap = 'PMMH: KDE estimates marginal posteriors'
par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
  tk = time_index[k]
  plot(kde_pmmh$x[[tk]], col='blue', lwd=2,
       xlab=bquote(x[.(tk)]),
       ylab='Posterior density',
       main=paste('t=', tk, sep=''))
  if (sample_data)
    points(data$x_true[tk], 0, col='green', pch=8, lwd=2)
}
if (sample_data) {
  plot(0, type='n', bty='n', xaxt='n', yaxt='n', xlab="", ylab="")
  legend('center', leg=c('Posterior density', 'True value'),
         col=c('blue', 'green'), pch=c(NA,8), lwd=2, lty=c(1,NA), pt.cex=c(2,1), pt.bg=c(4,NA),
         bty='n')
}
par(mfrow=c(1,1))
