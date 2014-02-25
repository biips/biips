#+ setup, include=FALSE
library(knitr)
opts_chunk$set(cache=TRUE, comment=NA, background='white', fig.show='hold')

#' # RBiips: Bayesian inference in nonlinear non-Gaussian hidden Markov model
#' In this tutorial, we consider applying sequential Monte Carlo methods for
#' Bayesian inference in a nonlinear non-Gaussian hidden Markov model.

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
#' of mean $m$ and covariance matrix $S$, $h(x)=x^2/20$, $f(x,t-1)=0.5\cdot x+25 x/(1+x^2)+8 \cos(1.2\cdot(t-1))$, $\mu_0=0$, $\lambda_0 = 5$, $\lambda_x = 0.1$ and $\lambda_y=1$. 

#' ## Statistical model in BUGS language
#' One needs to describe the model in BUGS language. We create the file
#'  'hmm_1d_nonlin.bug':

#+
#'
#' 
#'     var x_true[t_max], x[t_max], y[t_max]
#' 
#'     data
#'     {
#'       x_true[1] ~ dnorm(mean_x_init, prec_x_init)
#'       y[1] ~ dnorm(x_true[1]^2/20, prec_y)
#'       for (t in 2:t_max)
#'       {
#'         x_true[t] ~ dnorm(0.5*x_true[t-1]+25*x_true[t-1]/(1+x_true[t-1]^2)+8*cos(1.2*(t-1)), prec_x)
#'         y[t] ~ dnorm(x_true[t]^2/20, prec_y)
#'       }
#'     }
#' 
#'     model
#'     {
#'       x[1] ~ dnorm(mean_x_init, prec_x_init)
#'       y[1] ~ dnorm(x[1]^2/20, prec_y)
#'       for (t in 2:t_max)
#'       {
#'         x[t] ~ dnorm(0.5*x[t-1]+25*x[t-1]/(1+x[t-1]^2)+8*cos(1.2*(t-1)), prec_x)
#'         y[t] ~ dnorm(x[t]^2/20, prec_y)
#'       }
#'     }

#' ## Installation of RBiips package
#' 


#' ## Load model and data
#'

#+
#' **Model parameters**
t_max = 20
mean_x_init = 0
prec_x_init = 1/5
prec_x = 1/10
prec_y = 1
data = list(t_max=t_max, prec_x_init=prec_x_init,
            prec_x=prec_x, prec_y=prec_y, mean_x_init=mean_x_init)

#+
#' **Load RBiips package**
require(RBiips)

#+
#' **Compile BUGS model and sample data**
model_file = 'hmm_1d_nonlin.bug' # BUGS model filename
sample_data = TRUE # Boolean
model = biips.model(model_file, data, sample.data=sample_data) # Create biips model and sample data
data = model$data()

#' ## BiiPS Sequential Monte Carlo
#' Let now use BiiPS to run a particle filter. 

#+
#' **Parameters of the algorithm**. We want to monitor the variable x, and to
#' get the filtering and smoothing particle approximations. The algorithm
#' will use 10000 particles, stratified resampling, with a threshold of 0.5.
n_part = 10000 # Number of particles
variables = c('x') # Variables to be monitored
type = 'fs'; rs_type = 'stratified'; rs_thres = 0.5 # Optional parameters

#+
#' **Run SMC**
out_smc = smc.samples(model, variables, n_part,
                      type=type, rs.type=rs_type, rs.thres=rs_thres)

#+
#' **Diagnostic on the algorithm**
diagnostic(out_smc$x)

#+
#' **Summary statistics**
summ = summary(out_smc$x, fun=c('mean', 'median', 'quantiles'), 
               probs=c(.025, .975))

#+ fig.cap='SMC: Filtering estimates'
#' **Plot Filtering estimates**
x_f_mean = summ$f$Mean
x_f_med = summ$f$Med
x_f_inf = summ$f$'Qu. 0.025'
x_f_sup = summ$f$'Qu. 0.975'

plot(x_f_mean, type='l', col=4, 'lwd'=3,
     xlab='Time', ylab='Estimates', 
     ylim=c(min(x_f_inf), max(x_f_sup)),
     bty='n')
legend('topright', leg='Filtering Mean Estimate', 
       col=4, lwd=3, bty='n')

mycols = adjustcolor(palette('default'), alpha.f=.3)
palette(mycols)
polygon(c(1:t_max, t_max:1), c(x_f_inf, rev(x_f_sup)),
        col=4, border=NA)
legend('topleft', leg='95 % credible interval', 
       col=4, pch=15, pt.cex=2, bty='n')
palette('default')

#+ fig.cap='SMC: Smoothing estimates'
#' **Plot Smoothing estimates**
x_s_mean = summ$s$Mean
x_s_med = summ$s$Med
x_s_inf = summ$s$'Qu. 0.025'
x_s_sup = summ$s$'Qu. 0.975'

