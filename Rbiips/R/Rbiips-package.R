#' Bayesian inference with interacting particle systems.
#'
#' \pkg{Rbiips} is an interface with the \href{http://alea.bordeaux.inria.fr/biips}{Biips} C++ libraries for analysing
#' Bayesian graphical models using advanced particle methods.
#'
#' Biips is a general software for Bayesian inference with interacting particle
#' systems, a.k.a. sequential Monte Carlo (SMC) methods. It aims at popularizing
#' the use of these methods to non-statistician researchers and students, thanks
#' to its automated \dQuote{black box} inference engine. It borrows from the
#' \href{http://www.mrc-bsu.cam.ac.uk/software/bugs/}{BUGS}/\href{http://mcmc-jags.sourceforge.net/}{JAGS}
#' software, widely used in Bayesian statistics, the statistical
#' modeling with graphical models and the language associated with their
#' descriptions.
#'
#' See the \href{http://alea.bordeaux.inria.fr/biips}{Biips website} for more
#' information.
#'
#' The typical workflow is the following:
#' \itemize{
#'   \item Define the model in BUGS language (see the \href{http://sourceforge.net/projects/mcmc-jags/files/Manuals/3.x/jags_user_manual.pdf/download}{JAGS User Manual}
#'   for help) and the data.
#'   \item Add custom functions or distributions with
#'     \code{\link{biips_add_function}} and \code{\link{biips_add_distribution}}.
#'   \item Compile the model with \code{\link{biips_model}}
#'   \item Run inference algorithms:
#'     \itemize{
#'       \item Analyse sensitivity to parameters with \code{\link{biips_smc_sensitivity}}.
#'       \item Run SMC filtering and smoothing algorithms with \code{\link{biips_smc_samples}}.
#'       \item Run particle MCMC algorithms with \code{\link{biips_pimh_samples}} or
#'         \code{\link{biips_pmmh_samples}}.
#'     }
#'   \item Diagnose and analyze the output obtained as \code{\link{smcarray}} and
#'     \code{\link{mcmcarray}} objects with \code{\link{biips_diagnosis}},
#'     \code{\link{biips_summary}}, \code{\link{biips_density}},
#'     \code{\link{biips_hist}} and \code{\link{biips_table}}
#'  }
#'
#' @name Rbiips-package
#' @aliases Rbiips
#' @docType package
#' @author \pkg{Rbiips} development is supported by the team
#'   \href{http://alea.bordeaux.inria.fr}{ALEA} at
#'   \href{http://www.inria.fr/en/centre/bordeaux}{Inria Bordeaux Sud-Ouest}.
#'   \itemize{
#'     \item \href{https://sites.google.com/site/adrientodeschini/}{Adrien
#'     Todeschini}
#'     \item \href{http://www.stats.ox.ac.uk/~caron/}{Francois Caron}
#'     \item Marc Fuentes
#'   }
#' \pkg{Rbiips} is adapted from \pkg{\link[rjags]{rjags}} interface for
#'   \href{http://mcmc-jags.sourceforge.net/}{JAGS}.
#' @seealso \code{\link{biips_add_function}}, \code{\link{biips_add_distribution}},
#'   \code{\link{biips_model}}, \code{\link{biips_smc_sensitivity}}, \code{\link{biips_smc_samples}},
#'   \code{\link{biips_pimh_samples}}, \code{\link{biips_pmmh_samples}}, \code{\link{smcarray}},
#'   \code{\link{mcmcarray}}, \code{\link{biips_diagnosis}}, \code{\link{biips_summary}},
#'   \code{\link{biips_density}}, \code{\link{biips_hist}}, \code{\link{biips_table}},
#'   \href{http://alea.bordeaux.inria.fr/biips}{Biips website},
#'   \href{http://sourceforge.net/projects/mcmc-jags/files/Manuals/3.x/jags_user_manual.pdf/download}{JAGS User Manual}
#' @references A. Todeschini, F. Caron, M. Fuentes, P. Legrand, P. Del Moral.
#'   Biips: A Software platform for Bayesian Inference with Interacting Particle Systems.
#'   Technical Report, 2014. (work in progress)
#' @keywords package
#' @useDynLib Rbiips
#' @examples
#' #' # Add custom functions and distributions to BUGS language
#' #' Add custom function `f`
#' f_dim <- function(x_dim, t_dim) {
#'   # Check dimensions of the input and return dimension of the output of function f
#'   stopifnot(prod(x_dim) == 1, prod(t_dim) == 1)
#'   x_dim
#' }
#' f_eval <- function(x, t) {
#'   # Evaluate function f
#'   0.5 * x + 25 * x/(1 + x^2) + 8 * cos(1.2 * t)
#' }
#' biips_add_function('f', 2, f_dim, f_eval)
#'
#' #' Add custom sampling distribution `dMN`
#' dMN_dim <- function(mu_dim, Sig_dim) {
#'   # Check dimensions of the input and return dimension of the output of
#'   # distribution dMN
#'   stopifnot(prod(mu_dim) == mu_dim[1], length(Sig_dim) == 2, mu_dim[1] == Sig_dim)
#'   mu_dim
#' }
#' dMN_sample <- function(mu, Sig) {
#'   # Draw a sample of distribution dMN
#'   mu + t(chol(Sig)) %*% rnorm(length(mu))
#' }
#' biips_add_distribution('dMN', 2, dMN_dim, dMN_sample)
#'
#' #' # Compile model
#' modelfile <- system.file('extdata', 'hmm_f.bug', package = 'Rbiips')
#' stopifnot(nchar(modelfile) > 0)
#' cat(readLines(modelfile), sep = '\n')
#'
#' data <- list(tmax = 10, p = c(.5, .5), logtau_true = log(1), logtau = log(1))
#' model <- biips_model(modelfile, data, sample_data = TRUE)
#'
#' #' # SMC algorithm
#' n_part <- 100
#' out_smc <- biips_smc_samples(model, c('x', 'c[2:10]'), n_part, type = 'fs',
#'                              rs_thres = 0.5, rs_type = 'stratified')
#'
#' biips_diagnosis(out_smc)
#' biips_summary(out_smc)
#' par(mfrow = c(2, 2))
#' plot(biips_density(out_smc$x, bw = 'nrd0', adjust = 1, n = 100))
#' plot(biips_table(out_smc[['c[2:10]']]))
#'
#' #' # PIMH algorithm
#' n_part <- 50
#' obj_pimh <- biips_pimh_init(model, c('x', 'c[2:10]'))  # Initialize
#' out_pimh_burn <- biips_pimh_update(obj_pimh, 100, n_part)  # Burn-in
#' out_pimh <- biips_pimh_samples(obj_pimh, 100, n_part)  # Samples
#'
#' biips_summary(out_pimh)
#' par(mfrow = c(2, 2))
#' plot(biips_density(out_pimh$x))
#' biips_hist(out_pimh$x)
#' plot(biips_table(out_pimh[['c[2:10]']]))
#'
#' #' # SMC sensitivity analysis
#' n_part <- 50
#' logtau_val <- -10:10
#' out_sens <- biips_smc_sensitivity(model, list(logtau = logtau_val), n_part)
#'
#' #' # PMMH algorithm
#' data <- list(tmax = 10, p = c(.5, .5), logtau_true = log(1))
#' model <- biips_model(modelfile, data)
#'
#' n_part <- 50
#' obj_pmmh <- biips_pmmh_init(model, 'logtau', latent_names = c('x', 'c[2:10]'),
#'                             inits = list(logtau = -2))  # Initialize
#' out_pmmh_burn <- biips_pmmh_update(obj_pmmh, 100, n_part)  # Burn-in
#' out_pmmh <- biips_pmmh_samples(obj_pmmh, 100, n_part, thin = 1)  # Samples
#'
#' biips_summary(out_pmmh)
#' par(mfrow = c(2, 2))
#' plot(biips_density(out_pmmh$logtau))
#' biips_hist(out_pmmh$logtau)
#' plot(biips_density(out_pmmh$x))
#' biips_hist(out_pmmh$x)
#' plot(biips_table(out_pmmh[['c[2:10]']]))
NULL
