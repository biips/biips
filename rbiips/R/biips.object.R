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

monitor.biips <- function(obj, variable.names, type)
{
  if (!is.biips(obj))
    stop("Invalid BiiPS model.")
  
  if (!is.character(variable.names) || length(variable.names) == 0)
    stop("variable.names must be a character vector")
    
  pn <- parse.varnames(variable.names)
  
  type <- match.arg(type, c("f", "s", "b"), several.ok = TRUE)
  if ("f" %in% type) {
    .Call("set_filter_monitors", obj$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
  }
  if ("s" %in% type) {
    .Call("set_smooth_tree_monitors", obj$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
  }
  if ("b" %in% type) {
    .Call("set_smooth_monitors", obj$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
  }
  invisible(NULL)
}


is.monitored.biips <- function(obj, variable.names, type, check.released=TRUE)
{
  if (!is.biips(obj))
    stop("Invalid BiiPS model.")
  
  if (!is.character(variable.names) || length(variable.names) == 0)
    stop("variable.names must be a character vector")
  
  pn <- parse.varnames(variable.names)
  
  type <- match.arg(type, c("f", "s", "b"))
  if (type == "f") {
    ok <- .Call("is_filter_monitored", obj$ptr(), pn$names, pn$lower, pn$upper, check.released, PACKAGE="RBiips")
  }
  else if (type == "s") {
    ok <- .Call("is_smooth_tree_monitored", obj$ptr(), pn$names, pn$lower, pn$upper, check.released, PACKAGE="RBiips")
  }
  else if (type == "b") {
    ok <- .Call("is_smooth_monitored", obj$ptr(), pn$names, pn$lower, pn$upper, check.released, PACKAGE="RBiips")
  }
  return(ok)
}


clear.monitors.biips <- function(obj, type, release.only=FALSE)
{
  if (!is.biips(obj))
    stop("Invalid BiiPS model.")
  
  type <- match.arg(type, c("f", "s", "b"), several.ok = TRUE)
  if ("f" %in% type) {
    .Call("clear_filter_monitors", obj$ptr(), release.only, PACKAGE="RBiips")
  }
  if ("s" %in% type) {
    .Call("clear_smooth_tree_monitors", obj$ptr(), release.only, PACKAGE="RBiips")
  }
  if ("b" %in% type) {
    .Call("clear_smooth_monitors", obj$ptr(), release.only, PACKAGE="RBiips")
  }
  invisible(NULL)
}


build.sampler <- function(object, ...)
  UseMethod("build.sampler")


build.sampler.biips <- function(object, proposal= "auto", ...)
{
  if (!is.biips(object))
    stop("Invalid BiiPS model")
      
  if (!is.character(proposal) || !is.atomic(proposal)) {
    stop("Invalid proposal argument")
  }
  proposal <- match.arg(proposal, c("auto",
                                  "prior"))
    
  ## build smc sampler
  .Call("build_smc_sampler", object$ptr(), proposal=="prior", PACKAGE="RBiips")
  
  invisible(NULL)
}


run.smc.forward <- function(obj, n.part, rs.thres = 0.5, rs.type = "stratified",
                            smc.rng.seed)
{
  if (!is.biips(obj))
    stop("Invalid BiiPS model")
      
  if (!is.numeric(n.part) || !is.atomic(n.part) || n.part < 1) {
    stop("Invalid n.part argument")
  } 
  if (!is.numeric(rs.thres) || !is.atomic(rs.thres) || rs.thres < 0) {
    stop("Invalid rs.thres argument")
  }
  if (!is.character(rs.type) || !is.atomic(rs.type)) {
    stop("Invalid rs.type argument")
  }
  rs.type <- match.arg(rs.type, c("multinomial",
                                  "residual",
                                  "stratified",
                                  "systematic"))
  if (missing(smc.rng.seed)) {
    smc.rng.seed <- runif(1, 0, as.integer(Sys.time()));
  }
  if (!is.numeric(smc.rng.seed) || !is.atomic(smc.rng.seed) || smc.rng.seed < 0) {
    stop("Invalid smc.rng.seed argument")
  }
  
  ## build smc sampler
  if (!.Call("is_sampler_built", obj$ptr(), PACKAGE="RBiips")) {
    .Call("build_smc_sampler", obj$ptr(), FALSE, PACKAGE="RBiips")
  }
  
  ## run smc sampler
  ok <- .Call("run_smc_sampler", obj$ptr(), as.integer(n.part), as.integer(smc.rng.seed), rs.thres, rs.type, PACKAGE="RBiips")
  
  invisible(ok)
}


init.pmmh <- function(object, ...)
  UseMethod("init.pmmh")


init.pmmh.biips <- function(object, variable.names, inits=list(),
                            n.part, rs.thres=0.5, rs.type="stratified",
                            inits.rng.seed, ...)
{ 
  if (!is.biips(object))
    stop("Invalid BiiPS model")
  
  ## check variable.names
  if (!is.character(variable.names))
    stop("Invalid variable.names argument")
  pn <- parse.varnames(variable.names)
  
  ## check inits
  if (!is.list(inits))
    stop("Invalid inits argument")
  if (length(inits) > 0) {
    for (var in variable.names) {
      if (!var %in% names(inits))
        warning(paste("Missing init value for variable", var))
    }    
  }
  
  ## check inits.rng.seed parameter
  if (missing(inits.rng.seed)) {
    inits.rng.seed <- runif(1, 0, as.integer(Sys.time()));
  }
  if (!is.numeric(inits.rng.seed) || !is.atomic(inits.rng.seed) || inits.rng.seed < 0) {
    stop("Invalid inits.rng.seed argument")
  }
  
  ## stop biips verbosity
  verb <- .Call("verbosity", 0, PACKAGE="RBiips")
  on.exit(.Call("verbosity", verb, PACKAGE="RBiips"))
  
  ## make init sample
  sample <- list()
  for (v in seq(along=variable.names)) {
    var <- variable.names[[v]]
    if (var %in% names(inits)) {
      # take init value in inits param
      if(!.Call("change_data", object$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]],
                inits[[var]], TRUE, PACKAGE="RBiips"))
        stop(paste("data change failed: invalid initial value for variable", var))
      sample[[var]] <- inits[[var]]
    } else {
      # or sample init value
      data <- object$.data.sync()
      if (var %in% names(data))
        sample[[var]] <- data[[var]]
      else
        sample[[var]] <- .Call("sample_data", object$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]],
                               inits.rng.seed, PACKAGE="RBiips")
    }
  }
  
  ## log prior density
  log.prior <- list()
  for (v in seq(along=variable.names))
  {
    var <- variable.names[[v]]
    log.p <- .Call("get_log_prior_density", object$ptr(),
                   pn$names[[v]], pn$lower[[v]], pn$upper[[v]], PACKAGE="RBiips")
    
    if (is.na(log.p)) {
      stop("get log prior density: node is not stochastic.")
    }
    
    if (is.infinite(log.p) && log.p<0) 
      stop(paste("get log prior density: invalid initial values for variable", variable.names[[v]]))
    
    log.prior[[var]] <- log.p
  }
  
  ## build smc sampler
  if (!.Call("is_sampler_built", object$ptr(), PACKAGE="RBiips")) {
    .Call("build_smc_sampler", object$ptr(), FALSE, PACKAGE="RBiips")
  }
  
  ## get log normalizing constant
  if (!.Call("is_smc_sampler_at_end", object$ptr(), PACKAGE="RBiips")) {
    ## run smc
    .Call("message", "Initializing PMMH", PACKAGE="RBiips")
    if (!run.smc.forward(object, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type))
      stop("run smc forward sampler: invalid initial values.")
  }
  log.marg.like <- .Call("get_log_norm_const", object$ptr(), PACKAGE="RBiips")
  
  ans <- list(sample=sample, log.prior=log.prior, log.marg.like=log.marg.like)
  
  invisible(ans)
}


