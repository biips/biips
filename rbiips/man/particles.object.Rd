\name{particles.object}
\alias{particles.object}
\alias{particles.list.object}
\alias{diagnostic}
\alias{diagnostic.particles}
\alias{diagnostic.particles.list}
\alias{summary.particles}
\alias{summary.particles.list}
\alias{density.particles}
\alias{density.particles.list}
\alias{plot.particles}
\alias{plot.particles.list}
\title{
Objects for representing SMC output
}
\description{
A \code{particles} object is used by the \code{smc.samples} function to 
represent SMC output from a BiiPS model. It is an array with named dimensions, 
for which the dimension \dQuote{particles} has a special status.

A \code{particles.list} object is a list of \code{particles} objects with
different types of monitoring for the same variable.

Functions applying to \code{particles} objects apply identically to
\code{particles.list} objects by a call to each element of the list.
}
\usage{
\method{diagnostic}{particles}(object, ess.thres = 30, \dots)
\method{diagnostic}{particles.list}(object, \dots)

\method{summary}{particles}(object, fun, probs = c(0.25, 0.5, 0.75), \dots)
\method{summary}{particles.list}(object, \dots)

\method{density}{particles}(x, bw = "nrd0", adjust = 1, subset, \dots)
\method{density}{particles.list}(x, bw = "nrd0", adjust = 1, subset, \dots)

\method{plot}{particles}(x, fun = c("mean", "mode"), probs = c(0.25, 0.5, 0.75), \dots)
\method{plot}{particles.list}(x, fun = c("mean", "mode"), probs = c(0.25, 0.5, 0.75), \dots)
}
\arguments{
  \item{object, x}{
  a \code{particles} or \code{particles.list} object
}
  \item{fun}{
  a character vector indicating the functions to be used to generate summary
  statistics
}
  \item{probs}{
  a numerical vector containing the desired quantile probabilities
}
  \item{bw}{
  the smoothing bandwidth to be used. See \code{\link{density}} for more details
}
  \item{adjust}{
  the bandwidth used is actually \code{adjust*bw}. This makes it easy to specify values like \sQuote{half the default} bandwidth.
}
  \item{subset}{
}
  \item{ess.thres}{
}
  \item{\dots}{additional arguments to be passed to the default methods
}
}
\details{
%%  ~~ If necessary, more details than the description above ~~
}
\value{
The \code{summary} method for \code{particles} objects applies the given function
to the array, marginalizing the \dQuote{particles} dimension.
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

\seealso{
\link{density}
}
\examples{
##---- Should be DIRECTLY executable !! ----
##-- ==>  Define data, use random,
##--	or do  help(data=index)  for the standard data sets.
}
\keyword{models}

