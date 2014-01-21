##' Bayesian inference with interacting Particle Systems
##' 
##' \emph{RBiips} is an interface with the C++ \href{http://alea.bordeaux.inria.fr/biips}{BiiPS}
##' libraries for analysing Bayesian graphical models using advanced particle
##' methods.
##' 
##' BiiPS is a general software for Bayesian inference with interacting particle 
##' systems, a.k.a. sequential Monte Carlo (SMC) methods.
##' It aims at popularizing the use of these methods to non-statistician 
##' researchers and students, thanks to its automated "black box" inference engine.
##' It borrows from the BUGS/JAGS software, widely used in Bayesian statistics, 
##' the statistical modeling with graphical models and the language associated 
##' with their descriptions.
##' Unlike MCMC methods used by BUGS/JAGS, SMC methods are more adapted to 
##' dynamic problems (tracking, signal filtering, etc).
##' 
##' Typical usage of \emph{RBiips} consits in: \enumerate{ \item
##' compiling your model with \code{\link{biips.model}} function \item
##' optionally analyzing sensitivity to parameters with
##' \code{\link{smc.sensitivity}} \item generate samples from the conditional
##' distributions \itemize{ \item using standard SMC algorithm with
##' \code{\link{smc.samples}} function \item using particle MCMC methods with
##' \code{\link{pmmh.samples}} or \code{\link{pimh.samples}} functions } \item
##' analysing output from \code{\link[particles.object]{particles}} and
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
##' @examples
##' 
##' %% ~~ simple examples of the most important functions ~~
##' 
NULL
