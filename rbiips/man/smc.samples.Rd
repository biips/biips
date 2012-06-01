\name{smc.samples}
\alias{smc.samples}
\title{
Generate conditional samples
}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
Function to extract random weighted samples, aka. particles, from the
conditional distribution of the parameters of a \code{biips} model.
}
\usage{
smc.samples(obj, variable.names, type=c("filtering", "smoothing", "backward.smoothing"),
            n.part, backward = FALSE,
            rs.thres = 0.5, rs.type = "stratified", \dots)
}
\arguments{
  \item{obj}{
a biips model object
}
  \item{variable.names}{
a character vector giving the names of variables to be monitored
}
  \item{type}{
a character vector giving the types of monitoring desired.
}
  \item{n.part}{
number of particles
}
  \item{backward}{
logical flag. If \code{TRUE} then a backward smoothing algorithm will be run after the forward sampling step
}
  \item{rs.thres}{
  threshold on the ESS criterion to control the resampling step
}
  \item{rs.type}{
  a string indicating the resampling algorithm used
}
  \item{\dots}{
  additional arguments
}
}
\details{
%%  ~~ If necessary, more details than the description above ~~
  The \code{smc.samples} function creates monitors for the given types and variables, runs the SMC algorithm for \code{n.part} particles and returns the monitored samples.
  
  Argument \code{type} specifies the type of particle values that will be stored for each monitored node. Possible values are: \code{"filtering"}, \code{"smoothing"}, \code{"backward.smoothing"}
}
\value{
A list of \code{\link[=particles.list.object]{particles.list}} objects, with one element for each 
  element of the \code{variable.names} argument.
  
A \code{particles.list} object is a list of \code{\link[=particles.object]{particles}} object, with one
element for each type of monitoring \code{type} argument.
}
\author{
Adrien Todeschini, Francois Caron
}
\note{
By default, the seed of the random number generation is chosen randomly.
However for the sake of repatability, the optional \code{smc.rng.seed}
integer argument can be used to fix it.
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
\code{\link{biips.model}}, \code{\link{pmmh.samples}}, \code{\link{pimh.samples}}
}
\examples{
##---- Should be DIRECTLY executable !! ----
##-- ==>  Define data, use random,
##--	or do  help(data=index)  for the standard data sets.
}
\keyword{models}
