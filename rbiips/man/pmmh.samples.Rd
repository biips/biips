\name{pmmh.samples}
\alias{pmmh.samples}
\title{
Generate Particle Marginal Metropolis-Hastings samples
}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
}
\usage{
pmmh.samples(model, variable.names, n.iter, thin = 1, rw.sd, n.part, max.fail = 0, \dots)
}
\arguments{
  \item{model}{
  a biips model object
}
  \item{variable.names}{a character vector giving the names of variables to be monitored
}
  \item{n.iter}{
  number of iterations of the Markov chain to run
}
  \item{thin}{thinning interval for monitors
}
  \item{rw.sd}{list of the desired standard deviations of the random walk for each monitored variable
}
  \item{n.part}{
number of particles
}
  \item{max.fail}{
  maximum number of failures allowed
}
  \item{\dots}{additional arguments to be passed to the SMC algorithm
}
}
\details{
The \code{pmmh.samples} function creates monitors for the given variables, runs the model for \code{n.iter} iterations and returns the monitored samples.
}
\value{
%%  ~Describe the value returned
%%  If it is a LIST, use
%%  \item{comp1 }{Description of 'comp1'}
%%  \item{comp2 }{Description of 'comp2'}
%% ...
A list of \code{\link[rjags:mcarray.object]{mcarray}} objects, with one element for each element of the \code{variable.names} argument.
}
\author{
Adrien Todeschini, Francois Caron
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
\code{\link{biips.model}}, \code{\link{pimh.samples}}, \code{\link{smc.samples}}
}
\examples{
##---- Should be DIRECTLY executable !! ----
##-- ==>  Define data, use random,
##--	or do  help(data=index)  for the standard data sets.
}
\keyword{models }
