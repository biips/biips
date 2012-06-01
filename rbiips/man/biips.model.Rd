\name{biips.model}
\alias{biips.model}
\title{
Create a biips model object
}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
\code{biips.model} is used to create an object representing a
   Bayesian graphical model, specified with a BUGS language description
   of the prior distribution, and a set of data.
}
\usage{
biips.model(file, data = sys.frame(sys.parent()), sample.data = TRUE,
            data.rng.seed, quiet = FALSE)
}
\arguments{
  \item{file}{
a file containing a description of the model in the
    BiiPS/JAGS dialect of the BUGS language.
    
    Alternatively, \code{file} can be a readable text-mode connection, or a complete URL.
}
  \item{data}{
a list or environment containing the data. Any numeric
    objects in \code{data} corresponding to node arrays used in
    \code{file} are taken to represent the values of observed nodes
    in the model
}
  \item{sample.data}{
  logical flag. If \code{FALSE} then the \code{data} block of the model will be ignored.
}
  \item{data.rng.seed}{
optional integer used as the seed for the random number generator of the data
generation.
}
  \item{quiet}{if \code{TRUE} then messages generated during compilation will be suppressed.
}
}
\details{
%%  ~~ If necessary, more details than the description above ~~
}
\value{
%%  ~Describe the value returned
%%  If it is a LIST, use
%%  \item{comp1 }{Description of 'comp1'}
%%  \item{comp2 }{Description of 'comp2'}
%% ...
\code{biips.model} returns an object inheriting from class \code{biips}
  which can be used to generate dependent samples from the posterior
  distribution of the parameters
  
  An object of class \code{biips} is a list of functions that share a
  common environment. The functions can be used to query information 
  on the model.
  \item{ptr()}{Returns an external pointer to an object created by
    the BiiPS library.}
  \item{data()}{Returns a list containing the data that defines
    the observed nodes in the model.}
  \item{model()}{Returns a character vector containing the BUGS-language
    representation of the model.}
  \item{dot(file)}{Writes a description of the graph in dot language in
	\code{file}.}
  \item{nodes(type, observed)}{Returns a \code{data.frame} containing information on all
	the nodes of the graph: rank in the topological sort, node id, variable
	name or formula, type, observed. After calling \code{build.sampler} or
	any algorithm running smc sampler, the \code{data.frame} will also
	contain a column indicating the node sampler used for unobserved
	stochastic nodes.}
  \item{recompile()}{Recompiles the model using the original data set.}
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
\keyword{ models }
\keyword{ graphs }

