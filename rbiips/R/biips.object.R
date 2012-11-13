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
    .Call("set_smooth_tree_monitors", object$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
  }
  if ("b" %in% type) {
    .Call("set_smooth_monitors", object$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
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
    ok <- .Call("is_smooth_tree_monitored", object$ptr(), pn$names, pn$lower, pn$upper, check.released, PACKAGE="RBiips")
  }
  else if (type == "b") {
    ok <- .Call("is_smooth_monitored", object$ptr(), pn$names, pn$lower, pn$upper, check.released, PACKAGE="RBiips")
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
    .Call("clear_smooth_tree_monitors", object$ptr(), release.only, PACKAGE="RBiips")
  }
  if ("b" %in% type) {
    .Call("clear_smooth_monitors", object$ptr(), release.only, PACKAGE="RBiips")
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


run.smc.forward <- function(object, n.part, rs.thres = 0.5, rs.type = "stratified",
                            smc.rng.seed)
{
  if (!is.biips(object))
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
  if (!.Call("is_sampler_built", object$ptr(), PACKAGE="RBiips")) {
    .Call("build_smc_sampler", object$ptr(), FALSE, PACKAGE="RBiips")
  }
  
  ## run smc sampler
  ok <- .Call("run_smc_sampler", object$ptr(), as.integer(n.part),
              as.integer(smc.rng.seed), rs.thres, rs.type, PACKAGE="RBiips")
  
  invisible(ok)
}


init.pmmh <- function(object, ...)
  UseMethod("init.pmmh")


init.pmmh.biips <- function(object, param.names, latent.names=c(), inits=list(),
                            n.part, rs.thres=0.5, rs.type="stratified",
                            inits.rng.seed, quiet=FALSE,...)
{ 
  if (!is.biips(object))
    stop("Invalid BiiPS model")
  
  ## check param.names
  if (!is.character(param.names))
    stop("Invalid param.names argument")
  if (any(duplicated(param.names))) {
    stop("duplicated names in param.names: ", 
         paste(param.names[duplicated(param.names)], collapse = " "))
  }
  pn.param <- parse.varnames(param.names)
  
  ## check latent.names
  if (length(latent.names) > 0) {
    if (!is.character(latent.names))
      stop("Invalid latent.names argument")
    if (any(duplicated(latent.names))) {
      stop("duplicated names in latent.names: ", 
           paste(latent.names[duplicated(latent.names)], collapse = " "))
    }
    parse.varnames(latent.names)
  }
  
  ## check inits
  init.values <- list()
  if (length(inits) > 0) {
    if (is.function(inits)) {
      init.values <- inits()
      if(!is.list(init.values))
        stop("inits function must return a list.")
    } else if (is.list(inits)) {
      init.values <- inits
    } else {
      stop("inits must be a list or a function.")
    }
    
    inames <- names(init.values)
    
    if(is.null(inames)) ## case unnamed list
    {
      if (length(init.values) != length(param.names))
        stop("Invalid inits argument.")
      names(init.values) <- param.names
    } else  ## case named list
    {
      if (any(nchar(inames) == 0))
        stop("inits must be a named list.")
      
      if (any(duplicated(inames)))
        stop("duplicated names in inits list:", 
             paste(inames[duplicated(inames)], sep=","))
      
      ## Warn missing init values
      miss.inits <- !param.names %in% inames
      if (any(miss.inits)) {
        warning("Missing initial values for variable:",
                paste(param.names[miss.inits], sep=","))
      }
      ## Strip unkown variables from initial values, but give a warning
      unknown.inits <- !inames %in% param.names
      if (any(unknown.inits)) {
        warning("Ignoring unkown variable in initial values:",
                paste(inames[unknown.inits], sep=","))
        init.values <- init.values[!unknown.inits]
        inames <- names(init.values)
      }
      ## Strip null initial values, but give a warning
      null.inits <- sapply(init.values, is.null)
      if (any(null.inits)) {
        warning("Ignoring null initial values supplied for variable:",
                paste(inames[null.inits], sep=","))
        init.values <- init.values[!null.inits]
        inames <- names(init.values)
      }
    }
    
    nonnum.inits <- !sapply(init.values, is.numeric)
    if (any(nonnum.inits))
      stop("non numeric initial values supplied for variable:",
           paste(inames[nonnum.inits], sep=","))
  }
  
  
  ## check inits.rng.seed
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
  for (v in seq(along=param.names)) {
    var <- param.names[[v]]
    if (var %in% names(init.values)) {
      # take init value in inits param
      if(!.Call("change_data", object$ptr(),
                pn.param$names[[v]], pn.param$lower[[v]], pn.param$upper[[v]],
                init.values[[var]], TRUE, PACKAGE="RBiips"))
        stop("data change failed: invalid initial value for variable ", var)
      sample[[var]] <- init.values[[var]]
    } else {
      # or sample init value
      data <- object$.data.sync()
      if (var %in% names(data))
        sample[[var]] <- data[[var]]
      else
        sample[[var]] <- .Call("sample_data", object$ptr(),
                               pn.param$names[[v]], pn.param$lower[[v]], pn.param$upper[[v]],
                               inits.rng.seed, PACKAGE="RBiips")
    }
  }
  
  ## check NA
  has.na <- sapply(sample, function(x){any(is.na(x))})
  if (any(has.na)) {
    stop("PMMH initial values have NA: ", paste(names(sample[has.na]), "=", sample[has.na], sep="", collapse="; "), "\n")
  }
  
  ## initialize rw
  object$.rw.dim(sample)
  
  ## log prior density
  log.prior <- 0
  for (v in seq(along=param.names))
  {
    var <- param.names[[v]]
    log.p <- .Call("get_log_prior_density", object$ptr(),
                   pn.param$names[[v]], pn.param$lower[[v]], pn.param$upper[[v]], PACKAGE="RBiips")
    
    if (is.na(log.p)) {
      stop("Failed to get log prior density: node ", var, " is not stochastic.")
    }
    if (is.nan(log.p)) {
      stop("Failed to get log prior density: NaN.")
    }
    if (log.p == Inf) {
      stop("Failed to get log prior density: +Inf.")
    }
    if (log.p == -Inf) 
      stop("Negative infinite log prior density: invalid initial value: ", var, "=", sample[[var]])
    
    log.prior <- log.prior + log.p
  }
  
  run.smc.flag <- FALSE
  ## check latent variable is monitored
  if (length(latent.names) > 0) {
    monitored <- is.monitored.biips(object, latent.names, "s", FALSE)
    if (!monitored) {
      ## monitor variables
      monitor.biips(object, latent.names, type="s")
      run.smc.flag <- TRUE
    }
  }
  
  ## build smc sampler
  if (!.Call("is_sampler_built", object$ptr(), PACKAGE="RBiips")) {
    .Call("build_smc_sampler", object$ptr(), FALSE, PACKAGE="RBiips")
  }
  
  if (!run.smc.flag)
    run.smc.flag <- !.Call("is_smc_sampler_at_end", object$ptr(), PACKAGE="RBiips")
  
  ## get log normalizing constant
  if (run.smc.flag) {
    ## run smc
    if (!quiet)
      .Call("message", "Initializing PMMH", PACKAGE="RBiips")
    if (!run.smc.forward(object, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type))
      stop("run smc forward sampler: invalid initial values.")
  }
  log.marg.like <- .Call("get_log_norm_const", object$ptr(), PACKAGE="RBiips")
  
  if (is.nan(log.marg.like)) {
    stop("Failed to get log marginal likelihood: NaN.")
  }
  if (is.infinite(log.marg.like)) {
    stop("Failed to get log marginal likelihood: infinite value.")
  }
  
  ## get latent variable sampled value
  if (length(latent.names) > 0) {
    sampled.value <- .Call("get_sampled_smooth_tree_particle", object$ptr(), PACKAGE="RBiips")
    if (length(sampled.value)==0) {
      ## sample one particle
      rng.seed <- runif(1, 0, as.integer(Sys.time()))
      .Call("sample_smooth_tree_particle", object$ptr(), as.integer(rng.seed), PACKAGE="RBiips")
      
      sampled.value <- .Call("get_sampled_smooth_tree_particle", object$ptr(), PACKAGE="RBiips")
    }
    for (var in latent.names) {
      sample[[var]] <- sampled.value[[var]]
    }
  }
  
  ans <- list(sample=sample, log.prior=log.prior, log.marg.like=log.marg.like)
  
  invisible(ans)
}


one.update.pmmh.biips <- function(object, param.names, latent.names=c(), pn.param,
                                  sample, log.prior, log.marg.like,
                                  n.part, rw.learn, ...)
{  
  n.fail <- 0
  
  ## Random walk proposal
  prop <- object$.rw.proposal(sample[param.names])
  
  ## check NA
  for (var in names(prop)) {
    if (any(is.na(prop[[var]]))) {
      stop("PMMH proposal have NA: ", var, " = ", prop[[var]])
    }
  }
  
  ## compute log prior density
  log.prior.prop <- 0
  
  for (v in seq(along=param.names)) {
    var <- param.names[[v]]
    
    ## change model data
    ok <- .Call("change_data", object$ptr(),
                pn.param$names[[v]], pn.param$lower[[v]], pn.param$upper[[v]],
                prop[[var]], TRUE, PACKAGE="RBiips")
    
    if (!ok) {
      log.prior.prop <- -Inf
      n.fail <- n.fail+1
      warning("Failure changing data. proposal: ",
              var, "=", prop[[var]], "\n")
      break
    }
    
    log.p <- .Call("get_log_prior_density", object$ptr(),
                 pn.param$names[[v]], pn.param$lower[[v]], pn.param$upper[[v]], PACKAGE="RBiips")
    
    if (is.na(log.p)) {
      stop("Failed to get log prior density: node ", var, "is not stochastic.")
    }

    log.prior.prop <- log.prior.prop + log.p
    
    if (log.prior.prop == -Inf) {
      break
    }
  }
    
  ## compute log marginal likelihood: run smc sampler
  log.marg.like.prop <- 0
  
  if (log.prior.prop != -Inf) {
    ok <- run.smc.forward(object, n.part=n.part, ...)
    
    if (!ok) {
      log.marg.like.prop <- -Inf
      n.fail <- n.fail+1
      warning("Failure running smc forward sampler. proposal: ",
              paste(names(prop), "=", prop, sep="", collapse="; "),
              "\n")
    } else {
      log.marg.like.prop <- .Call("get_log_norm_const", object$ptr(), PACKAGE="RBiips")
      if (is.nan(log.marg.like.prop)) {
        stop("Failed to get log marginal likelihood: NaN.")
      }
      if (log.marg.like.prop == Inf) {
        stop("Failed to get log marginal likelihood: +Inf.")
      }
    }
  }
  
  ## Acceptance rate
  accept.rate <- exp(log.marg.like.prop - log.marg.like + log.prior.prop - log.prior)
  if (is.nan(accept.rate)) {
    stop("Failed to compute acceptanc rate: NaN.")
  }
  
  ## Accept/Reject step  
  accepted <- (runif(1) < accept.rate)
  if (accepted) {
    sample <- prop
    log.prior <- log.prior.prop
    log.marg.like <- log.marg.like.prop
    
    if (length(latent.names) > 0) {
      ## sample one particle for the latent variables
      rng.seed <- runif(1, 0, as.integer(Sys.time()))
      .Call("sample_smooth_tree_particle", object$ptr(), as.integer(rng.seed), PACKAGE="RBiips")
      
      ## get sampled value
      sampled.value <- .Call("get_sampled_smooth_tree_particle", object$ptr(), PACKAGE="RBiips")
      for (var in latent.names) {
        sample[[var]] <- sampled.value[[var]]
      }
    }
  }
  
  ## rescale random walk step
  object$.rw.rescale(accept.rate)
  
  ## update random walk covariance matrices
  if (rw.learn)
    object$.rw.learn.cov(sample[param.names], accepted)
  
  ans <- list(sample=sample, log.prior=log.prior, log.marg.like=log.marg.like,
              accept.rate=accept.rate, accepted=accepted, n.fail=n.fail)
  invisible(ans)
}


update.pmmh <- function(object, ...)
  UseMethod("update.pmmh")


update.pmmh.biips <- function(object, param.names, n.iter, 
                              n.part, max.fail=0, inits=list(),
                              rw.step, rw.adapt=TRUE, rw.learn=TRUE, ...)
{ 
  ### check arguments
  ### -------------------------------
  
  ## check object
  if (!is.biips(object))
    stop("Invalid BiiPS model")
  ## check n.iter
  if (!is.numeric(n.iter) || !is.atomic(n.iter) || n.iter < 1)
    stop("Invalid n.iter argument")
  n.iter <- as.integer(n.iter)
  ## check rw.adapt
  if (!is.logical(rw.adapt) && ! is.atomic(rw.adapt))
    stop("invalid rw.adapt argument")
  ## check rw.learn
  if (!is.logical(rw.learn) && ! is.atomic(rw.learn))
    stop("invalid rw.learn argument")
  ## check rw.step
  if (!missing(rw.step))
  {
    rw.step.values <- list()
    
    if (is.numeric(rw.step)) {
      ## duplicate value for each variable
      for (n in param.names) {
        rw.step.values[[n]] <- rw.step
      }
    } else {
      if (!is.list(rw.step))
        stop("rw.step must be a numeric or list of numeric.")
      rw.step.values <- rw.step
      step.names <- names(rw.step.values)
      if(is.null(step.names)) ## case unnamed list
      {
        if (length(rw.step.values) != length(param.names))
          stop("rw.step length does not match param.names length.")
        names(rw.step.values) <- param.names
      } else ## case named list
      {
        if (any(nchar(step.names) == 0))
          stop("rw.step has unnamed values.")
        if (any(duplicated(step.names)))
          stop("rw.step has duplicated names:", paste(step.names[duplicated(step.names)], collapse=", "))
        ## check missing rw.step
        miss.step <- !param.names %in% step.names
        if (any(miss.step)) {
          stop("Missing rw.step values for variable:",
               paste(param.names[miss.step], collapse=", "))
        }
        ## check unkown variables from rw.step
        unknown.step <- !step.names %in% param.names
        if (any(unknown.step)) {
          stop("Unkown variable in rw.step values:",
               paste(step.names[unknown.step], collapse=", "))
        }
      }
      ## check non numeric values
      nonnum.step <- !sapply(rw.step.values, is.numeric)
      if (any(nonnum.step))
        stop("Non numeric rw.step values for variables:", paste(names(rw.step.values)[nonnum.step], collapse=", "))
    }
  }
  
  ### stop biips verbosity
  verb <- .Call("verbosity", 0, PACKAGE="RBiips")
  on.exit(.Call("verbosity", verb, PACKAGE="RBiips"))
  
  ### Initialize
  ### -------------------------------
  out <- init.pmmh.biips(object, param.names=param.names, n.part=n.part,
                         inits=inits, quiet=TRUE,...)
  
  pn.param <- parse.varnames(param.names)
  
  ### assign rw step
  if (!missing(rw.step))
    object$.rw.step(rw.step.values)
  
  ### stop adaptation if necessary
  if (!rw.adapt) {
    if (object$.rw.adapt() && !object$.rw.check.adapt())
      cat("NOTE: Stopping adaption of the PMMH random walk.\n")
    object$.rw.adapt.off()
  }
  
  ### display message and progress bar
  title <- ifelse(object$.rw.adapt(), "Adapting", "Updating")
  .Call("message", paste(title, "PMMH with", n.part, "particles"), PACKAGE="RBiips")
  symbol <- ifelse(object$.rw.adapt(), '+', '*')
  bar <- .Call("progress_bar", n.iter, symbol, "iterations", PACKAGE="RBiips")
  
  ## initialize counters
  n.fail <- 0
  n.accept <- 0
  
  ### Metropolis-Hastings iterations
  ### -------------------------------
  
  for(i in 1:n.iter) {
    ## iterate
    out <- one.update.pmmh.biips(object, param.names=param.names,
                                 pn.param=pn.param,
                                 sample=out$sample, log.prior=out$log.prior, log.marg.like=out$log.marg.like,
                                 n.part=n.part, rw.learn=rw.learn, ...)
    
    ## advance progress bar
    .Call("advance_progress_bar", bar, 1, PACKAGE="RBiips")
    
    ## increment counters
    n.fail <- n.fail + out$n.fail
    n.accept <- n.accept + out$accepted
    
    ## check failures
    if (n.fail > max.fail) {
      stop("Number of failures exceeds max.fail: ", n.fail, " failures.")
    }
  }
  
  ### turn off adaption if checked
  if (object$.rw.check.adapt())
    object$.rw.adapt.off()
  
  ### release monitors memory
  clear.monitors.biips(object, type="s", TRUE)
  
  ### reset log norm const and sampled values if not accepted
  if (n.iter > 0 && !out$accepted) {
    for (v in seq(along=param.names)) {
      var <- param.names[[v]]
      if(!.Call("change_data", object$ptr(),
                pn.param$names[[v]], pn.param$lower[[v]], pn.param$upper[[v]],
                out$sample[[var]], TRUE, PACKAGE="RBiips"))
        stop("can not reset previous data: ", var, "=", out$sample[[var]])
    }
    .Call("set_log_norm_const", object$ptr(), out$log.marg.like, PACKAGE="RBiips")
  }
  
  ### effective acceptance rate
  ar.eff <- n.accept/n.iter
  
  ans <- list(ar.eff=ar.eff, n.fail=n.fail)
  invisible(ans)
}


init.pimh.biips <- function(object, variable.names,
                            n.part, rs.thres=0.5, rs.type="stratified")
{
  monitored <- is.monitored.biips(object, variable.names, "s", FALSE)
  if (!monitored) {
    ## monitor variables
    monitor.biips(object, variable.names, type="s")
  }
  
  built <- .Call("is_sampler_built", object$ptr(), PACKAGE="RBiips")
  if (!built) {
    ## build smc sampler
    .Call("build_smc_sampler", object$ptr(), FALSE, PACKAGE="RBiips")
  }
  
  atend <- .Call("is_smc_sampler_at_end", object$ptr(), PACKAGE="RBiips")
  ## get log normalizing constant
  if (!monitored || !built || !atend) {
    ## run smc sampler
    .Call("message", "Initializing PIMH", PACKAGE="RBiips")
    run.smc.forward(object, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type)
  }
  log.marg.like <- .Call("get_log_norm_const", object$ptr(), PACKAGE="RBiips")
  
  ## get sampled value
  sampled.value <- .Call("get_sampled_smooth_tree_particle", object$ptr(), PACKAGE="RBiips")
  if (length(sampled.value)==0) {
    ## sample one particle
    rng.seed <- runif(1, 0, as.integer(Sys.time()))
    .Call("sample_smooth_tree_particle", object$ptr(), as.integer(rng.seed), PACKAGE="RBiips")
    
    sampled.value <- .Call("get_sampled_smooth_tree_particle", object$ptr(), PACKAGE="RBiips")
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
    .Call("sample_smooth_tree_particle", object$ptr(), as.integer(rng.seed), PACKAGE="RBiips")
    
    ## get sampled value
    sampled.value <- .Call("get_sampled_smooth_tree_particle", object$ptr(), PACKAGE="RBiips")
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