one.update.pmmh.biips <- function(obj, variable.names, pn,
                                  sample, log.prior, log.marg.like,
                                  n.part, ...)
{
  n.fail <- 0
  
  accept.rate <- list()
  
  ## Metropolis-Hastings iteration
  ##------------------------------
  for (v in seq(along=variable.names)) {
    var <- variable.names[[v]]
    accept.rate[[var]] <- 0
    
    ## Random walk proposal
    prop <- list()
    l <- length(sample[[var]])
    prop[[var]] <- rnorm(l, sample[[var]], obj$.rw.step())
    dim(prop[[var]]) <- dim(sample[[var]])
    
    ## change model data
    ok <- .Call("change_data", obj$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]],
                prop[[var]], TRUE, PACKAGE="RBiips")
    
    if (!ok) {
      accepted <- FALSE
      n.fail <- n.fail+1
      warning(paste("Failure changing data. proposal:", prop))
      ## reset previous value
      prop[[var]] <- sample[[var]]
      if(!.Call("change_data", obj$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]],
                sample[[var]], TRUE, PACKAGE="RBiips"))
        stop("can not reset previous data.")
      next
    }
    
    log.prior.prop <- .Call("get_log_prior_density", obj$ptr(),
                            pn$names[[v]], pn$lower[[v]], pn$upper[[v]], PACKAGE="RBiips")
    
    if (is.na(log.prior.prop)) {
      stop("get log prior density: node is not stochastic.")
    }
      
    if (is.infinite(log.prior.prop) && log.prior.prop<0) {
      accepted <- FALSE
      ## reset previous value
      if(!.Call("change_data", obj$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]],
                sample[[var]], TRUE, PACKAGE="RBiips"))
        stop("can not reset previous data.")
      next
    }
    
    ## run smc sampler
    ok <- run.smc.forward(obj, n.part=n.part, ...)
    
    if (!ok) {
      accepted <- FALSE
      n.fail <- n.fail+1
      warning(paste("Failure running smc forward sampler. proposal:", prop))
      ## reset previous value
      if(!.Call("change_data", obj$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]],
                sample[[var]], TRUE, PACKAGE="RBiips"))
        stop("can not reset previous data.")
      next
    }
    
    ## Acceptance rate
    log.marg.like.prop <- .Call("get_log_norm_const", obj$ptr(), PACKAGE="RBiips")
    log.ar <- log.marg.like.prop - log.marg.like + log.prior.prop - log.prior[[var]]
    
    accept.rate[[var]] <- exp(log.ar)
    
    ## Accept/Reject step
    accepted <- (runif(1) < accept.rate[[var]])
    if (accepted) {
      sample[[var]] <- prop[[var]]
      log.prior[[var]] <- log.prior.prop
      log.marg.like <- log.marg.like.prop
    } else {
      ## reset previous value
      if(!.Call("change_data", obj$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]],
                sample[[var]], TRUE, PACKAGE="RBiips"))
        stop("can not reset previous data.")
    }
    
    # rescale random walk step
    obj$.rw.rescale(accept.rate[[var]])
  }
  
  
  ans <- list(sample=sample, log.prior=log.prior, log.marg.like=log.marg.like,
              accept.rate=accept.rate, accepted=accepted, n.fail=n.fail)
  invisible(ans)
}


