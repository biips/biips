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
#  \file     biips-object.R
#  \brief    R functions manipulating biips objects
#
#  \author   $LastChangedBy$
#  \date     $LastChangedDate$
#  \version  $LastChangedRevision$
#  Id:       $Id$
#


##' Manipulate biips model objects
##' 
##' A \code{biips} object represents a Bayesian graphical model described using
##' the BUGS language.
##' 
##' The \code{variable.names} function returns a character vector of names of
##' node arrays used in the model.
##' 
##' @name biips-object 
##' @aliases biips.object build.sampler build.sampler.biips pmmh.init
##' pmmh.init.biips pmmh.update pmmh.update.biips pimh.update pimh.update.biips
##' variable.names.biips is.biips
##' @param object a biips model object
##' @param proposal
##' @param variable.names a character vector giving the names of variables to
##' be monitored
##' @param inits
##' @param n.part number of particles
##' @param rs.thres threshold on the ESS criterion to control the resampling
##' step
##' @param rs.type a string indicating the resampling algorithm used
##' @param inits.rng.seed optional integer used as the seed for the random
##' number generator of the initial values generation.
##' @param n.iter number of iterations of the Markov chain to run
##' @param max.fail maximum number of failures allowed
##' @param ... additional arguments
##' @return %% ~Describe the value returned
##' @author Adrien Todeschini, Francois Caron
##' @keywords models
##' @examples
##' 
##' ## Should be DIRECTLY executable !! 
##' ##-- ==>  Define data, use random,
##' ##--  or do  help(data=index)  for the standard data sets.
##' 
NULL


##' Try to parse string of form "a" or "a[n,p:q,r]" where "a" is a
##' variable name and n,p,q,r are integers
##' @param varname string containing the name of the variable to sparse
parse_varname <- function(varname) {
  
  
  v <- try(parse(text=varname, n=1), silent=TRUE)
  if (!is.expression(v) || length(v) != 1)
    return(NULL)
  
  v <- v[[1]]
  if (is.name(v)) {
    ## Full node array requested
    return(list(name=deparse(v)))
  }
  else if (is.call(v) && identical(deparse(v[[1]]), "[") && length(v) > 2) {
    ## Subset requested
    ndim <- length(v) - 2
    lower <- upper <- numeric(ndim)
    if (any(nchar(sapply(v, deparse)) == 0)) {
      ## We have to catch empty indices here or they will cause trouble
      ## below
      return(NULL)
    }
    for (i in 1:ndim) {
      index <- v[[i+2]]
      if (is.numeric(index)) {
        ## Single index
        lower[i] <- upper[i] <- index
      }
      else if (is.call(index) && length(index) == 3 &&
                 identical(deparse(index[[1]]), ":") &&
                 is.numeric(index[[2]]) && is.numeric(index[[3]]))
      {
        ## Index range
        lower[i] <- index[[2]]
        upper[i] <- index[[3]]
      }
      else return(NULL)
    }
    if (any(upper < lower))
      return (NULL)
    return(list(name = deparse(v[[2]]), lower=lower, upper=upper))
  }
  return(NULL)
}


parse_varnames <- function(varnames)
{
  names <- character(length(varnames))
  lower <- upper <- vector("list", length(varnames))
  for (i in seq(along=varnames)) {
    y <- parse_varname(varnames[i])
    if (is.null(y)) {
      stop("Invalid variable subset ", varnames[i])
    }
    names[i] <- y$name
    if (!is.null(y$lower)) {
      lower[[i]] <- y$lower
    }
    if (!is.null(y$upper)) {
      upper[[i]] <- y$upper
    }
  }
  return(list(names=names, lower=lower, upper=upper))
}


##' @export
is.biips <- function(object)
{
  return(class(object) == "biips")
}

##' @S3method print biips
print.biips <- function(x,...)
{
  cat("BiiPS model:\n\n")
  
  model <- x$model()
  for (i in 1:length(model)) {
    cat(model[i],"\n",sep="")
  }
  
  data <- x$data()
  full <- !sapply(lapply(data, is.na), any)
  if (any(full)) {
    cat("\n\nFully observed variables:\n", names(data)[full], "\n")
  }
  part <- !full & !sapply(lapply(data, is.na), all)
  if (any(part)) {
    cat("\nPartially observed variables:\n", names(data)[part], "\n")
  }
}


monitor <- function(object, ...)
  UseMethod("monitor")

##' @S3method monitor biips
monitor.biips <- function(object, variable.names, type)
{
  if (!is.character(variable.names) || length(variable.names) == 0)
    stop("variable.names must be a character vector")
    
  pn <- parse_varnames(variable.names)
  
  type <- match.arg(type, c("f", "s", "b"), several.ok = TRUE)
  if ("f" %in% type) {
    .Call("set_filter_monitors", object$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
  }
  if ("s" %in% type) {
    .Call("set_gen_tree_smooth_monitors", object$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
  }
  if ("b" %in% type) {
    .Call("set_backward_smooth_monitors", object$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
  }
  invisible(NULL)
}


is_monitored <- function(object, ...)
  UseMethod("is_monitored")

##' @S3method is_monitored biips
is_monitored.biips <- function(object, variable.names, type, check.released=TRUE)
{
  if (!is.character(variable.names) || length(variable.names) == 0)
    stop("variable.names must be a character vector")
  
  pn <- parse_varnames(variable.names)
  
  type <- match.arg(type, c("f", "s", "b"))
  if (type == "f") {
    ok <- .Call("is_filter_monitored", object$ptr(), pn$names, pn$lower, pn$upper, check.released, PACKAGE="RBiips")
  }
  else if (type == "s") {
    ok <- .Call("is_gen_tree_smooth_monitored", object$ptr(), pn$names, pn$lower, pn$upper, check.released, PACKAGE="RBiips")
  }
  else if (type == "b") {
    ok <- .Call("is_backward_smooth_monitored", object$ptr(), pn$names, pn$lower, pn$upper, check.released, PACKAGE="RBiips")
  }
  return(ok)
}


clear_monitors <- function(object, ...)
  UseMethod("clear_monitors")

##' @S3method clear_monitors biips
clear_monitors.biips <- function(object, type, release.only=FALSE)
{
  type <- match.arg(type, c("f", "s", "b"), several.ok = TRUE)
  if ("f" %in% type) {
    .Call("clear_filter_monitors", object$ptr(), release.only, PACKAGE="RBiips")
  }
  if ("s" %in% type) {
    .Call("clear_gen_tree_smooth_monitors", object$ptr(), release.only, PACKAGE="RBiips")
  }
  if ("b" %in% type) {
    .Call("clear_backward_smooth_monitors", object$ptr(), release.only, PACKAGE="RBiips")
  }
  invisible(NULL)
}


##' @export
build_sampler <- function(object, ...)
  UseMethod("build_sampler")


##' @S3method build_sampler biips
build_sampler.biips <- function(object, proposal= "auto", ...)
{
  if (!is.character(proposal) || length(proposal)!=1) {
    stop("Invalid proposal argument")
  }
  proposal <- match.arg(proposal, c("auto",
                                  "prior"))
    
  ## build smc sampler
  .Call("build_smc_sampler", object$ptr(), proposal=="prior", PACKAGE="RBiips")
  
  invisible(NULL)
}


##' @importFrom stats variable.names
##' @S3method variable.names biips
variable.names.biips <- function(object, ...) {
    .Call("get_variable_names", object$ptr(), PACKAGE="RBiips")
}
