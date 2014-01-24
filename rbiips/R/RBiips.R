#                                               -*- R -*-
#
#  RBiips package for GNU R is an interface to BiiPS C++ libraries for
#  Bayesian inference with interacting Particle Systems.
#  Copyright (C) Inria, 2012
#  Authors: Adrien Todeschini, Francois Caron
#  
#  RBiips is derived software based on:
#  BiiPS, Copyright (C) Inria, 2012
#  rjags, Copyright (C) Martyn Plummer, 2002-2010
#  Rcpp, Copyright (C) Dirk Eddelbuettel and Romain Francois, 2009-2011
#
#  This file is part of RBiips.
#
#  RBiips is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#
#  \file     RBiips.R
#  \brief    main RBiips functions
#
#  \author   $LastChangedBy$
#  \date     $LastChangedDate$
#  \version  $LastChangedRevision$
#  Id:       $Id$
#
## COPY: Adapted from rjags module file: jags.R


##' load the corresponding module into the Biips environment
##' @param name the module name
##' @param quiet verbose flag
##' @return null
load.biips.module <- function(name, quiet=FALSE)
{    
  if (!is.character(name) || length(name)>1)
    stop("invalid name")
  
  ok <- .Call("load_module", name, PACKAGE="RBiips")
  if (!ok) {
    stop("module ", name, " not found\n")
  }
  else if (!quiet) {
    cat("module", name, "loaded\n")
  }
  invisible(NULL)
}



# TODO
is_legal_biips_vname <- function(name) {
  TRUE
}


data_preprocess <- function(data) { # , varnames) {
  # COPY: Adapted from rstan
  # Preprocess the data (list or env) to list for stan
  # 
  # Args:
  #  data: A list or an environment
  #  of objects 
  #   * stop if no-name lists; duplicate names  
  #   * stop if the objects given name is not found  
  #   * remove NULL, non-numeric elements 
  
  if (is.environment(data)) {
    data <- as.list(data) 
  } else if (is.list(data)) {
    v <- names(data)
    if (is.null(v)) 
      stop("data must be a named list")
    
    if (any(duplicated(v))) {
      stop("duplicated names in data list: ", 
           paste(v[duplicated(v)], collapse = " "))
    }
  } else {
    stop("data must be a list or an environment") 
  } 
  
  names <- names(data) 
  for (n in names) { 
    if (!is_legal_biips_vname(n))
      stop('data with name ', n, " is not allowed in Biips")
  } 
  
  data <- lapply(data, 
                 FUN = function(x) {
                   ## change data.frame to array 
                   if (is.data.frame(x)) { x <- data.matrix(x) }
                   
                   # remove those not numeric data 
                   if (!is.numeric(x)) { x <- NULL }
                   
                   return(x)
                 })
  
  data[!sapply(data, is.null)] 
}


mklist <- function(names, env = parent.frame()) { 
  # COPY: Adapted from rstan
  # Make a list using names 
  # Args: 
  #   names: character strings of names of objects 
  #   env: the environment to look for objects with names
  # Note: we use inherits = TRUE when calling mget 
  d <- mget(names, env, ifnotfound = NA, inherits = TRUE, mode="numeric") 
  n <- which(is.na(d)) 
  if (length(n) > 0) {
    stop("objects ", paste("'", names[n], "'", collapse = ', ', sep = ''), " not found")
  } 
  d 
}






