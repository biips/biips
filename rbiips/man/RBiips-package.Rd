\name{RBiips-package}
\alias{RBiips-package}
\alias{RBiips}
\docType{package}
\title{
Bayesian inference with interacting Particle Systems
}
\description{
Interface with C++ BiiPS software for analysing Bayesian graphical models using particle methods, aka Sequential Monte Carlo (SMC) methods.

}
\details{
\tabular{ll}{
Package: \tab RBiips\cr
Type: \tab Package\cr
Version: \tab @Biips_VERSION@\cr
Date: \tab @Biips_DATE@\cr
License: \tab @Biips_LICENSE@\cr
LazyLoad: \tab yes\cr
}
Compile your model using biips.model function. Then compute estimates using standard SMC algorithm (using smc.samples), or Particle MCMC methods (using pmmh.samples or pimh.samples functions).
}
\author{
RBiips development is supported by the team ALEA at Inria Bordeaux Sud-Ouest.

Individual contributors:
Adrien Todeschini (Design and development)
François Caron (Probability expertise and specifications)

Maintainer: biips-support <biips-support@gforge.inria.fr>
}
\references{
~~ Literature or other references for background information ~~
}
~~ Optionally other standard keywords, one per line, from file ~~
~~ KEYWORDS in the R documentation directory ~~
\keyword{ package }
\seealso{
~~ Optional links to other man pages, e.g. ~~
~~ \code{\link[<pkg>:<pkg>-package]{<pkg>}} ~~
}
\examples{
%% ~~ simple examples of the most important functions ~~
}
