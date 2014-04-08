#+ setup, include=FALSE
library(knitr)
opts_chunk$set(cache=TRUE, comment=NA, background='white', fig.show='hold',
               tidy=FALSE, pars=TRUE)
knit_hooks$set(pars = function(before, options, envir) {
  if (before) graphics::par(bty='l')
})

#' # RBiips: Tutorial 3
#' In this tutorial, we will see how to introduce user-defined functions in the BUGS model.

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
#' of mean $m$ and covariance matrix $S$, $h(x)=x^2/20$, $f(x,t-1)=0.5 x+25 x/(1+x^2)+8 \cos(1.2 (t-1))$, $\mu_0=0$, $\lambda_0 = 5$, $\lambda_x = 0.1$ and $\lambda_y=1$. 

#' ## Statistical model in BUGS language
#' One needs to describe the model in BUGS language. We create the file
#'  'hmm_1d_nonlin_funmat.bug':

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
#'         x_true[t] ~ dnorm(funmat(x_true[t-1],t-1), prec_x)
#'         y[t] ~ dnorm(x_true[t]^2/20, prec_y)
#'       }
#'     }
#' 
#' 
#'     model
#'     {
#'       x[1] ~ dnorm(mean_x_init, prec_x_init)
#'       y[1] ~ dnorm(x[1]^2/20, prec_y)
#'       for (t in 2:t_max)
#'       {
#'         x[t] ~ dnorm(funmat(x[t-1],t-1), prec_x)
#'         y[t] ~ dnorm(x[t]^2/20, prec_y)
#'       }
#'     }
#'
#' Although the nonlinear function f can be defined in BUGS language, we
#' choose here to use an external user-defined function 'funmat', which will
#' call a Matlab function. 

#' ## User-defined functions in Matlab
#' The BUGS model calls a function funcmat. In order to be able to use this
#' function, one needs to create two functions in Matlab. The first
#' function, called here 'f_eval.m' provides the evaluation of the function.
#'
#' *f_eval.m*
#'
#'     function out = f_eval(x, k)
#' 
#'     out = .5 * x + 25*x/(1+x^2) + 8*cos(1.2*k)
#'
#' The second function, f_dim.m, provides the dimensions of the output of f_eval, 
#' possibly depending on the dimensions of the inputs.
#'
#' *f_dim.m* 
#'
#'     function out_dim = f_dim(x_dim, k_dim)
#' 
#'     out_dim = c(1,1)


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
prec_x_init = 1/5
prec_x = 1/10
prec_y = 1
data = list(t_max=t_max, prec_x_init=prec_x_init,
    prec_x=prec_x,  prec_y=prec_y, mean_x_init=mean_x_init)

#+
#' **Load RBiips package**
require(RBiips)

#+
#' *Add the user-defined function 'funmat'*
fun_bugs = 'funmat'; fun_dim = 'f_dim';funeval = 'f_eval';fun_nb_inputs = 2
biips_add_function(fun_bugs, fun_nb_inputs, fun_dim, funeval)


#+
#' **Compile BUGS model and sample data**
model = 'hmm_1d_nonlin_funmat$bug' # BUGS model filename
sample_data = TRUE # Boolean
c(model_id, data) = biips_model(model, data, sample_data=sample_data) # Create biips model and sample data
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
out_smc = biips_smc_samples(model_id, variables, n_part,
    type=type, rs_type=rs_type, rs_thres=rs_thres)

#+
#' **Diagnostic on the algorithm**. 
diag = biips_diagnostic(out_smc)

#+
#' **Summary statistics**
summary = summary(out_smc, probs=c(.025, .975))

#+
#' **Plot Filtering estimates**
x_f_mean = summary$x$f$Mean
x_f_med = summary$x$f$Med
x_f_inf = summary$x$f$'Qu. 0.025'
x_f_sup = summary$x$f$'Qu. 0.925'


lightcols = adjustcolor(palette('default'), alpha.f=.3)
palette(lightcols)
plot(c(1:t_max, t_max:1), c(x_f_inf, rev(x_f_sup)), type='n')
polygon(c(1:t_max, t_max:1), c(x_f_inf, rev(x_f_sup)),
        col='blue', border=NA))
legend('topleft', leg='95 % credible interval', 
       col='blue', pch=15, pt.cex=2, bty='n')
palette('default')


lines(x_f_mean, lwd=3)
title(xlab='Time')
title(ylab='Estimates')
legend('topright', leg=c('Filtering Mean Estimate'),
       col=c('green3','red'), lwd=c(3,2), bty='n')



#+
#' **Plot Smoothing estimates**
x_s_mean = summary$x$s$Mean
x_s_inf = summary$x$s$'Qu. 0.025'
x_s_sup = summary$x$s$'Qu. 0.925'

palette(lightcols)
plot(c(1:t_max, t_max:1), c(x_s_inf, rev(x_s_sup)), type='n')
polygon(c(1:t_max, t_max:1), c(x_s_inf, rev(x_s_sup)),
        col='blue', border=NA))
legend('topleft', leg='95 % credible interval', 
       col='blue', pch=15, pt.cex=2, bty='n')
palette('default')


lines(x_s_mean, lwd=3)
title(xlab='Time')
title(ylab='Estimates')
legend('topright', leg=c('Smoothing Mean Estimate'),
       col=c('green3','red'), lwd=c(3,2), bty='n')



#+
#' Marginal filtering and smoothing densities

kde_estimates = density(out_smc)
time_index = c(5, 10, 15, 20)

par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
    tk = time_index[k]

    plot(kde_estimates$x[[tk]], col=c('blue','red'), lty=1, sub='',
         legend.text=FALSE)
    points(data$x_true[tk], 0, col='green3', pch=8)
    title(xlab=bquote(x[.(tk)]))
    title(ylab='posterior density')
    title(main=paste('t=', tk, sep=''))
}
legend('topright', leg=c('filtering density', 'smoothing density', 'True value'),
       col=c('green3','blue','red'), pch=c(NA,NA,8), lty=c(1,1,NA), bg='white')
par(mfrow=c(1,1))

#' ## Clear model
#' 

biips_clear(model_id)
