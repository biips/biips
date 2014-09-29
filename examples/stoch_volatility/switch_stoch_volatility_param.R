#' ---
#' title: "Rbiips example: Switching stochastic volatility with estimation of static parameters"
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
#' In this example, we consider the Markov switching stochastic volatility
#' model with parameter estimation.
#'
#' Reference: C.M. Carvalho and H.F. Lopes. Simulation-based sequential analysis of Markov switching
#' stochastic volatility models. Computational Statistics and Data analysis (2007) 4526-4542.

#+ setup, include=FALSE
knitr::opts_chunk$set(cache=TRUE, comment=NA, warning=FALSE, fig.show='hold', fig.align='center', pars=TRUE)
knitr::knit_hooks$set(pars = function(before, options, envir) {
  if (before) par(bty='l')
}, fig.mfrow = function(before, options, envir) {
  if (before) par(mfrow=options$fig.mfrow)
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
#' We assume the following
#' priors over the parameters $\alpha $, $\phi $, $\sigma $ and $p$:
#'
#' $$ \alpha_1=\gamma_1$$
#'
#' $$\alpha_2 = \gamma_1+\gamma_2$$
#'
#' $$\gamma_1 \sim \mathcal N(0,100)$$
#'
#' $$\gamma_2 \sim \mathcal {TN}_{(0,\infty)}(0,100)$$
#'
#' $$\phi \sim \mathcal {TN}_{(-1,1)}(0,100) $$
#'
#' $$\sigma^2 \sim invGamma(2.001,1) $$
#'
#' $$\pi_{11} \sim Beta(10,0.5)$$
#'
#' $$\pi_{22} \sim Beta(10,0.5)$$
#'
#' $\mathcal N(m,\sigma^2)$ denotes the normal
#' distribution of mean $m$ and variance $\sigma^2$.
#' $\mathcal {TN}_{(a,b)}(m,\sigma^2)$ denotes the truncated normal
#' distribution of mean $m$ and variance $\sigma^2$.

#'
#' # Statistical model in BUGS language
#' Content of the file `'switch_stoch_volatility_param.bug'`:
model_file = 'switch_stoch_volatility_param.bug' # BUGS model filename
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
light_green = rgb(.7, 1, .7)
light_gray = rgb(.9, .9, .9)

#' Set the random numbers generator seed for reproducibility
set.seed(0)

#' # Load model and load or simulate data
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
}

#' #### Model parameters
if (!sample_data) {
  data = list(t_max=t_max, y=y)
} else {
  sigma_true = .4; alpha_true = c(-2.5, -1); phi_true = .5
  pi11 = .9; pi22 = .9
  pi_true = matrix(c(pi11, 1-pi11, 1-pi22, pi22), nrow = 2, byrow=TRUE)
  data = list(t_max=t_max, sigma_true=sigma_true,
              alpha_true=alpha_true, phi_true=phi_true, pi_true=pi_true)
}

#' #### Compile BUGS model and sample data if simulated data
model = biips_model(model_file, data, sample_data=sample_data)
data = model$data()

#' Plot the data
#+ fig.cap='Log-returns'
if (sample_data) {
  plot(1:t_max, data$y, type='l', col='blue', lwd=2,
       main='Observed data',
       xlab='Time', ylab='Log-return', xaxt = 'n')
} else {
  plot(SP500_date_num, data$y, type='l', col='blue', lwd=2,
       main='Observed data: S&P 500',
       xlab='Date', ylab='Log-return', xaxt = 'n')
  axis.Date(1, SP500_date_num, format="%Y-%m-%d")
}

#' # Biips Particle Marginal Metropolis-Hastings
#' We now use Biips to run a Particle Marginal Metropolis-Hastings in order
#' to obtain posterior MCMC samples of the parameters $\alpha$, $\phi$,
#' $\sigma$, $\pi$, and of the variables $x$.
#' Note: We use below a reduced number of MCMC iterations to have reasonable
#' running times. But the obtained samples are obviously very correlated,
#' and the number of iterations should be set to a higher value, and proper
#' convergence tests should be used.

#'
#' #### Parameters of the PMMH
n_burn = 2000 # nb of burn-in/adaptation iterations
n_iter = 40000 # nb of iterations after burn-in
thin = 10 # thinning of MCMC outputs
n_part = 50 # nb of particles for the SMC
param_names = c('gamma[1]', 'gamma[2]', 'phi', 'tau', 'pi[1,1]', 'pi[2,2]') # names of the variables updated with MCMC (others are updated with SMC)
latent_names = c('x', 'c', 'alpha[1]', 'alpha[2]', 'sigma') # names of the variables updated with SMC and that need to be monitored

#' #### Init PMMH
inits = list(-1, 1, .5, 10, .8, .8)
obj_pmmh = biips_pmmh_init(model, param_names, inits=inits,
                           latent_names=latent_names) # creates a pmmh object

#' #### Run PMMH
biips_pmmh_update(obj_pmmh, n_burn, n_part) # adaptation and burn-in iterations
out_pmmh = biips_pmmh_samples(obj_pmmh, n_iter, n_part, thin=thin) # samples

#' #### Some summary statistics
summ_pmmh = biips_summary(out_pmmh, probs=c(.025, .975))

#' #### Compute kernel density estimates
kde_pmmh = biips_density(out_pmmh)

#' #### Compute probability mass function estimates of the discrete marginal variables c[t]
table_c = biips_table(out_pmmh$c)

param_plot = c('alpha[1]', 'alpha[2]', 'phi', 'sigma', 'pi[1,1]', 'pi[2,2]')
param_lab = expression(alpha[1], alpha[2], phi, sigma, pi[11], pi[22])
if (sample_data)
  param_true = c(alpha_true, phi_true, sigma_true, pi11, pi22)

#' #### Posterior mean and credible interval of the parameters
for (k in 1:length(param_plot)) {
  summ_param = summ_pmmh[[param_plot[k]]]
  cat('Posterior mean of ',  param_plot[k], ': ', summ_param$mean, '\n', sep='');
  cat('95% credible interval of ',  param_plot[k], ': [', summ_param$quant[[1]], ', ', summ_param$quant[[2]],']\n', sep='')
}

#' #### Trace of MCMC samples of the parameters
#+ fig.cap = 'PMMH: Trace samples parameter', fig.subcap = param_plot, fig.mfrow=c(2,3)
for (k in 1:length(param_plot)) {
  samples_param = out_pmmh[[param_plot[k]]]
  plot(samples_param[1,], type='l', col='blue', lwd=1,
       xlab='Iteration', ylab=param_lab[k],
       main=param_lab[k])
  if (sample_data)
    points(0, param_true[k], col='green', pch=8, lwd=2)
}

#' #### Histogram and KDE estimate of the posterior of the parameters
#+ fig.cap = 'PMMH: Histogram posterior parameter', fig.subcap = param_plot, fig.mfrow=c(2,3)
for (k in 1:length(param_plot)) {
  samples_param = out_pmmh[[param_plot[k]]]
  hist(samples_param, breaks=15, col='blue', border='white',
       xlab=param_lab[k], ylab='Number of samples',
       main=param_lab[k])
  if (sample_data)
    points(param_true[k], 0, col='green', pch=8, lwd=2)
}

#+ fig.cap = 'PMMH: KDE estimate posterior parameter', fig.subcap = param_plot, fig.mfrow=c(2,3)
for (k in 1:length(param_plot)) {
  kde_param = kde_pmmh[[param_plot[k]]]
  plot(kde_param, col='blue', lwd=2,
       xlab=param_lab[k], ylab='Posterior density',
       main=param_lab[k])
  if (sample_data)
    points(param_true[k], 0, col='green', pch=8, lwd=2)
}

#' #### Posterior probability of c[t]=2
#+ fig.cap = 'PMMH: Posterior probabilities of c[t]=2'
prob_c2 = rep(0, t_max)
plot(1:t_max, rep(1, t_max), type='n', ylim=c(0,1),
     xlab='Time', ylab='Posterior probability')
for (t in 1:t_max) {
  if (data$c_true[t]==2)
    polygon(c(t-1,t,t,t-1), c(0,0,1,1), col=light_green, border=NA)
  ind = which(names(table_c[[t]]) == 2)
  if (length(ind)==0)
    prob_c2[t] = 1-sum(table_c[[t]])
  else
    prob_c2[t] = table_c[[t]][ind]
}
lines(1:t_max, prob_c2, col='red', lwd=2)
legend('topleft', leg=expression(paste('True ', c[t]==2, ' intervals'), paste('PMMH estimate of ', Pr(c[t]==2))),
       col=c(light_green, 'red'), lwd=c(NA,3), pch=c(15,NA), pt.cex=c(2,1),
       box.col=light_gray, inset=.01)

#' #### Posterior mean and quantiles for x
#+ fig.cap = 'PMMH: Posterior mean and quantiles'
x_pmmh_mean = summ_pmmh$x$mean
x_pmmh_quant = summ_pmmh$x$quant

xx = c(1:t_max, t_max:1)
yy = c(x_pmmh_quant[[1]], rev(x_pmmh_quant[[2]]))
plot(xx, yy, type='n', xlab='Time', ylab='Log-volatility')

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
       xlab='Iteration',
       ylab=bquote(x[.(tk)]),
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
