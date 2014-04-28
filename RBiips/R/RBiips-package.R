##' Bayesian inference with interacting Particle Systems
##' 
##' \emph{RBiips} is an interface with the C++ \href{http://alea.bordeaux.inria.fr/biips}{BiiPS}
##' libraries for analysing Bayesian graphical models using advanced particle
##' methods.
##' 
##' BiiPS is a general software for Bayesian inference with interacting particle 
##' systems, a.k.a. sequential Monte Carlo (SMC) methods.
##' It aims at popularizing the use of these methods to non-statistician 
##' researchers and students, thanks to its automated 'black box' inference engine.
##' It borrows from the BUGS/JAGS software, widely used in Bayesian statistics, 
##' the statistical modeling with graphical models and the language associated 
##' with their descriptions.
##' Unlike MCMC methods used by BUGS/JAGS, SMC methods are more adapted to 
##' dynamic problems (tracking, signal filtering, etc).
##' 
##' Typical usage of \emph{RBiips} consits in: \enumerate{ \item
##' compiling your model with \code{\link{biips_model}} function \item
##' optionally analyzing sensitivity to parameters with
##' \code{\link{smc.sensitivity}} \item generate samples from the conditional
##' distributions \itemize{ \item using standard SMC algorithm with
##' \code{\link{smc_samples}} function \item using particle MCMC methods with
##' \code{\link{pmmh.samples}} or \code{\link{pimh_samples}} functions } \item
##' analysing output from \code{\link[smcarray-object]{smcarray}} and
##' \code{mcarray} objects }
##' 
##' @name RBiips-package
##' @aliases RBiips-package RBiips
##' @docType package
##' @author \emph{RBiips} development is supported by the team
##' \href{http://alea.bordeaux.inria.fr}{ALEA} at
##' \href{http://www.inria.fr/en/centre/bordeaux}{Inria Bordeaux Sud-Ouest}.
##' 
##' Individual authors: \itemize{ \item Adrien Todeschini (Software design and
##' development) \item Francois Caron (Probability expertise and
##' specifications) }
##' 
##' Maintainer: \email{biips-project@@lists.gforge.inria.fr}
##' @seealso \code{\link[rjags:jags.model]{rjags}},
##' \code{\link[pomp:pomp-package]{pomp}}
##' @references %%~~ Literature or other references for background information
##' ~~
##' @keywords package
##' @useDynLib RBiips
##' @examples
##' 
##' %% ~~ simple examples of the most important functions ~~
##' 
NULL


.onLoad <- function(lib, pkg) {
  RBiips("load_module", "basemod")
}


##' helper function to call RBiips c++ routines
RBiips <- function(funcname, ...) {
  stopifnot(is.character(funcname), length(funcname) == 1, nchar(funcname) > 0)
  .Call(funcname, ..., PACKAGE = "RBiips")
}


##' load the corresponding module into the Biips environment
##' @param name the module name
##' @param quiet verbose flag
##' @return null
biips_load_module <- function(name, quiet = FALSE) {
  stopifnot(is.character(name), length(name) == 1, nchar(name) > 0)
  stopifnot(is.logical(quiet), length(quiet) == 1)
  
  ok <- RBiips("load_module", name)
  if (!ok) {
    stop("module ", name, " not found\n")
  } else if (!quiet) {
    message("module", name, "loaded\n")
  }
  return(invisible())
}



##' Add the corresponding R function to the bugs model
##' @param name name of the new  function
##' @param n_param number of arguments of the new function
##' @param fun_dim R function returning a vector containg arguments sizes
##' @param fun_eval R function computing the result of function
##' @param fun_check_param R function checking the arguments 
##' @param fun_is_discrete R function telling is new function is discrete wrt its arguments
##' @export
biips_add_function <- function(name, n_param, fun_dim, fun_eval, fun_check_param, 
  fun_is_discrete) {
  stopifnot(is.character(name), length(name) == 1, nchar(name) > 0)
  RBiips("add_function", name, n_param, fun_dim, fun_eval, fun_check_param, fun_is_discrete)
  return(invisible())
}



##' Add the corresponding R distribution to the bugs model
##' @param name name of the new distribution
##' @param n_param number of arguments of the new distribution
##' @param fun_dim R function returning a vector containg arguments sizes
##' @param fun_sample R function sampling from the distribution
##' @param fun_check_param R function checking the arguments 
##' @param fun_is_discrete R function telling is new function is discrete wrt its arguments
##' @export
biips_add_distribution <- function(name, n_param, fun_dim, fun_sample, fun_check_param, 
  fun_is_discrete) {
  stopifnot(is.character(name), length(name) == 1, nchar(name) > 0)
  RBiips("add_distribution", name, n_param, fun_dim, fun_sample, fun_check_param, 
    fun_is_discrete)
  return(invisible())
} 
