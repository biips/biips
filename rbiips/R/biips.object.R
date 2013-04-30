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
#  \file     biips.object.R
#  \brief    R functions manipulating biips objects
#
#  \author   $LastChangedBy$
#  \date     $LastChangedDate$
#  \version  $LastChangedRevision$
#  Id:       $Id$
#

monitor.biips <- function(object, variable.names, type)
{
  if (!is.biips(object))
    stop("Invalid BiiPS model.")
  
  if (!is.character(variable.names) || length(variable.names) == 0)
    stop("variable.names must be a character vector")
    
  pn <- parse.varnames(variable.names)
  
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


is.monitored.biips <- function(object, variable.names, type, check.released=TRUE)
{
  if (!is.biips(object))
    stop("Invalid BiiPS model.")
  
  if (!is.character(variable.names) || length(variable.names) == 0)
    stop("variable.names must be a character vector")
  
  pn <- parse.varnames(variable.names)
  
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


clear.monitors.biips <- function(object, type, release.only=FALSE)
{
  if (!is.biips(object))
    stop("Invalid BiiPS model.")
  
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


build.sampler <- function(object, ...)
  UseMethod("build.sampler")


build.sampler.biips <- function(object, proposal= "auto", ...)
{
  if (!is.biips(object))
    stop("Invalid BiiPS model")
      
  if (!is.character(proposal) || length(proposal)!=1) {
    stop("Invalid proposal argument")
  }
  proposal <- match.arg(proposal, c("auto",
                                  "prior"))
    
  ## build smc sampler
  .Call("build_smc_sampler", object$ptr(), proposal=="prior", PACKAGE="RBiips")
  
  invisible(NULL)
}


run.smc.forward <- function(object, n.part, rs.thres = 0.5, rs.type = "stratified",
                            smc.rng.seed)
{
  if (!is.biips(object))
    stop("Invalid BiiPS model")
      
  if (!is.numeric(n.part) || length(n.part)!=1 || n.part < 1) {
    stop("Invalid n.part argument")
  } 
  if (!is.numeric(rs.thres) || length(rs.thres)!=1 || rs.thres < 0) {
    stop("Invalid rs.thres argument")
  }
  if (!is.character(rs.type) || length(rs.type)!=1) {
    stop("Invalid rs.type argument")
  }
  rs.type <- match.arg(rs.type, c("multinomial",
                                  "residual",
                                  "stratified",
                                  "systematic"))
  if (missing(smc.rng.seed)) {
    smc.rng.seed <- runif(1, 0, as.integer(Sys.time()));
  }
  if (!is.numeric(smc.rng.seed) || length(smc.rng.seed)!=1 || smc.rng.seed < 0) {
    stop("Invalid smc.rng.seed argument")
  }
  
  ## build smc sampler
  if (!.Call("is_sampler_built", object$ptr(), PACKAGE="RBiips")) {
    .Call("build_smc_sampler", object$ptr(), FALSE, PACKAGE="RBiips")
  }
  
  ## run smc sampler
  ok <- .Call("run_smc_sampler", object$ptr(), as.integer(n.part),
              as.integer(smc.rng.seed), rs.thres, rs.type, PACKAGE="RBiips")
  
  invisible(ok)
}

init.pimh.biips <- function(object, variable.names,
                            n.part, rs.thres=0.5, rs.type="stratified")
{
  monitored <- is.monitored.biips(object, variable.names, "s", FALSE)
  if (!monitored) {
    ## monitor variables
    monitor.biips(object, variable.names, type="s")
  }
  
  if (!.Call("is_sampler_built", object$ptr(), PACKAGE="RBiips")) {
    ## build smc sampler
    .Call("build_smc_sampler", object$ptr(), FALSE, PACKAGE="RBiips")
  }
  
  atend <- .Call("is_smc_sampler_at_end", object$ptr(), PACKAGE="RBiips")
  ## get log normalizing constant
  if (!monitored || !atend) {
    ## run smc sampler
    .Call("message", "Initializing PIMH", PACKAGE="RBiips")
    run.smc.forward(object, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type)
  }
  log.marg.like <- .Call("get_log_norm_const", object$ptr(), PACKAGE="RBiips")
  
  ## get sampled value
  sampled.value <- .Call("get_sampled_gen_tree_smooth_particle", object$ptr(), PACKAGE="RBiips")
  if (length(sampled.value)==0) {
    ## sample one particle
    rng.seed <- runif(1, 0, as.integer(Sys.time()))
    .Call("sample_gen_tree_smooth_particle", object$ptr(), as.integer(rng.seed), PACKAGE="RBiips")
    
    sampled.value <- .Call("get_sampled_gen_tree_smooth_particle", object$ptr(), PACKAGE="RBiips")
  }
  sample <- list()
  for (var in variable.names) {
    sample[[var]] <- sampled.value[[var]]
  }
  
  ans <- list(sample=sample, log.marg.like=log.marg.like)
  invisible(ans)
}


one.update.pimh.biips <- function(object, variable.names,
                                  n.part, rs.thres=0.5, rs.type="stratified",
                                  sample, log.marg.like)
{
  ## SMC
  run.smc.forward(object, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type)

  ## Acceptance rate
  log.marg.like.prop <- .Call("get_log_norm_const", object$ptr(), PACKAGE="RBiips")
  log.ar <- log.marg.like.prop - log.marg.like

  ## Accept/Reject step
  accepted <- (runif(1) < exp(log.ar))
  if (accepted) {
    log.marg.like <- log.marg.like.prop
    
    ## sample one particle
    rng.seed <- runif(1, 0, as.integer(Sys.time()))
    .Call("sample_gen_tree_smooth_particle", object$ptr(), as.integer(rng.seed), PACKAGE="RBiips")
    
    ## get sampled value
    sampled.value <- .Call("get_sampled_gen_tree_smooth_particle", object$ptr(), PACKAGE="RBiips")
    for (var in variable.names) {
      sample[[var]] <- sampled.value[[var]]
    }
  }
  
  ans <- list(sample=sample, log.marg.like=log.marg.like, accepted=accepted)
  invisible(ans)
}


update.pimh <- function(object, ...)
  UseMethod("update.pimh")


update.pimh.biips <- function(object, variable.names, n.iter,
                       n.part, ...)
{
  if (!is.biips(object))
    stop("Invalid BiiPS model")
  
  ## check variable.names
  if (!is.character(variable.names))
    stop("Invalid variable.names argument")
  parse.varnames(variable.names)
  
  if (!is.numeric(n.iter) || length(n.iter)!=1 || n.iter < 1)
    stop("Invalid n.iter argument")
  n.iter <- as.integer(n.iter)
  
  ## stop biips verbosity
  verb <- .Call("verbosity", 0, PACKAGE="RBiips")
  on.exit(.Call("verbosity", verb, PACKAGE="RBiips"))
  
  ## initialize
  out <- init.pimh.biips(object, variable.names=variable.names,
                         n.part=n.part, ...)
  sample <- out$sample
  log.marg.like <- out$log.marg.like
  
  .Call("message", paste("Updating PIMH with", n.part, "particles"), PACKAGE="RBiips")
  ## progress bar
  bar <- .Call("progress_bar", n.iter, '*', "iterations", PACKAGE="RBiips")
  
  n.samples <- 0
  
  ## Independant Metropolis-Hastings iterations
  ##-------------------------------------------
  for(i in 1:n.iter) {
    out <- one.update.pimh.biips(object, variable.names=variable.names, 
                                 sample=sample, log.marg.like=log.marg.like,
                                 n.part=n.part, ...)
    sample <- out$sample
    log.marg.like <- out$log.marg.like
    accepted <- out$accepted
  
    ## advance progress bar
    .Call("advance_progress_bar", bar, 1, PACKAGE="RBiips")
  }
  
  ## release monitors memory
  clear.monitors.biips(object, type="s", TRUE)
  
  ## reset log norm const and sampled value
  if (n.iter > 0 && !accepted) {
    .Call("set_log_norm_const", object$ptr(), log.marg.like, PACKAGE="RBiips")
    .Call("set_sampled_gen_tree_smooth_particle", object$ptr(), sample, PACKAGE="RBiips")
  }
  
  invisible(NULL)
}


variable.names.biips <- function(object, ...) {
    .Call("get_variable_names", object$ptr(), PACKAGE="RBiips")
}
