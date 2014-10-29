#' Bayesian inference with interacting Particle Systems
#'
#' \pkg{Rbiips} is an interface with the \href{http://alea.bordeaux.inria.fr/biips}{Biips}
#' C++ libraries for analysing Bayesian graphical models using advanced particle
#' methods.
#'
#' Biips is a general software for Bayesian inference with interacting particle
#' systems, a.k.a. sequential Monte Carlo (SMC) methods.
#' It aims at popularizing the use of these methods to non-statistician
#' researchers and students, thanks to its automated \dQuote{black box} inference engine.
#' It borrows from the BUGS/JAGS software, widely used in Bayesian statistics,
#' the statistical modeling with graphical models and the language associated
#' with their descriptions.
#' Unlike MCMC methods used by BUGS/JAGS, SMC methods are more adapted to
#' dynamic problems (tracking, signal filtering, etc).
#'
#' Typical usage of \pkg{Rbiips} consits in:
#' \itemize{
#'   \item compiling your model with \code{\link{biips_model}} function
#'   \item analyzing sensitivity to parameters with \code{\link{biips_smc_sensitivity}}
#'   \item generating samples from the conditional
#'     distributions
#'     \itemize{
#'       \item using standard SMC algorithm with \code{\link{biips_smc_samples}}
#'         function
#'       \item using particle MCMC methods with \code{\link{biips_pimh_samples}}
#'         or \code{\link{biips_pmmh_samples}} functions
#'     }
#'   \item analysing output from \code{\link{smcarray}} and \code{\link{mcmcarray}}
#'     objects
#' }
#'
#' @name Rbiips-package
#' @aliases Rbiips-package Rbiips
#' @docType package
#' @author \pkg{Rbiips} development is supported by the team
#' \href{http://alea.bordeaux.inria.fr}{ALEA} at
#' \href{http://www.inria.fr/en/centre/bordeaux}{Inria Bordeaux Sud-Ouest}.
#' @author \href{https://sites.google.com/site/adrientodeschini/}{Adrien Todeschini}
#' @author \href{http://www.stats.ox.ac.uk/~caron/}{Francois Caron}
#' @author Marc Fuentes
#' @author \pkg{Rbiips} is adapted from \pkg{rjags} interface for \href{http://mcmc-jags.sourceforge.net/}{JAGS}.
#' @seealso \link[rjags:jags.model]{rjags}, \link[pomp:pomp-package]{pomp}
#' @references TODO ******* add references ********
#' @keywords package
#' @useDynLib Rbiips
#' @examples
#' ## Compile model ------------------------------
#' modelfile <- system.file("extdata", "hmm.bug", package = "Rbiips")
#' stopifnot(nchar(modelfile)>0)
#' cat(readLines(modelfile), sep="\n")
#'
#' data <- list(tmax = 10, logtau = log(10))
#' model <- biips_model(modelfile, data, sample_data = TRUE)
#'
#' ## SMC ------------------------------
#' n_part <- 100
#'
#' out_smc <- biips_smc_samples(model, "x", n_part)
#'
#' biips_diagnosis(out_smc)
#' summ_smc <- biips_summary(out_smc, order = 2, probs = c(0.025, 0.975))
#' dens_smc <- biips_density(out_smc, bw='nrd0', adjust=1, n = 100)
#'
#' ## PIMH ------------------------------
#' n_part <- 50
#' obj_pimh <- biips_pimh_init(model, "x")  # Initialize
#' out_pimh_burn <- biips_pimh_update(obj_pimh, 100, n_part)  # Burn-in
#' out_pimh <- biips_pimh_samples(obj_pimh, 100, n_part)  # Samples
#'
#' summ_pimh <- biips_summary(out_pimh, order = 2, probs = c(0.025, 0.975))
#' dens_pimh <- biips_density(out_pimh)
#'
#' ## SMC sensitivity analysis ------------------------------
#' n_part <- 50
#' logtau_val <- -10:10
#' out_sens <- biips_smc_sensitivity(model, list(logtau = logtau_val),
#'                                   n_part)
#'
#' ## PMMH ------------------------------
#' modelfile <- system.file("extdata", "hmm.bug", package = "Rbiips")
#' stopifnot(nchar(modelfile)>0)
#'
#' data <- list(tmax = 10)
#' model <- biips_model(modelfile, data, sample_data = TRUE)
#'
#' n_part <- 50
#' obj_pmmh <- biips_pmmh_init(model, "logtau", latent_names = "x",
#'                             inits = list(logtau = -2))  # Initialize
#' out_pmmh_burn <- biips_pmmh_update(obj_pmmh, 100, n_part)  # Burn-in
#' out_pmmh <- biips_pmmh_samples(obj_pmmh, 100, n_part, thin = 1)  # Samples
#'
#' summ_pmmh <- biips_summary(out_pmmh, order = 2, probs = c(0.025, 0.975))
#' dens_pmmh <- biips_density(out_pmmh)
NULL


#' @keywords internal
.onLoad <- function(lib, pkg) {
    Rbiips("load_module", "basemod")
}


#' Helper function to call Rbiips C++ routines
#' @keywords internal
Rbiips <- function(funcname, ...) {
    stopifnot(is.character(funcname), length(funcname) == 1,
        nchar(funcname) > 0)
    .Call(funcname, ..., PACKAGE = "Rbiips")
}
