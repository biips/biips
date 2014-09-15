#' ---
#' title: "Rbiips: Tutorial 2"
#' author: NULL
#' date: NULL
#' output:
#'   html_document:
#'     toc: true
#'     fig_caption: true
#'     theme: cerulean
#'     highlight: tango
#'   pdf_document:
#'     toc: true
#'     fig_caption: true
#'     highlight: tango
#' ---
#' In this tutorial, we consider applying sequential Monte Carlo methods for
#' sensitivity analysis and parameter estimation in a nonlinear non-Gaussian hidden Markov model.

#+ setup, include=FALSE
knitr::opts_chunk$set(cache=TRUE, comment=NA, warning=FALSE, fig.show='hold', fig.align='center', pars=TRUE)
knitr::knit_hooks$set(pars = function(before, options, envir) {
  if (before) par(bty='l')
})

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
#' where $\mathcal N\left (m, S\right )$ denotes the Gaussian distribution
#' of mean $m$ and covariance matrix $S$, $h(x)=x^2/20$, $f(x,t-1)=0.5
#' x+25 x/(1+x^2)+8 \cos(1.2 (t-1))$, $\mu_0=0$, $\lambda_0 = 5$, $\lambda_x
#' = 0.1$.  The precision of the observation noise
#' $\lambda_y$ is also assumed to be unknown. We will assume a uniform prior
#' for $\log(\lambda_y)$:
#'
#' $$ \log(\lambda_y) \sim Unif(-3,3) $$

#+
#' ## Statistical model in BUGS language
#' We describe the model in BUGS language in the file `'hmm_1d_nonlin_param.bug'`:
model_file = 'hmm_1d_nonlin_param.bug' # BUGS model filename
cat(readLines(model_file), sep = "\n")

#+
#' ## Installation of Rbiips package
#' 1. [Download](https://alea.bordeaux.inria.fr/biips/doku.php?id=download) the latest version of Rbiips package depending on your system
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

#'
#' **Note:** Linux installation needs a previous installation of
#' Biips libraries and headers as well as Boost.

#+
#' ## General settings
par(bty='l')
light_blue = rgb(.7, .7, 1);
light_red = rgb(1, .7, .7);

#' Set the random numbers generator seed for reproducibility
set.seed(2)

#' ## Load model and data

#' **Model parameters**
t_max = 20
mean_x_init = 0
prec_x_init = 1/5
prec_x = 1/10
log_prec_y_true = log(1) # True value used to sample the data
data = list(t_max=t_max, prec_x_init=prec_x_init,
            prec_x=prec_x, log_prec_y_true=log_prec_y_true,
            mean_x_init=mean_x_init)

#' **Compile BUGS model and sample data**
sample_data = TRUE # Boolean
model = biips_model(model_file, data, sample_data=sample_data) # Create Biips model and sample data
data = model$data()


#' ## Biips Sensitivity analysis with Sequential Monte Carlo
#' Let now use Biips to provide estimates of the marginal log-likelihood and
#' penalized marginal log-likelihood given various values of the
#' log-precision parameters $\log(\lambda_y)$.


#' ## Biips Sequential Monte Carlo
#' Let now use Biips to run a particle filter.

#' **Parameters of the algorithm**.
n_part = 100 % Number of particles
param_names = c('log_prec_y') # Parameter for which we want to study sensitivity
param_values = as.list(-5:.2:3) # Range of values

#' *Run sensitivity analysis with SMC*
out_sens = smc_sensitivity(model, param_names, param_values, n_part)

#' **Plot log-marginal likelihood and penalized log-marginal likelihood**
#+ fig.cap='Log-marginal likelihood'
plot(param_values{1}, out_sens$log_marg_like, pch=20,
     xlab ='Parameter log_prec_y',
     ylab = 'Log-marginal likelihood')

#+ fig.cap='Penalized log-marginal likelihood'
plot(param_values{1}, out_sens$log_marg_like_pen, pch=20,
     xlab ='Parameter log_prec_y',
     ylab = 'Penalized log-marginal likelihood')


#' ## Biips Particle Marginal Metropolis-Hastings
#' We now use Biips to run a Particle Marginal Metropolis-Hastings in order
#' to obtain posterior MCMC samples of the parameter and the variables $x$.

#' **Parameters of the PMMH**.
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

#' **Init PMMH**
obj_pmmh = pmmh_init(model, param_names, inits=list(-2),
                     latent_names=latent_names) # creates a pmmh object

#' **Run PMMH**
obj_pmmh = pmmh_update(obj_pmmh, n_burn, n_part) # adaptation and burn-in iterations
out_pmmh = pmmh_samples(obj_pmmh, n_iter, n_part, thin=thin) # samples

#' **Some summary statistics**
summary_pmmh = summary(out_pmmh, 'probs', [.025, .975])