##' Create a biips model object
##' 
##' \code{biips.model} is used to create an object representing a Bayesian
##' graphical model, specified with a BUGS language description of the prior
##' distribution, and a set of data.
##' 
##' @param file a file containing a description of the model in the BiiPS/JAGS
##' dialect of the BUGS language.
##' 
##' Alternatively, \code{file} can be a readable text-mode connection, or a
##' complete URL.
##' @param data a list or environment containing the data. Any numeric objects
##' in \code{data} corresponding to node arrays used in \code{file} are taken
##' to represent the values of observed nodes in the model
##' @param sample.data logical flag. If \code{FALSE} then the \code{data} block
##' of the model will be ignored.
##' @param data.rng.seed optional integer used as the seed for the random
##' number generator of the data generation.
##' @param quiet if \code{TRUE} then messages generated during compilation will
##' be suppressed.
##' @return \code{biips.model} returns an object inheriting from class
##' \code{biips} which can be used to generate dependent samples from the
##' posterior distribution of the parameters
##' 
##' An object of class \code{biips} is a list of functions that share a common
##' environment. The functions can be used to query information on the model.
##' \item{ptr()}{Returns an external pointer to an object created by the BiiPS
##' library.} \item{data()}{Returns a list containing the data that defines the
##' observed nodes in the model.} \item{model()}{Returns a character vector
##' containing the BUGS-language representation of the model.}
##' \item{dot(file)}{Writes a description of the graph in dot language in
##' \code{file}.} \item{nodes(type, observed)}{Returns a \code{data.frame}
##' containing information on all the nodes of the graph: rank in the
##' topological sort, node id, variable name or formula, type, observed. After
##' calling \code{build.sampler} or any algorithm running smc sampler, the
##' \code{data.frame} will also contain a column indicating the node sampler
##' used for unobserved stochastic nodes.} \item{recompile()}{Recompiles the
##' model using the original data set.}
##' @author Adrien Todeschini, Francois Caron
##' @keywords models graphs
##' @export
##' @examples
##' 
##' ## Should be DIRECTLY executable !! 
##' ##-- ==>  Define data, use random,
##' ##--	or do  help(data=index)  for the standard data sets.
##' 
biips.model <- function(file, data=parent.frame(), sample.data=TRUE, data.rng.seed, quiet = FALSE)
{
  if (missing(file)) {
    stop("Model file name missing")
  }
  if (is.character(file)) {
    f <- try(file(file, "rt"))
    if (inherits(f, "try-error")) {
      stop("Cannot open model file \"", file, "\"")
    }
    close(f)
    model.code <- readLines(file, warn = FALSE)
    modfile <- file
  }
  else if (!inherits(file, "connection")) {
    stop("'file' must be a character string or connection")
  } else {
    model.code <- readLines(file, warn = FALSE)
    modfile <- tempfile()
    writeLines(model.code, modfile)
  }
  
  if(!is.logical(sample.data) || length(sample.data)>1) {
    stop("Invalid sample.data argument.");
  }
  if(missing("data.rng.seed")) {
    data.rng.seed <- runif(1, 0, as.integer(Sys.time()));
  }
  else if(!is.numeric(data.rng.seed) || length(data.rng.seed)!=1) {
    stop("Invalid data.rng.seed argument.");
  }
  
  if (quiet) {
    verb <- .Call("verbosity", 0, PACKAGE="RBiips")
    on.exit(.Call("verbosity", verb, PACKAGE="RBiips"), add = TRUE)
  }
  
  # check data before compiling model, which typically takes more time
  if (is.character(data)) data <- mklist(data) 
  if (length(data) > 0) data <- data_preprocess(data)
  
  # make console and check model
  p <- .Call("make_console", PACKAGE="RBiips")
  .Call("check_model", p, modfile, PACKAGE="RBiips")
  
  # discard unused data
  varnames <- .Call("get_variable_names", p, PACKAGE="RBiips")
  
  v <- names(data)
  relevant.variables <- v %in% varnames
  data <- data[relevant.variables]
  unused.variables <- setdiff(v, varnames)
  if (length(unused.variables)>0)
    warning("Unused variables in data:", paste(unused.variables ,sep=","))
  
  # compile model
  .Call("compile_model", p, data, sample.data, as.integer(data.rng.seed), PACKAGE="RBiips")
  
  # data after possible sampling (from "data" block in the BUGS language model)
  model.data <- .Call("get_data", p, PACKAGE = "RBiips")
  
  # local initial parameters of the algorithm of adaptation
  # of the step of random walk in the PMMH algorithm
  rw <- list(rescale=TRUE,
             learn=FALSE,
             niter=1,
             pmean=0,
             lstep=c(),
             beta=.05,
             alpha=1,
             targetprob=0.234, # Target acceptance probability. The default seems to
             # be a fairly robust optimal value. Comes from Rosenthal 2009, Examples 
             # of Adaptives MCMC, p 16. In one-dimensional case, we take 0.44 which
             # is optimal in certain settings.
             povertarget=FALSE,
             ncrosstarget=10, #The value ncrosstarget controls the reduction in the step size when rescale is
             #called. There is no reason to give it an initial value of zero. In
             #fact this is a poor choice since the  step size would be immediately
             #halved. We start with a value of 10 so the first change in step size
             #is 10%.
             dim=list(),
             d=c(),
             count=0,
             buffer=c(),
             buff.count=c(),
             mean=c(),
             cov=c()
  )
  
  model <- list("ptr" = function() {p},
                "model" = function() {model.code},
                "dot" = function(file) {
                  .Call("print_graphviz", p, file, PACKAGE="RBiips")
                  invisible(NULL)
                },
                "data" = function() {
                  model.data
                },
                ".data.sync" = function() {
                  .Call("get_data", p, PACKAGE="RBiips")
                },
                "nodes" = function(type, observed) {
                  sorted.nodes <- data.frame(.Call("get_sorted_nodes", p, PACKAGE="RBiips"))
                  
                  if(.Call("is_sampler_built", p, PACKAGE="RBiips")) {
                    samplers <- data.frame(.Call("get_node_samplers", p, PACKAGE="RBiips"))
                    sorted.nodes <- cbind(sorted.nodes, samplers)
                  }
                  if(!missing(type)) {
                    if(!is.character(type) || length(type)!=1) {
                      stop("Invalid type argument.");
                    }
                    type <- match.arg(type, c("Constant", "Logical", "Stochastic"))
                    sorted.nodes <- sorted.nodes[sorted.nodes["type"]==type,]
                  }
                  if(!missing(observed)) {
                    if(!is.logical(observed) || length(observed)!=1) {
                      stop("Invalid observed argument.");
                    }
                    sorted.nodes <- sorted.nodes[sorted.nodes["observed"]==observed,]
                  }
                  return(sorted.nodes);
                },
                "recompile" = function() {
                  ## Clear the console
                  .Call("clear_console", p, PACKAGE="RBiips")
                  p <<- .Call("make_console", PACKAGE="RBiips")
                  ## Write the model to a temporary file so we can re-read it
                  mf <- tempfile()
                  writeLines(model.code, mf)
                  .Call("check_model", p, mf, PACKAGE="RBiips")
                  unlink(mf)
                  ## Re-compile
                  .Call("compile_model", p, data,
                        FALSE, as.integer(data.rng.seed), PACKAGE="RBiips")
                  invisible(NULL)
                },
                ## store the dimensions of the variables
                ".rw.init" = function(sample) {
                  ## FIXME
                  
                  ## store the dimensions of the variables
                  sampledim <<- lapply(sample, dim)
                  dim.null <- sapply(sampledim, is.null)
                  if (any(dim.null)) {
                    len <- lapply(sample, length)
                    sampledim[d.null] <<- len[d.null]
                  }
                  if (length(rw$dim)!=0) {
                    stopifnot(all(mapply(identical,rw$dim,sampledim)))
                  }
                  else {
                    rw$dim <<- sampledim
                    
                    rw$niter <<- 1
                    rw$pmean <<- 0
                    rw$povertarget <<- FALSE
                    rw$ncrosstarget <<- 10
                    
                    rw$d <<- sum(sapply(rw$dim, FUN=sum))
                    rw$targetprob <<- if (rw$d==1) 0.44 else 0.234
                    rw$lstep <<- log(0.1/sqrt(rw$d))
                    
                    # clear learnt covariance matrix
                    rw$count <<- 0
                    rw$buffer <<- c()
                    rw$buff.count <<- c()
                    rw$mean <<- c()
                    rw$cov <<- c()
                  }
                  
                  invisible(NULL)
                },
                ## assign rw step
                ".rw.step" = function(rw.step) {
                  ## check values and dimensions
                  for (n in names(rw.step)) {
                    if (any(is.na(rw.step[[n]])))
                      stop("Missing (NA) rw.step value for variable:", n)
                    if (any(is.infinite(rw.step[[n]])))
                      stop("Infinite rw.step value for variable:", n)
                    if (any(is.nan(rw.step[[n]])))
                      stop("NaN rw.step value for variable:", n)
                    if (any(rw.step[[n]]<=0))
                      stop("Negative or zero rw.step value for variable:", n)
                    if (length(rw.step[[n]])==1) {
                      rw.step[[n]] <- array(rw.step[[n]], dim=rw$dim[[n]])
                    } else {
                      if (is.null(dim(rw.step[[n]])))
                        dim(rw.step[[n]]) <- length(rw.step[[n]])
                      if (length(rw.step[[n]]) != length(rw$dim[[n]]) || any(dim(rw.step[[n]]) != rw$dim[[n]]))
                        stop("Incorrect rw.step dimension for variable:", n)
                    }
                  }
                  
                  # concatenate all log values in a vector
                  # always in the order of rw$dim
                  rw$lstep <<- c()
                  for (n in names(rw$dim)) {
                    rw$lstep <<- c(rw$lstep, log(rw.step[[n]]))
                  }
                  
                  # clear rescale parameters
                  rw$niter <<- 1
                  rw$pmean <<- 0
                  rw$povertarget <<- FALSE
                  rw$ncrosstarget <<- 10
                  
                  # clear learnt covariance matrix
                  rw$count <<- 0
                  rw$buffer <<- c()
                  rw$buff.count <<- c()
                  rw$mean <<- c()
                  rw$cov <<- c()
                    
                  invisible(NULL)
                },
                ".rw.adapt" = function() {
                  return(rw$rescale|rw$learn)
                },
                ".rw.check.adapt" = function() {
                  if (rw$pmean==0 || rw$pmean==1) {
                    return(FALSE)
                  }
                  # The distance, on a logistic scale, between pmean
                  # and the target acceptance probability.
                  dist <- abs(log(rw$pmean/(1-rw$pmean))
                              - log(rw$targetprob/(1-rw$targetprob)))
                  return (dist < 0.5)
                },
                ".rw.adapt.off" = function() {
                  rw$rescale <<- FALSE
                  rw$learn <<- FALSE
                  invisible(NULL)
                },
                ".rw.rescale.off" = function() {
                  rw$rescale <<- FALSE
                  invisible(NULL)
                },
                ".rw.learn.off" = function() {
                  rw$learn <<- FALSE
                  invisible(NULL)
                },
                ".rw.learn.on" = function() {
                  rw$rescale <<- FALSE
                  rw$learn <<- TRUE
                  rw$niter <<- 1
                  invisible(NULL)
                },
                ".rw.proposal" = function(sample) {
                  # concatenate all variables in a vector
                  # always in the order of rw$dim
                  sample_vec <- c()
                  for (n in names(rw$dim)) {
                    sample_vec <- c(sample_vec, sample[[n]])
                  }
                  stopifnot(length(sample_vec) == rw$d)
                  
                  if (length(rw$cov) == 0 || (rw$learn && (runif(1)<1-rw$beta))) {
                    # modification with diagonal covariance
                    prop_vec <- sample_vec + exp(rw$lstep) * rnorm(rw$d)
                  } else {
                    eps <- 1/rw$niter
                    cov_chol <- t(chol((1-eps) * rw$cov + eps * diag(0.1^2, nrow=rw$d)))
                    prop_vec <- sample_vec + 2.38/sqrt(rw$d) * cov_chol %*% rnorm(rw$d)
                  }
                  rw$niter <<- rw$niter+1
                  
                  # rearrange vectorized parameter to list of arrays with 
                  # correct dimensions
                  prop <- list()
                  from <- 1
                  for (n in names(rw$dim)) {
                    to <- from+prod(rw$dim[[n]])-1
                    prop[[n]] <- array(prop_vec[from:to], dim=rw$dim[[n]])
                    from <- to+1
                  }
                  invisible(prop)
                },
                ".rw.rescale" = function(p, type='d') {
                  # We keep a weighted mean estimate of the mean acceptance probability
                  # with the weights in favour of more recent iterations
                  p <- min(p, 1.0)
                  rw$pmean <<- rw$pmean + 1*(p-rw$pmean)/rw$niter
                  
                  if (!rw$rescale) {
                    return(NULL)
                  }
                  
                  type = match.arg(type, c("dureau", "plummer"))
                  if (type=="dureau") {
                    rw$lstep <<- rw$lstep + rw$alpha^(rw$niter)*(rw$pmean-rw$targetprob)
                  }
                  else if (type=="plummer") {
                    # The step size is adapted to achieve the
                    # target acceptance rate using a noisy gradient algorithm.
                    rw$lstep <<- rw$lstep + (p-rw$targetprob)/rw$ncrosstarget
                  }
                  
                  if ((p>rw$targetprob) != rw$povertarget) {
                    rw$povertarget <<- !rw$povertarget
                    rw$ncrosstarget <<- rw$ncrosstarget+1
                  }
                  
                  invisible(NULL)
                },
                ".rw.learn.cov" = function(sample, accepted) {
                  if (!rw$learn) {
                    return(NULL)
                  }
                  if (accepted || is.null(rw$buffer)) {
                    # concatenate all variables in a vector
                    # always in the order of rw$dim
                    sample_vec <- c()
                    for (n in names(rw$dim)) {
                      sample_vec <- c(sample_vec, sample[[n]])
                    }
                    # push sample back in buffer
                    rw$buffer <<- rbind(rw$buffer, sample_vec, deparse.level=0)
                    rw$buff.count <<- c(rw$buff.count, 1)
                  } else {
                    # increment last sample counter
                    n <- nrow(rw$buffer)
                    rw$buff.count[n] <<- rw$buff.count[n]+1
                  }
                  
                  naccept <- nrow(rw$buffer)
                  d <- ncol(rw$buffer)
                  m <- sum(rw$buff.count)
                  
                  # update mean and covariance
                  if (naccept == 2*d) {
                    # empirical mean and covariance of the buffer
                    mean_buff <- colSums(rw$buffer*rw$buff.count/m)
                    cov_buff <- t(rw$buffer)%*% (rw$buffer*rw$buff.count/m) - outer(mean_buff, mean_buff)
                    
                    if (length(rw$mean) == 0) {
                      rw$mean <<- mean_buff
                      rw$cov <<- cov_buff
                    } else {
                      # update empirical mean and covariance
                      n <- rw$count
                      N <- n+m
                      rw$mean <<- n/N*rw$mean + m/N*mean_buff
                      diff <- mean_buff - rw$mean
                      rw$cov <<- n/N*rw$cov + m/N*cov_buff + n*m/N^2 * diff %*% t(diff)
                      
                      rw$count <<- N
                    }
                    rw$count <<- rw$count+m
                    
                    # clear buffer
                    rw$buffer <<- c()
                    rw$buff.count <<- c()
                  }
                  
                  invisible(NULL)
                },
                ".get.rw.step"=function(){exp(rw$lstep)}
                )
  class(model) <- "biips"
  
  # TRICK: We return functions in model list that use variables of the parent
  # environment (ie the currrent function environment).
  # This specific R trick allows to read and write persistent variables, 
  # surrogating a class with private members and their modifiers.
  return(model)
}




##' Add the corresponding R function to the bugs model
##' @param name of the new  function
##' @param number of arguments of the new function
##' @param R function returning a vector containg arguments sizes
##' @param R function computing the result of function
##' @param R function checking the arguments 
##' @param R function telling is new function is discrete wrt its arguments
##' @export
biips.add.function <- function(name, nb.args, fundim, funeval, funcheckparam, funisdiscrete)
{    
  if (!is.character(name) || length(name)>1)
  {
    stop("invalid name")
  }
  ok <- .Call("add_function", name, nb.args, fundim , funeval, funcheckparam, funisdiscrete, PACKAGE="RBiips")
}