plot(x_s_mean, type='l', col=4, lwd=3, 
     xlab='Time', ylab='Estimates',, 
     ylim=c(min(x_s_inf), max(x_s_sup)),
     bty='n')
legend('topright', leg='Smoothing Mean Estimate', 
       col=4, lwd=3, bty='n')

palette(mycols)
polygon(c(1:t_max, t_max:1), c(x_s_inf, rev(x_s_sup)),
        col=4, border=NA)
legend('topleft', leg='95 % credible interval', 
       col=4, pt.cex=2, pch=15, bty='n')
palette('default')

#+ fig.cap='SMC: Marginal posteriors'
#' **Marginal filtering and smoothing densities**
kde_estimates = density(out_smc$x)
time_index = c(5, 10, 15, 20)
par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
  tk = time_index[k]
  plot(kde_estimates[[tk]]$f$density, col=4,
       xlab=bquote(x[.(tk)]), ylab='posterior density',
       main=paste('t=', tk, sep=''),
       ylim=c(0, max(c(kde_estimates[[tk]]$f$density$y, kde_estimates[[tk]]$s$density$y))))
  lines(kde_estimates[[tk]]$s$density, col=2)
  points(data$x_true[tk], 0, col=3, pch=8)
}
legend('topright', leg=c('filtering density', 'smoothing density', 'True value'), 
       col=c(4, 2, 3), pch=c(NA,NA,8), lty=c(1,1,NA), bg='white')
par(mfrow=c(1,1))


#' ## BiiPS Particle Independent Metropolis-Hastings
#' We now use BiiPS to run a Particle Independent Metropolis-Hastings

#+
#' **Parameters of the PIMH**
n_burn = 500
n_iter = 500
thin = 1
n_part = 100

#+
#' **Run PIMH**
pimh.update(model, variables, n.iter=n_burn, n.part=n_part) # burn-in iterations
out_pimh = pimh.samples(model, variables, n_iter, n_part, thin=thin)

#+
#' **Some summary statistics**
require(rjags)
summary_pimh = summary(as.mcmc.list(out_pimh$x), quantiles=c(.025, .975))

#+ fig.cap='PIMH: Posterior mean and quantiles'
#' **Posterior mean and quantiles**
x_pimh_mean = summary_pimh$statistics[,'Mean']
x_pimh_inf = summary_pimh$quantiles[,1]
x_pimh_sup = summary_pimh$quantiles[,2]
plot(x_pimh_mean, lwd=3, type='l', col=4,
     xlab='Time', ylab='Estimates', 
     ylim=c(min(x_pimh_inf), max(x_pimh_sup)),
     bty='n')
legend('topright', leg='PIMH Mean Estimate', 
       col=4, lwd=3, bty='n')

palette(mycols)
polygon(c(1:t_max, t_max:1), c(x_pimh_inf, rev(x_pimh_sup)), 
        col=4, border=NA)
legend('topleft', leg='95 % credible interval', 
       col=4, pch=15, pt.cex=2, bty='n')
palette('default')

#+ fig.cap='PIMH: Trace samples'
#' **Trace of MCMC samples**
time_index = c(5, 10, 15, 20)
par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
  tk = time_index[k]
  plot(out_pimh$x[tk,], col=4, type='l',
       xlab='Iterations', ylab='PIMH samples',
       main=paste('t=', tk, sep=''))
  points(0, data$x_true[tk], col=3, pch=8)
}
legend('topright', leg=c('PIMH samples', 'True value'), 
       col=c(4, 3), pch=c(NA,8), lty=c(1,NA), bg='white')


#+ fig.cap='PIMH: Histograms Marginal Posteriors'
#' **Histograms of posteriors**
par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
  tk = time_index[k]
  hist(out_pimh$x[tk,], breaks=20, col=4,
       xlab='Iterations', ylab='number of samples',
       main=paste('t=', tk, sep=''))
  points(data$x_true[tk], 0, col=3, pch=8)
}
legend('topright', leg=c('posterior density', 'True value'), 
       col=c(1, 3), pch=c(22,8), lwd=1, lty=NA, pt.cex=c(2,1), pt.bg=c(4,NA), 
       bg='white')

#+ fig.cap='PIMH: KDE estimates Marginal posteriors'
#' **Kernel density estimates of posteriors**
par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
  tk = time_index[k]
  kde_estimate_pimh = density(out_pimh$x[tk,])
  plot(kde_estimate_pimh, col=4,
       xlab=bquote(x[.(tk)]), ylab='posterior density',
       main=paste('t=', tk, sep=''))
  points(data$x_true[tk], 0, col=3, pch=8)
}
legend('topright', leg=c('posterior density', 'True value'), 
       col=c(4, 3), pch=c(NA,8), lty=c(1,NA), bg='white')
par(mfrow=c(1,1))