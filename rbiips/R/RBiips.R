#                                               -*- R -*-
#
#  RBiips package for GNU R is an interface to BiiPS libraries for
#  Bayesian inference with interacting Particle Systems.
#  Copyright (C) Inria, 2012
#  Contributors: Adrien Todeschini, Francois Caron
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

set.biips.verbosity <- function(level=1)
{
  .Call("set_verbosity", level, PACKAGE="RBiips")
	invisible(NULL)
}


load.biips.module <- function(name, quiet=FALSE)
{    
  if (!is.character(name) || !is.atomic(name))
    stop("invalid name")

  ok <- .Call("load_module", name, PACKAGE="RBiips")
  if (!ok) {
    stop("module", name, "not found\n", sep=" ")
  }
  else if (!quiet) {
    cat("module", name, "loaded\n", sep=" ")
  }
	invisible(NULL)
}


biips.model <- function(file, data=sys.frame(sys.parent()), sample.data=TRUE, data.rng.seed)
{
  if (missing(file)) {
    stop("Model file name missing")
  }
  if (!is.atomic(file)) {
    stop(paste("Only one model file allowed."))
  }
  if (!file.exists(file)) {
    stop(paste("Model file \"", file, "\" not found", sep=""))
  }
  
  p <- .Call("make_console", PACKAGE="RBiips")
  .Call("check_model", p, file, PACKAGE="RBiips")

  varnames <- .Call("get_variable_names", p, PACKAGE="RBiips")
  if (is.environment(data)) {
    ##Get a list of numeric objects from the supplied environment
    data <- mget(varnames, envir=data, mode="numeric",
                 ifnotfound=list(NULL))
    ##Strip null entries
    data <- data[!sapply(data, is.null)]
  }
  else if (is.list(data)) {
    v <- names(data)
    if (is.null(v)) {
      stop("data must be a named list")
    }
    if (any(nchar(v)==0)) {
      stop("unnamed variables in data list")
    }
    if (any(duplicated(v))) {
      stop("Duplicated names in data list: ",
         paste(v[duplicated(v)], collapse=" "))
    }
    relevant.variables <- v %in% varnames
    data <- data[relevant.variables]
    unused.variables <- setdiff(v, varnames)
    for (i in seq(along=unused.variables)) {
      warning("Unused variable \"", unused.variables[i], "\" in data")
    }
  }
  else {
    stop("data must be a list or environment")
  }
  
  if(!is.logical(sample.data) || !is.atomic(sample.data)) {
    stop("Invalid sample.data argument.");
  }
  if(missing("data.rng.seed")) {
    data.rng.seed <- runif(1, 0, as.integer(Sys.time()));
  }
  else if(!is.numeric(data.rng.seed) || !is.atomic(data.rng.seed)) {
    stop("Invalid data.rng.seed argument.");
  }
  
  .Call("compile_model", p, data, sample.data, as.integer(data.rng.seed), PACKAGE="RBiips")

  model.code <- readLines(file, warn=FALSE)
  model <- list("ptr" = function() {p},
                "model" = function() {model.code},
                "dot" = function(dot.file) {
                  .Call("print_graphviz", p, dot.file, PACKAGE="RBiips")
                  invisible(NULL)
                },
                "data" = function() { .Call("get_data", p, PACKAGE="RBiips") },
                "nodes" = function(type, observed) {
                   sorted.nodes <- data.frame(.Call("get_sorted_nodes", p, PACKAGE="RBiips"))
                   
                   if(.Call("is_sampler_built", p, PACKAGE="RBiips")) {
                     samplers <- data.frame(.Call("get_node_samplers", p, PACKAGE="RBiips"))
                     sorted.nodes <- cbind(sorted.nodes, samplers)
                   }
                   if(!missing(type)) {
                     if(!is.character(type) || !is.atomic(type)) {
                       stop("Invalid type argument.");
                     }
                     type <- match.arg(type, c("Constant", "Logical", "Stochastic"))
                     sorted.nodes <- sorted.nodes[sorted.nodes["type"]==type,]
                   }
                   if(!missing(observed)) {
                     if(!is.logical(observed) || !is.atomic(observed)) {
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
                  .Call("compile_model", p, .Call("get_data", p, PACKAGE="RBiips"),
                        FALSE, as.integer(data.rng.seed), PACKAGE="RBiips")
                  invisible(NULL)
                })
  class(model) <- "biips"

  return(model)
}

  
is.biips <- function(obj)
{
  return(class(obj) == "biips")
}


print.biips <- function(x, ...)
{
  if (!is.biips(x))
    stop("Invalid BiiPS model.")
  
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


parse.varname <- function(varname) {

  ## Try to parse string of form "a" or "a[n,p:q,r]" where "a" is a
  ## variable name and n,p,q,r are integers

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


parse.varnames <- function(varnames)
{
  names <- character(length(varnames))
  lower <- upper <- vector("list", length(varnames))
  for (i in seq(along=varnames)) {
    y <- parse.varname(varnames[i])
    if (is.null(y)) {
      stop(paste("Invalid variable subset", varnames[i]))
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


smc.samples <- function(obj, variable.names, type="fs", backward=FALSE, ...)
{  
  if (!is.logical(backward) || !is.atomic(backward)) {
    stop("Invalid backward argument")
  }
  
  type <- unlist(strsplit(type, NULL))
  type <- match.arg(type, c('f', 's', 'b'), several.ok=TRUE)
  backward <- ('b' %in% type)
  
  ## monitor
  if(backward) {
    .Call("set_default_monitors", obj$ptr(), PACKAGE="RBiips")
  }
  if (!missing(variable.names))
    monitor.biips(obj, variable.names, type)
  
  ## smc forward sampler
  run.smc.forward(obj, ...)
  
  log.norm.const <- .Call("get_log_norm_const", obj$ptr(), PACKAGE="RBiips")
  ans <- list()
  
  mon <- .Call("get_filter_monitors", obj$ptr(), PACKAGE="RBiips")
  for (n in names(mon)) {
    ans[[n]][["filtering"]] <- mon[[n]]
  }
  if (!backward) {
    clear.monitors.biips(obj, type="f")
  }
  
  mon <- .Call("get_smooth_tree_monitors", obj$ptr(), PACKAGE="RBiips")
  for (n in names(mon)) {
    ans[[n]][["smoothing"]] <- mon[[n]]
  }
  clear.monitors.biips(obj, type="s")
  
  ## smc backward smoother
  if (backward)
  {
    ## run backward smoother
    .Call("run_backward_smoother", obj$ptr(), PACKAGE="RBiips")
    
    clear.monitors.biips(obj, type="f")
    
    mon <- .Call("get_smooth_monitors", obj$ptr(), PACKAGE="RBiips")
    for (n in names(mon)) {
      ans[[n]][["backward.smoothing"]] <- mon[[n]]
    }
    clear.monitors.biips(obj, type="b")
  }
  
  for (n in names(ans)) {
    class(ans[[n]]) <- "particles.list"
  }
  ans[["log.norm.const"]] <- log.norm.const

  return(ans)
}


pmmh.samples <- function(obj, variable.names, n.iter, thin=1, rw.sd,
                         n.part, max.fail=0, ...)
{  
  if (!is.numeric(n.iter) || !is.atomic(n.iter) || n.iter < 1)
    stop("Invalid n.iter argument")
  n.iter <- as.integer(n.iter)
  if (!is.numeric(thin) || !is.atomic(thin) || thin < 1)
    stop("Invalid thin argument")
  thin <- as.integer(thin)
  
  if (!is.list(rw.sd))
    stop("Invalid rw.sd argument")
  for (var in variable.names) {
    if (!var %in% names(rw.sd))
      stop(paste("Missing rw.sd value for variable", var))
  }
  
  ## stop biips verbosity
  set.biips.verbosity(0)
  
  ## restart biips verbosity on exit
  on.exit(set.biips.verbosity(1)) 
  
  ## Initialization
  #---------------  
  out <- init.pmmh.biips(obj, variable.names=variable.names, n.part=n.part, ...)
  sample <- out$sample
  log.prior <- out$log.prior
  log.norm.const <- out$log.norm.const
  
  pn <- parse.varnames(variable.names)
  
  # check rw.sd dimension
  for (var in variable.names) {
    if (length(rw.sd[[var]]) != length(sample[[var]]))
      stop(paste("incorrect rw.sd dimension for variable", var))
  }
  
  ## reset data to sample on exit
  on.exit(
      if (n.iter > 0 && !accepted) {
        .Call("set_log_norm_const", obj$ptr(), log.norm.const, PACKAGE="RBiips")
      }, add=TRUE)
  
  ans <- list()
  n.samples <- 0
  n.fail <- 0
  accepted <- TRUE
  
  ## progress bar
  bar <- .Call("progress_bar", n.iter, '*', "iterations", PACKAGE="RBiips")
  
  ## Metropolis-Hastings iterations
  ##-------------------------------
  for(i in 1:n.iter) {
    out <- one.update.pmmh.biips(obj, variable.names=variable.names, pn=pn, rw.sd=rw.sd,
                                sample=sample, log.prior=log.prior, log.norm.const=log.norm.const,
                                 n.part=n.part, ...)
    sample <- out$sample
    log.prior <- out$log.prior
    log.norm.const <- out$log.norm.const
    accepted <- out$accepted
    n.fail <- n.fail + out$n.fail
    
    ## advance progress bar
    .Call("advance_progress_bar", bar, 1, PACKAGE="RBiips")
    
    if (n.fail > max.fail) {
      stop(paste("Number of failures exceeds max.fail:", n.fail, "failures."))
    }
    
    ## Store output
    if ((i-1)%%thin == 0) {
      n.samples <- n.samples +1
      ans[["log.norm.const"]][n.samples] <- log.norm.const
      for (var in variable.names)
        ans[[var]] <- c(ans[[var]], sample[[var]])
    }
  }
  
  ## set output dimensions
  for (var in variable.names) {
    dim(ans[[var]]) <- c(dim(sample[[var]]), n.samples)
    names(dim(ans[[var]])) <- c(rep("", length(dim(sample[[var]]))), "iteration")
    class(ans[[var]]) <- "mcarray"
  }
  dim(ans[["log.norm.const"]]) <- c(1, n.samples)
  names(dim(ans[["log.norm.const"]])) <- c("", "iteration")
  class(ans[["log.norm.const"]]) <- "mcarray"
  
  return(ans)
}


pimh.samples <- function(obj, variable.names, n.iter, thin=1,
                       n.part, rs.thres=0.5, rs.type="stratified")
{
  if (!is.biips(obj))
    stop("Invalid BiiPS model")
  
  ## check variable.names
  if (!is.character(variable.names))
    stop("Invalid variable.names argument")
  parse.varnames(variable.names)
  
  if (!is.numeric(n.iter) || !is.atomic(n.iter) || n.iter < 1)
    stop("Invalid n.iter argument")
  n.iter <- as.integer(n.iter)
  if (!is.numeric(thin) || !is.atomic(thin) || thin < 1)
    stop("Invalid thin argument")
  thin <- as.integer(thin)
  
  ## stop biips verbosity
  set.biips.verbosity(0)
  
  ## Initialization
  ##---------------
  out <- init.pimh.biips(obj, variable.names=variable.names,
                         n.part=n.part, rs.thres=rs.thres, rs.type=rs.type)
  sample <- out$sample
  log.norm.const <- out$log.norm.const
  
  ans <- list()
  n.samples <- 0
  
  ## progress bar
  bar <- .Call("progress_bar", n.iter, '*', "iterations", PACKAGE="RBiips")
  
  ## Independant Metropolis-Hastings iterations
  ##-------------------------------------------
  for(i in 1:n.iter) {
    out <- one.update.pimh.biips(obj, variable.names=variable.names,
                                 n.part=n.part, rs.thres=rs.thres, rs.type=rs.type,
                                 sample=sample, log.norm.const=log.norm.const)
    sample <- out$sample
    log.norm.const <- out$log.norm.const
    accepted <- out$accepted
    
    ## Store output
    if ((i-1)%%thin == 0) {
      n.samples <- n.samples + 1
      ans[["log.norm.const"]][n.samples] <- log.norm.const
      for (var in variable.names)
        ans[[var]] <- c(ans[[var]], sample[[var]])
    }
    
    ## advance progress bar
    .Call("advance_progress_bar", bar, 1, PACKAGE="RBiips")
  }
  
  clear.monitors.biips(obj, type="s")
  
  ## reset log norm const and sampled value
  if (n.iter > 0 && !accepted) {
    .Call("set_log_norm_const", obj$ptr(), log.norm.const, PACKAGE="RBiips")
    .Call("set_sampled_smooth_tree_particle", obj$ptr(), sample, PACKAGE="RBiips")
  }
 
  
  ## set output dimensions
  for (var in variable.names) {
    n.dim <- length(dim(sample[[var]]))
    dim(ans[[var]]) <- c(dim(sample[[var]]), n.samples)
    names(dim(ans[[var]])) <- c(rep("", n.dim), "iteration")
    class(ans[[var]]) <- "mcarray"
  }
  dim(ans[["log.norm.const"]]) <- c(1, n.samples)
  names(dim(ans[["log.norm.const"]])) <- c("", "iteration")
  class(ans[["log.norm.const"]]) <- "mcarray"
  
  ## start biips verbosity
  set.biips.verbosity(1)
    
  return(ans)
}
