#' ---
#' title: "Rbiips example: Object tracking"
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
#' In this example, we consider the tracking of an object in 2D, observed by
#' a radar.
#'
#' Reference: B. Ristic. Beyond the Kalman filter: Particle filters for
#' Tracking applications. Artech House, 2004.

#+ setup, include=FALSE
knitr::opts_chunk$set(cache=TRUE, comment=NA, warning=FALSE, fig.show='hold', fig.align='center', pars=TRUE)
knitr::knit_hooks$set(pars = function(before, options, envir) {
  if (before) par(bty='l')
})

#' # Statistical model
#' The statistical model is defined as follows.
#'
#' Let $X_t$ be a 4-D vector containing the position and velocity of an
#' object in 2D. We obtain distance-angular measurements $Y_t$ from a radar.
#'
#' The model is defined as follows. For $t=1,\ldots,t_{\max}$
#'
#' $$ X_t = F X_{t-1} + G V_t,~~ V_t\sim\mathcal N(0,Q)$$
#'
#' $$ Y_{t} = g(X_t) + W_t,~~ W_t \sim\mathcal N(0,R)$$
#'
#' $F$ and $G$ are known matrices, $g(X_t)$ is the known nonlinear measurement function and $Q$ and $R$ are known covariance matrices.

#'
#' # Statistical model in BUGS language
model_file = 'hmm_4d_nonlin_tracking.bug' # BUGS model filename
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

#' Set the random numbers generator seed for reproducibility
set.seed(5)

#' # Load model and data

#' #### Model parameters
t_max = 20
mean_x_init = c(0, 0, 1, 0)
prec_x_init = diag(1000, nrow = 4)
x_pos = c(60, 0)
mean_v = c(0, 0)
prec_v = diag(1, 2)
prec_y = diag(c(100, 500))
delta_t = 1
F = matrix(c(1, 0, delta_t, 0,
             0, 1, 0, delta_t,
             0, 0, 1, 0,
             0, 0, 0, 1), nrow=4, byrow=TRUE)
G = matrix(c(delta_t^2/2, 0,
    0, delta_t^2/2,
    delta_t, 0,
    0, delta_t), nrow=4, byrow=TRUE)

data = list(t_max=t_max, mean_x_init=mean_x_init, prec_x_init=prec_x_init,
            x_pos=x_pos, mean_v=mean_v, prec_v=prec_v, prec_y=prec_y,
            F=F, G=G)

#' #### Compile BUGS model and sample data
sample_data = TRUE # Boolean
model = biips_model(model_file, data, sample_data=sample_data) # Create Biips model and sample data
data = model$data()
x_pos_true = data$x_true[1:2,]

#' # Biips Particle filter
#'
#' #### Parameters of the algorithm
n_part = 100000 # Number of particles
variables = c('x') # Variables to be monitored

#' #### Run SMC
out_smc = biips_smc_samples(model, variables, n_part)

#' #### Diagnostics of the algorithm
diag_smc = biips_diagnosis(out_smc)

#' #### Plot particles
#+ fig.cap = 'SMC: Particles (filtering)'
matplot(out_smc$x$f$values[1,,], out_smc$x$f$values[2,,],
        type='p', col='red', pch=16, cex=.2,
        xlab='Position X', ylab='Position Y')
lines(x_pos_true[1,], x_pos_true[2,], type='l', col='green', lwd=2, lty=2)
points(x_pos[1], x_pos[2], col='black', pch=0, lwd=2)
legend('bottomleft', leg=c('Particles (filtering)', 'True trajectory',
                           'Position of the radar'),
       col=c('red', 'green', 'black'),
       lwd=2, pch=c(16,NA,0), lty=c(NA,2,NA),
       bty='n')

#' #### Summary statistics
summ_smc = biips_summary(out_smc, probs=c(.025, .975))

#' #### Plot estimates
#+ fig.cap='SMC: Filtering and smoothing estimates'
x_f_mean = summ_smc$x$f$mean
x_s_mean = summ_smc$x$s$mean

plot(x_f_mean[1,], x_f_mean[2,], type='l', col='blue', lwd=2,
     xlab='Position X', ylab='Position Y')
lines(x_s_mean[1,], x_s_mean[2,], type='l', col='red', lwd=2, lty=4)
lines(x_pos_true[1,], x_pos_true[2,], type='l', col='green', lwd=2, lty=2)
points(x_pos[1], x_pos[2], col='black', pch=0, lwd=2)
legend('bottomleft', leg=c('Filtering estimate', 'Smoothing estimate',
                        'True trajectory', 'Position of the radar'),
       col=c('blue', 'red', 'green', 'black'),
       lwd=2, pch=c(NA,NA,NA,0), lty=c(1,4,2,NA),
       bty='n')

title_fig = c('Position X', 'Position Y', 'Velocity X', 'Velocity Y')

#' #### Plot Filtering estimates
#+ fig.cap='SMC: Filtering estimates', fig.subcap=title_fig
x_f_quant = summ_smc$x$f$quant

for (k in 1:4) {
  xx = c(1:t_max, t_max:1)
  yy = c(x_f_quant[[1]][k,], rev(x_f_quant[[2]][k,]))
  plot(xx, yy, type='n', xlab='Time', ylab='Estimates',
       main=title_fig[k])

  polygon(xx, yy, col=light_blue, border=NA)
  lines(1:t_max, x_f_mean[k,], col='blue', lwd=3)
  lines(1:t_max, data$x_true[k,], col='green', lwd=2)
  legend('topleft', leg=c('95% credible interval', 'Filtering mean estimate', 'True value'),
         col=c(light_blue,'blue','green'), lwd=c(NA,3,2), pch=c(15,NA,NA), pt.cex=c(2,1,1), bty='n')
}
#' #### Plot Smoothing estimates
#+ fig.cap='SMC: Smoothing estimates', fig.subcap=title_fig
x_f_quant = summ_smc$x$f$quant
x_s_mean = summ_smc$x$s$mean
x_s_quant = summ_smc$x$s$quant

for (k in 1:4) {
  xx = c(1:t_max, t_max:1)
  yy = c(x_s_quant[[1]][k,], rev(x_s_quant[[2]][k,]))
  plot(xx, yy, type='n', xlab='Time', ylab='Estimates',
       main=title_fig[k])

  polygon(xx, yy, col=light_red, border=NA)
  lines(1:t_max, x_s_mean[k,], col='red', lwd=3)
  lines(1:t_max, data$x_true[k,], col='green', lwd=2)
  legend('topleft', leg=c('95% credible interval', 'Smoothing mean estimate', 'True value'),
         col=c(light_red,'red','green'), lwd=c(NA,3,2), pch=c(15,NA,NA), pt.cex=c(2,1,1), bty='n')
}