update.pmmh <- function(object, ...)
  UseMethod("update.pmmh")


update.pmmh.biips <- function(object, variable.names, n.iter, 
                              n.part, max.fail = 0, ...)
{  
  if (!is.biips(object))
    stop("Invalid BiiPS model")
  
  if (!is.numeric(n.iter) || !is.atomic(n.iter) || n.iter < 1)
    stop("Invalid n.iter argument")
  n.iter <- as.integer(n.iter)
  
  ## stop biips verbosity
  verb <- .Call("verbosity", 0, PACKAGE="RBiips")
  on.exit(.Call("verbosity", verb, PACKAGE="RBiips"))
  
  ## initialize
  out <- init.pmmh.biips(object, variable.names=variable.names, n.part=n.part, ...)
  sample <- out$sample
  log.prior <- out$log.prior
  log.marg.like <- out$log.marg.like
  
  pn <- parse.varnames(variable.names)
  
  
  ## reset data to sample on exit
  on.exit(
      if (n.iter > 0 && !accepted) {
        .Call("set_log_norm_const", object$ptr(), log.marg.like, PACKAGE="RBiips")
      }, add=TRUE)
  
  .Call("message", paste("Updating PMMH with", n.part, "particles"), PACKAGE="RBiips")
  ## progress bar
  symbol <- ifelse(object$.rw.adapt(), '+', '*')
  bar <- .Call("progress_bar", n.iter, symbol, "iterations", PACKAGE="RBiips")
  
  n.fail <- 0
  accepted <- TRUE
  
  accept.rate <- list()
  for (var in variable.names) {
    accept.rate[[var]] <- vector(length=n.iter)
  }
  
  ## Metropolis-Hastings iterations
  ##-------------------------------
  for(i in 1:n.iter) {
    out <- one.update.pmmh.biips(object, variable.names=variable.names, pn=pn, 
                                 sample=sample, log.prior=log.prior, log.marg.like=log.marg.like,
                                 n.part=n.part, ...)
    sample <- out$sample
    log.prior <- out$log.prior
    log.marg.like <- out$log.marg.like
    accepted <- out$accepted
    n.fail <- n.fail + out$n.fail
    
    for (var in variable.names) {
      accept.rate[[var]][i] <- out$accept.rate[[var]]
    }
    
    ## advance progress bar
    .Call("advance_progress_bar", bar, 1, PACKAGE="RBiips")
    
    if (n.fail > max.fail) {
      stop(paste("Number of failures exceeds max.fail:", n.fail, "failures."))
    }
  }
  
  mean.ar <- list()
  for (var in variable.names) {
    mean.ar[[var]] <- mean(accept.rate[[var]])
  }
  
  ans <- list(mean.ar=mean.ar, n.fail=n.fail)
  invisible(ans)
}


