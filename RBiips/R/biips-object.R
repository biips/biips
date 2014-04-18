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
##' @aliases biips-object build.sampler build.sampler.biips pmmh.init
##' pmmh.init.biips pmmh.update pmmh.update.biips pimh.update pimh.update.biips
##' variable.names.biips is.biips
##' @param object a biips model object
##' @param proposal
##' @param variable.names a character vector giving the names of variables to
##' be monitored
##' @param inits
##' @param n_part number of particles
##' @param rs_thres threshold on the ESS criterion to control the resampling
##' step
##' @param rs_type a string indicating the resampling algorithm used
##' @param n_iter number of iterations of the Markov chain to run
##' @param max_fail maximum number of failures allowed
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



##' @export
is.biips <- function(object)
{
  return(class(object) == "biips")
}

##' @S3method print biips
print.biips <- function(x, ...)
{
  stopifnot(is.biips(x))
  
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


##' @importFrom stats variable.names
##' @S3method variable.names biips
variable.names.biips <- function(object, ...) {
  stopifnot(is.biips(object))
  
  var_names <- RBiips("get_variable_names",  object$ptr())
  
  return(var_names)
}


##' @export
build_sampler <- function(object, ...)
  UseMethod("build_sampler")


##' Assigns a sampler to each node of the graph
##' @param object a biips model object
##' @param proposal string. Keyword defining the type of proposal desired.
##'              Possible values are "auto" and "prior". "auto" selects the best sampler
##'              among available ones automatically. "prior" forces asignment of the prior
##'              sampler to every node. "prior" switches off lots of instructions and can
##'              speed up the startup of the SMC for big models.
##'              default is "auto"
##' @param ... unused
##' @S3method build_sampler biips
build_sampler.biips <- function(object, proposal = "auto", ...)
{
  stopifnot(is.biips(object))
  proposal <- match.arg(proposal, c("auto", "prior"))
  
  ## build smc sampler
  RBiips("build_smc_sampler",  object$ptr(), proposal=="prior")
  
  invisible()
}


monitor <- function(object, ...)
  UseMethod("monitor")

##' @S3method monitor biips
monitor.biips <- function(object, var_names, type="s", ...)
{
  stopifnot(is.biips(object))
  stopifnot(is.character(var_names), length(var_names)>0)
  type <- check_type(type, several.ok = TRUE)
  
  pn <- parse_varnames(var_names)

  for (t in type) {
    switch(t,
           f = RBiips("set_filter_monitors",  object$ptr(), pn$names, pn$lower, pn$upper),
           s = RBiips("set_gen_tree_smooth_monitors",  object$ptr(), pn$names, pn$lower, pn$upper),
           b = RBiips("set_backward_smooth_monitors",  object$ptr(), pn$names, pn$lower, pn$upper))
  }
  
  invisible()
}


is_monitored <- function(object, ...)
  UseMethod("is_monitored")

##' @S3method is_monitored biips
is_monitored.biips <- function(object, var_names, type="s", check_released=TRUE)
{
  stopifnot(is.biips(object))
  stopifnot(is.character(var_names), length(var_names)>0)
  type <- check_type(type, several.ok = FALSE)
  stopifnot(is.logical(check_released), length(check_released)==1)
  
  pn <- parse_varnames(var_names)
  
  ok <- switch(type,
           f = RBiips("is_filter_monitored",  object$ptr(), pn$names, pn$lower, pn$upper, check_released),
           s = RBiips("is_gen_tree_smooth_monitored",  object$ptr(), pn$names, pn$lower, pn$upper, check_released),
           b = RBiips("is_backward_smooth_monitored",  object$ptr(), pn$names, pn$lower, pn$upper, check_released))

  return(ok)
}


clear_monitors <- function(object, ...)
  UseMethod("clear_monitors")

##' Clears some monitors  
##' @param console      id of the current console
##' @param type        character vector with  'f' - forward
##'                                       's' - smoothing
##'                                       'b' - backward smoothing
##' @param release_only boolean flag to indicate what kind of clearing has 
##'                   to be done
##' @S3method clear_monitors biips
clear_monitors.biips <- function(object, type="fsb", release_only=FALSE, ...)
{
  stopifnot(is.biips(object))
  type <- check_type(type, several.ok = TRUE)
  stopifnot(is.logical(release_only), length(release_only)==1)
  
  for (t in type) {
    switch(t,
           f = RBiips("clear_filter_monitors",  object$ptr(), release_only),
           s = RBiips("clear_gen_tree_smooth_monitors",  object$ptr(), release_only),
           b = RBiips("clear_backward_smooth_monitors",  object$ptr(), release_only))
  }
  
  invisible()
}


