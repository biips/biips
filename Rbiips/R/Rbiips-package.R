##' Bayesian inference with interacting Particle Systems
##'
##' \pkg{Rbiips} is an interface with the \href{http://alea.bordeaux.inria.fr/biips}{Biips}
##' C++ libraries for analysing Bayesian graphical models using advanced particle
##' methods.
##'
##' Biips is a general software for Bayesian inference with interacting particle
##' systems, a.k.a. sequential Monte Carlo (SMC) methods.
##' It aims at popularizing the use of these methods to non-statistician
##' researchers and students, thanks to its automated \dQuote{black box} inference engine.
##' It borrows from the BUGS/JAGS software, widely used in Bayesian statistics,
##' the statistical modeling with graphical models and the language associated
##' with their descriptions.
##' Unlike MCMC methods used by BUGS/JAGS, SMC methods are more adapted to
##' dynamic problems (tracking, signal filtering, etc).
##'
##' Typical usage of \pkg{Rbiips} consits in: \enumerate{ \item
##' compiling your model with \code{\link{biips_model}} function \item
##' optionally analyzing sensitivity to parameters with
##' \code{\link{biips_smc_sensitivity}} \item generating samples from the conditional
##' distributions \itemize{ \item using standard SMC algorithm with
##' \code{\link{biips_smc_samples}} function \item using particle MCMC methods with
##' \code{\link{biips_pimh_samples}} or \code{\link{biips_pmmh_samples}} functions } \item
##' analysing output from \code{\link{smcarray}} and
##' \code{\link{mcmcarray}} objects }
##'
##' @name Rbiips-package
##' @aliases Rbiips-package Rbiips
##' @docType package
##' @author \pkg{Rbiips} development is supported by the team
##' \href{http://alea.bordeaux.inria.fr}{ALEA} at
##' \href{http://www.inria.fr/en/centre/bordeaux}{Inria Bordeaux Sud-Ouest}.
##' @author \href{https://sites.google.com/site/adrientodeschini/}{Adrien Todeschini}
##' @author \href{http://www.stats.ox.ac.uk/~caron/}{François Caron}
##' @author Marc Fuentes
##' @author Adapted from \pkg{rjags}.
##' @seealso \link[rjags:jags.model]{rjags}, \link[pomp:pomp-package]{pomp}
##' @references %%~~ Literature or other references for background information
##' ~~
##' @keywords package
##' @useDynLib Rbiips
##' @examples
##'
##' %% ~~ simple examples of the most important functions ~~
##'
NULL


##' @keywords internal
.onLoad <- function(lib, pkg) {
    Rbiips("load_module", "basemod")
}


##' Helper function to call Rbiips C++ routines
##' @keywords internal
Rbiips <- function(funcname, ...) {
    stopifnot(is.character(funcname), length(funcname) == 1,
        nchar(funcname) > 0)
    .Call(funcname, ..., PACKAGE = "Rbiips")
}
