#' ---
#' title: "Rbiips: Tutorial 3"
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
#' In this tutorial, we will see how to introduce user-defined functions in the BUGS model.

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
#' of mean $m$ and covariance matrix $S$, $h(x)=x^2/20$,
#' $f(x,t-1)=0.5 x+25 x/(1+x^2)+8 \cos(1.2 (t-1))$, $\mu_0=0$, $\lambda_0 = 5$,
#' $\lambda_x = 0.1$ and $\lambda_y=1$.

#'
#' # Statistical model in BUGS language
#' We describe the model in BUGS language in the file `'hmm_1d_nonlin_fext.bug'`:
model_file = 'hmm_1d_nonlin_fext.bug' # BUGS model filename
cat(readLines(model_file), sep = "\n")

#' Although the nonlinear function `f` can be defined in BUGS language, we
#' choose here to use an external user-defined function `fext`, which will call a R function.

#'
#' # User-defined functions in R
#' The BUGS model calls a function `fext`. In order to be able to use this function,
#' one needs to create two functions in R. The first function, called here `f_eval`
#' provides the evaluation of the function.
f_eval <- function(x, k) { .5 * x + 25*x/(1+x^2) + 8*cos(1.2*k) }

#' The second function, `f_dim`, provides the dimensions of the output of `f_eval`,
#' possibly depending on the dimensions of the inputs.
f_dim <- function(x_dim, k_dim) { 1 }

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

#' Set the random numbers generator seed for reproducibility
set.seed(2)

#' # Load model and data

#' #### Model parameters
t_max = 20
mean_x_init = 0
prec_x_init = 1/5
prec_x = 1/10
prec_y = 1
data = list(t_max=t_max, prec_x_init=prec_x_init,
            prec_x=prec_x, prec_y=prec_y, mean_x_init=mean_x_init)

#' #### Add the user-defined function `fext`
fun_bugs = 'fext'; fun_nb_inputs = 2
fun_dim = f_dim; fun_eval = f_eval
biips_add_function(fun_bugs, fun_nb_inputs, fun_dim, fun_eval)

#' #### Compile BUGS model and sample data
sample_data = TRUE # Boolean
model = biips_model(model_file, data, sample_data=sample_data) # Create Biips model and sample data
data = model$data()

#' # Biips Sequential Monte Carlo
#' Let now use Biips to run a particle filter.

#'
#' #### Parameters of the algorithm
#' We want to monitor the variable `x`, and to
#' get the filtering and smoothing particle approximations. The algorithm
#' will use 10000 particles, stratified resampling, with a threshold of 0.5.
n_part = 10000 # Number of particles
variables = c('x') # Variables to be monitored
mn_type = 'fs'; rs_type = 'stratified'; rs_thres = 0.5 # Optional parameters

#' #### Run SMC
out_smc = biips_smc_samples(model, variables, n_part,
                      type=mn_type, rs_type=rs_type, rs_thres=rs_thres)

#' #### Diagnosis of the algorithm
diag_smc = biips_diagnosis(out_smc)

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
       col=c(light_blue,'blue','green'), lwd=c(NA,3,2), pch=c(15,NA,NA), pt.cex=c(2,1,1), bty='n')

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
       col=c(light_red,'red','green'), lwd=c(NA,3,2), pch=c(15,NA,NA), pt.cex=c(2,1,1), bty='n')

#' #### Marginal filtering and smoothing densities
#+ fig.cap='SMC: Marginal posteriors'
kde_smc = biips_density(out_smc)
time_index = c(5, 10, 15)
par(mfrow=c(2,2))
for (k in 1:length(time_index)) {
  tk = time_index[k]
  plot(kde_smc$x[[tk]], col=c('blue','red'), lwd=2,
       xlab=bquote(x[.(tk)]), ylab='Posterior density',
       main=paste('t=', tk, sep=''))
  points(data$x_true[tk], 0, col='green', pch=8, lwd=2)
}
plot(0, type='n', bty='n', xaxt='n', yaxt='n', xlab="", ylab="")
legend('center', leg=c('Filtering density', 'Smoothing density', 'True value'),
       col=c('blue', 'red', 'green'), pch=c(NA,NA,8), lty=c(1,1,NA), lwd=2,
       bty='n')
par(mfrow=c(1,1))
