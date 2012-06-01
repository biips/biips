\name{biips.object}
\alias{biips.object}
\alias{build.sampler}
\alias{build.sampler.biips}
\alias{init.pmmh}
\alias{init.pmmh.biips}
\alias{update.pmmh}
\alias{update.pmmh.biips}
\alias{update.pimh}
\alias{update.pimh.biips}
\alias{variable.names.biips}
\alias{is.biips}
\title{
Manipulate biips model objects
}
\description{
  A \code{biips} object represents a Bayesian graphical model
  described using the BUGS language. 

}
\usage{
\method{build.sampler}{biips}(object, proposal = "auto", \dots)
\method{init.pmmh}{biips}(object, variable.names, inits = list(), n.part,
                          rs.thres = 0.5, rs.type = "stratified",
                          inits.rng.seed, \dots)
\method{update.pmmh}{biips}(object, variable.names, n.iter, rw.sd, n.part,
                            max.fail = 0, \dots)
\method{update.pimh}{biips}(object, variable.names, n.iter, n.part, \dots)
\method{variable.names}{biips}(object, \dots)
is.biips(object)
}
\arguments{
  \item{object}{
  a biips model object
}
  \item{proposal}{
}
  \item{variable.names}{a character vector giving the names of variables to be monitored
}
  \item{inits}{
}
  \item{n.part}{number of particles
}
  \item{rs.thres}{threshold on the ESS criterion to control the resampling step
}
  \item{rs.type}{a string indicating the resampling algorithm used
}
  \item{inits.rng.seed}{optional integer used as the seed for the random number generator of the initial values
generation.
}
  \item{n.iter}{number of iterations of the Markov chain to run
}
  \item{rw.sd}{list of the desired standard deviations of the random walk for each variable
}
  \item{max.fail}{maximum number of failures allowed
}
  \item{\dots}{additional arguments
}
}
\details{
  The \code{variable.names} function returns a character vector of
  names of node arrays used in the model.
}
\value{
%%  ~Describe the value returned
%%  If it is a LIST, use
%%  \item{comp1 }{Description of 'comp1'}
%%  \item{comp2 }{Description of 'comp2'}
%% ...
}
\author{
Adrien Todeschini, Francois Caron
}
%% ~Make other sections like Warning with \section{Warning }{....} ~

\examples{
##---- Should be DIRECTLY executable !! ----
##-- ==>  Define data, use random,
##--	or do  help(data=index)  for the standard data sets.
}
\keyword{models}

