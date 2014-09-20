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
})

#' # Statistical model
#' The continuous-time Lotka-Volterra Markov jump process describes the
#' evolution of two species $X_{1}(t)$ (prey) and $X_{2}(t)$ (predator) at time $t$. Let $dt$ be an infinitesimal interval. The process evolves as
#'
#' $$\Pr(X_1(t+dt)=x_1(t)+1,X_2(t+dt)=x_2(t)|x_1(t),x_2(t))=c_1x_1(t)dt+o(dt)$$
#'
#' $$\Pr(X_1(t+dt)=x_1(t)-1,X_2(t+dt)=x_2(t)+1|x_1(t),x_2(t))=c_2x_1(t)x_2(t)dt+o(dt)$$
#'
#' $$\Pr(X_1(t+dt)=x_1(t),X_2(t+dt)=x_2(t)-1|x_1(t),x_2(t))=c_3 x_2(t)dt+o(dt)$$
#'
#' where $c_1=0.5$, $c_2=0.0025$ and $c_3=0.3$.
#'
#' Forward simulation can be done using the Gillespie algorithm. We additionally assume that we observe at some time $t=1,2,\ldots,t_{\max}$ the number of preys with some noise
#'
#' $$ Y(t)=X_1(t) + \epsilon(t), ~~\epsilon(t)\sim\mathcal N(0,\sigma^2) $$

#+
#' # Statistical model in BUGS language
#' Content of the file `'stoch_kinetic_gill.bug'`:
model_file = 'stoch_kinetic_gill.bug' # BUGS model filename
cat(readLines(model_file), sep = "\n")

#+
#' # User-defined R functions
lotka_volterra_gillespie <- function(x, c1, c2, c3, dt) {
  # Simulation from a Lotka-Volterra model with the Gillepsie algorithm
  # x1 is the number of prey
  # x2 is the number of predator
  # R1: (x1,x2) -> (x1+1,x2)      At rate c1x1
  # R2: (x1,x2) -> (x1-1,x2+1)    At rate c2x1x2
  # R3: (x1,x2) -> (x1,x2-1)      At rate c3xx2
  z = matrix(c(1, -1, 0,
               0, 1, -1), nrow=2, byrow=TRUE)

  t = 0
  while (TRUE) {
    rate = c(c1*x[1], c2*x[1]*x[2], c3*x[2])
    sum_rate = sum(rate);
    t = t - log(runif(1))/sum_rate # Sample next event from an exponential distribution
    ind = which((sum_rate*runif(1))<=cumsum(rate))[1] # Sample the type of event
    if (t>dt)
      break
    x = x + z[,ind]
  }
  return(c(x))
}

lotka_volterra_dim <- function(x_dim, c1_dim, c2_dim, c3_dim, dt_dim) { c(2,1) }

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

#+
#' # General settings
par(bty='l')
light_blue = rgb(.7, .7, 1)
light_red = rgb(1, .7, .7)
dark_blue = rgb(0, 0, .5)
dark_red = rgb(.5, 0, 0)

#' Set the random numbers generator seed for reproducibility
set.seed(0)

#' # Add new sampler to Biips

#+
#' #### Add the user-defined function 'LV' to simulate from the Lotka-Volterra model
fun_bugs = 'LV'; fun_nb_inputs = 5
fun_dim = lotka_volterra_dim; fun_sample = lotka_volterra_gillespie
biips_add_distribution(fun_bugs, fun_nb_inputs, fun_dim, fun_sample)

#' # Load model and data

#' #### Model parameters
t_max = 40
x_init = c(100, 100)
c = c(.5, .0025, .3)
sigma = 10
data = list(t_max=t_max, c=c, x_init=x_init, sigma=sigma)

#' #### Compile BUGS model and sample data
sample_data = TRUE # Boolean
model = biips_model(model_file, data, sample_data=sample_data) # Create Biips model and sample data
data = model$data()

#' #### Plot data
#+ fig.cap='data'
plot(1:t_max, data$x_true[1,],
     type='l', col='blue', lwd=2,
     xlab='Time',
     ylab='Number of individuals',
     ylim=c(0, 450))
lines(1:t_max, data$x_true[2,], col='red', lwd=2)
points(1:t_max, data$y, pch=8, col='green', lwd=2)
legend('topright', leg=c('Prey', 'Predator', 'Measurements'),
       col=c('blue', 'red', 'green'), lwd=2, pch=c(NA,NA,8),
       bty='n')

#' # Biips Sequential Monte Carlo algorithm

#+
#' #### Run SMC
n_part = 10#000 # Number of particles
variables = c('x') # Variables to be monitored
out_smc = biips_smc_samples(model, variables, n_part, type='fs')

#' #### Summary statistics
summ_smc = biips_summary(out_smc, probs=c(.025, .975))

#' #### Smoothing ESS
#+ fig.cap = 'SMC: SESS'
plot(out_smc$x$s$ess[1,], type='l', log='y', col='blue', lwd=2,
     xlab='Time', ylab='SESS',
     ylim=c(1,max(out_smc$x$s$ess[1,])))
lines(1:t_max, rep(30,t_max), lwd=2, lty=2)
legend('topright', leg='Smoothing effective sample size',
       col='blue', lwd=2, bty='n')

#' #### Posterior mean and quantiles for x
#+ fig.cap='SMC: Posterior mean and quantiles'
x_smc_mean = summ_smc$x$s$mean
x_smc_quant = summ_smc$x$s$quant

xx = c(1:t_max, t_max:1)
yy = c(x_smc_quant[[1]][1,], rev(x_smc_quant[[2]][1,]))
plot(xx, yy, type='n', xlab='Time', ylab='Estimates',
     ylim=c(0, 450))
polygon(xx, yy, col=light_blue, border=NA)

yy = c(x_smc_quant[[1]][2,], rev(x_smc_quant[[2]][2,]))
polygon(xx, yy, col=light_red, border=NA)

lines(1:t_max, x_smc_mean[1,], col='blue', lwd=3)
lines(1:t_max, data$x_true[1,], col='dark_blue', lwd=2, lty=2)

lines(1:t_max, x_smc_mean[2,], col='red', lwd=3)
lines(1:t_max, data$x_true[2,], col='dark_red', lwd=2, lty=2)

legend('topright', leg=c('95 % credible interval (prey)',
                         'SMC mean estimate (prey)',
                         'True number of preys',
                         '95 % credible interval (predator)',
                         'SMC mean estimate (predator)',
                         'True number of predators'),
       col=c(light_blue,'blue',dark_blue,light_red,'red',dark_red),
       lwd=c(NA,3,2,NA,3,2),
       pch=c(15,NA,NA,15,NA,NA),
       lty=c(NA,1,2,NA,1,2),
       pt.cex=c(2,1,1,2,1,1), bty='n')