init.pimh.biips <- function(obj, variable.names,
                            n.part, rs.thres=0.5, rs.type="stratified")
{
  monitored <- is.monitored.biips(obj, variable.names, "s", FALSE)
  if (!monitored) {
    ## monitor variables
    monitor.biips(obj, variable.names, type="s")
  }
  
  built <- .Call("is_sampler_built", obj$ptr(), PACKAGE="RBiips")
  if (!built) {
    ## build smc sampler
    .Call("build_smc_sampler", obj$ptr(), FALSE, PACKAGE="RBiips")
  }
  
  atend <- .Call("is_smc_sampler_at_end", obj$ptr(), PACKAGE="RBiips")
  ## get log normalizing constant
  if (!monitored || !built || !atend) {
    ## run smc sampler
    .Call("message", "Initializing PIMH", PACKAGE="RBiips")
    run.smc.forward(obj, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type)
  }
  log.marg.like <- .Call("get_log_norm_const", obj$ptr(), PACKAGE="RBiips")
  
  ## get sampled value
  sampled.value <- .Call("get_sampled_smooth_tree_particle", obj$ptr(), PACKAGE="RBiips")
  if (length(sampled.value)==0) {
    ## sample one particle
    rng.seed <- runif(1, 0, as.integer(Sys.time()))
    .Call("sample_smooth_tree_particle", obj$ptr(), as.integer(rng.seed), PACKAGE="RBiips")
    
    sampled.value <- .Call("get_sampled_smooth_tree_particle", obj$ptr(), PACKAGE="RBiips")
  }
  sample <- list()
  for (var in variable.names) {
    sample[[var]] <- sampled.value[[var]]
  }
  
  ans <- list(sample=sample, log.marg.like=log.marg.like)
  invisible(ans)
}


one.update.pimh.biips <- function(obj, variable.names,
                                  n.part, rs.thres=0.5, rs.type="stratified",
                                  sample, log.marg.like)
{
  ## SMC
  run.smc.forward(obj, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type)

  ## Acceptance rate
  log.marg.like.prop <- .Call("get_log_norm_const", obj$ptr(), PACKAGE="RBiips")
  log.ar <- log.marg.like.prop - log.marg.like

  ## Accept/Reject step
  accepted <- (runif(1) < exp(log.ar))
  if (accepted) {
    log.marg.like <- log.marg.like.prop
    
    ## sample one particle
    rng.seed <- runif(1, 0, as.integer(Sys.time()))
    .Call("sample_smooth_tree_particle", obj$ptr(), as.integer(rng.seed), PACKAGE="RBiips")
    
    ## get sampled value
    sampled.value <- .Call("get_sampled_smooth_tree_particle", obj$ptr(), PACKAGE="RBiips")
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
  
  if (!is.numeric(n.iter) || !is.atomic(n.iter) || n.iter < 1)
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
  
  clear.monitors.biips(object, type="s", TRUE)
  
  ## reset log norm const and sampled value
  if (n.iter > 0 && !accepted) {
    .Call("set_log_norm_const", object$ptr(), log.marg.like, PACKAGE="RBiips")
    .Call("set_sampled_smooth_tree_particle", object$ptr(), sample, PACKAGE="RBiips")
  }
  
  invisible(NULL)
}


variable.names.biips <- function(object, ...) {
    .Call("get_variable_names", object$ptr(), PACKAGE="RBiips")
}
