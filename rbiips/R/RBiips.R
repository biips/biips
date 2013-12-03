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

#' load the corresponding module into the Biips environment
#' @param the module name
#' @param quiet verbose flag
#' @return null
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


#' Add the corresponding R function to the bugs model
#' @param name of the new  function
#' @param number of arguments of the new function
#' @param R function returning a vector containg arguments sizes
#' @param R function computing the result of function
#' @param R function checking the arguments 
#' @param R function telling is new function is discrete wrt its arguments
biips.add.function <- function(name, nb.args, fundim, funeval, funcheckparam, funisdiscrete)
{    
  if (!is.character(name) || length(name)>1)
  {
    stop("invalid name")
  }
  ok <- .Call("add_function", name, nb.args, fundim , funeval, funcheckparam, funisdiscrete, PACKAGE="RBiips")
}

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
  rw <- list(adapt=TRUE,
                niter=2,
                pmean=0,
                lstep=log(0.1),
                targetprob=0.234, # Target acceptance probability. The default seems to
                # be a fairly robust optimal value. Comes From (Rosenthal 2009, Examples 
                #of Adaptives MCMC, p 16. Beware , in one-dimensional case, 
                # it would be better to take ~0.44 
                povertarget=FALSE,
                ncrosstarget=10, #The value ncrosstarget controls the reduction in the step size when rescale is
                #called. There is no reason to give it an initial value of zero. In
                #fact this is a poor choice since the  step size would be immediately
                #halved. We start with a value of 10 so the first change in step size
                #is 10%.
                count=0,
                dim=list(),
                buffer=c(),
                buff.count=c(),
                mean=c(),
                cov=c(),
                cov_chol=c()
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
                ".rw.dim" = function(sample) {
                  rw$dim <<- lapply(sample, dim)
                  d.null <- sapply(rw$dim, is.null)
                  if (any(d.null)) {
                    len <- lapply(sample, length)
                    rw$dim[d.null] <<- len[d.null]
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
                    
                  invisible(NULL)
                },
                ".rw.rescale" = function(p) {
                  # We keep a weighted mean estimate of the mean acceptance probability
                  # with the weights in favour of more recent iterations
                  p <- min(p, 1.0)
                  rw$pmean <<- rw$pmean + 2*(p-rw$pmean)/rw$niter
                  rw$niter <<- rw$niter+1
                  
                  if (!rw$adapt) {
                    return(NULL)
                  }
                  
                  # The step size is adapted to achieve the
                  # target acceptance rate using a noisy gradient algorithm.
                  rw$lstep <<- rw$lstep + (p-rw$targetprob)/rw$ncrosstarget
                  if ((p>rw$targetprob) != rw$povertarget) {
                    rw$povertarget <<- !rw$povertarget
                    rw$ncrosstarget <<- rw$ncrosstarget+1
                  }
                  
                  invisible(NULL)
                },
                ".rw.adapt" = function() {
                  return(rw$adapt)
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
                  rw$adapt <<- FALSE
                  invisible(NULL)
                },
                ".rw.proposal" = function(sample) {
                  # concatenate all variables in a vector
                  # always in the order of rw$dim
                  sample_vec <- c()
                  for (n in names(rw$dim)) {
                    sample_vec <- c(sample_vec, sample[[n]])
                  }
                  d <- length(sample_vec)
                  if (rw$adapt || length(rw$cov_chol) == 0) {
                    # modification with independent components
                    prop_vec <- sample_vec + 2.38/sqrt(d) * exp(rw$lstep) * rnorm(d)
                  } else {
                    prop_vec <- sample_vec + 2.38/sqrt(d) * rw$cov_chol %*% rnorm(d)
                  }
                  prop <- list()
                  from <- 1
                  for (n in names(rw$dim)) {
                    to <- from+prod(rw$dim[[n]])-1
                    prop[[n]] <- array(prop_vec[from:to], dim=rw$dim[[n]])
                    from <- to+1
                  }
                  invisible(prop)
                },
                ".rw.learn.cov" = function(sample, accepted) {
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
                    }
                    # clear buffer
                    rw$buffer <<- c()
                    rw$buff.count <<- c()
                    
                    # compute choleski decomposition
                    rw$cov_chol <<- t(chol(rw$cov))
                    
                    rw$count <<- rw$count+m
                  }
                  
                  invisible(NULL)
                })
  class(model) <- "biips"
  
  return(model)
}


is.biips <- function(object)
{
  return(class(object) == "biips")
}


print.biips <- function(x,...)
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

#' Try to parse string of form "a" or "a[n,p:q,r]" where "a" is a
#' variable name and n,p,q,r are integers
#' @param varname string containing the name of the variable to sparse
parse.varname <- function(varname) {
  
  
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


smc.samples <- function(object, variable.names, n.part, type="fs",
                        rs.thres = 0.5, rs.type = "stratified", ...)
{
  if (!is.character(type))
    stop("'type' must be a character string with characters 'f', 's', 'b'")
  
  type <- unlist(strsplit(type, NULL))
  type <- match.arg(type, c('f', 's', 'b'), several.ok=TRUE)
  backward <- ('b' %in% type)
  
  ## monitor
  if(backward) {
    .Call("set_default_monitors", object$ptr(), PACKAGE="RBiips")
  }
  if (!missing(variable.names))
    monitor.biips(object, variable.names, type)
  
  ## smc forward sampler
  run.smc.forward(object, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type, ...)
  
  log.marg.like <- .Call("get_log_norm_const", object$ptr(), PACKAGE="RBiips")
  ans <- list()
  
  mon <- .Call("get_filter_monitors", object$ptr(), PACKAGE="RBiips")
  for (n in names(mon)) {
    ans[[n]][["filtering"]] <- mon[[n]]
  }
  if (!backward) {
    clear.monitors.biips(object, type="f")
  }
  
  mon <- .Call("get_gen_tree_smooth_monitors", object$ptr(), PACKAGE="RBiips")
  for (n in names(mon)) {
    ans[[n]][["smoothing"]] <- mon[[n]]
  }
  clear.monitors.biips(object, type="s")
  
  ## smc backward smoother
  if (backward)
  {
    ## run backward smoother
    .Call("run_backward_smoother", object$ptr(), PACKAGE="RBiips")
    
    clear.monitors.biips(object, type="f")
    
    mon <- .Call("get_backward_smooth_monitors", object$ptr(), PACKAGE="RBiips")
    for (n in names(mon)) {
      ans[[n]][["backward.smoothing"]] <- mon[[n]]
    }
    clear.monitors.biips(object, type="b")
  }
  
  for (n in names(ans)) {
    class(ans[[n]]) <- "particles.list"
  }
  ans[["log.marg.like"]] <- log.marg.like
  
  return(ans)
}


pimh.samples <- function(object, variable.names, n.iter, thin = 1,
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
  if (!is.numeric(thin) || length(thin)!=1 || thin < 1)
    stop("Invalid thin argument")
  thin <- as.integer(thin)
  
  ## stop biips verbosity
  verb <- .Call("verbosity", 0, PACKAGE="RBiips")
  on.exit(.Call("verbosity", verb, PACKAGE="RBiips"))
  
  ## Initialization
  ##---------------  
  out <- init.pimh.biips(object, variable.names=variable.names,
                         n.part=n.part, ...)
  sample <- out$sample
  log.marg.like <- out$log.marg.like
  
  ans <- list()
  n.samples <- 0
  
  .Call("message", paste("Generating PIMH samples with", n.part, "particles"), PACKAGE="RBiips")
  ## progress bar
  bar <- .Call("progress_bar", n.iter, '*', "iterations", PACKAGE="RBiips")
  
  ## Independant Metropolis-Hastings iterations
  ##-------------------------------------------
  for(i in 1:n.iter) {
    out <- one.update.pimh.biips(object, variable.names=variable.names,
                                 n.part=n.part,
                                 sample=sample, log.marg.like=log.marg.like, ...)
    sample <- out$sample
    log.marg.like <- out$log.marg.like
    accepted <- out$accepted
    
    ## Store output
    if ((i-1)%%thin == 0) {
      n.samples <- n.samples + 1
      ans[["log.marg.like"]][n.samples] <- log.marg.like
      for (var in variable.names)
        ans[[var]] <- c(ans[[var]], sample[[var]])
    }
    
    ## advance progress bar
    .Call("advance_progress_bar", bar, 1, PACKAGE="RBiips")
  }
  
  clear.monitors.biips(object, type="s")
  
  ## reset log norm const and sampled value
  if (n.iter > 0 && !accepted) {
    .Call("set_log_norm_const", object$ptr(), log.marg.like, PACKAGE="RBiips")
    .Call("set_sampled_gen_tree_smooth_particle", object$ptr(), sample, PACKAGE="RBiips")
  }
  
  
  ## set output dimensions
  for (var in variable.names) {
    n.dim <- length(dim(sample[[var]]))
    dim(ans[[var]]) <- c(dim(sample[[var]]), n.samples)
    names(dim(ans[[var]])) <- c(rep("", n.dim), "iteration")
    class(ans[[var]]) <- "mcarray"
  }
  dim(ans[["log.marg.like"]]) <- c(1, n.samples)
  names(dim(ans[["log.marg.like"]])) <- c("", "iteration")
  class(ans[["log.marg.like"]]) <- "mcarray"
  
  return(ans)
}


smc.sensitivity <- function(object, params,
                            n.part, ...)
{
  if (!is.biips(object))
    stop("Invalid BiiPS model")
  
  if (missing(n.part))
    stop("missing n.part argument")
  
  ## check params
  if (!is.list(params) || length(params) == 0)
    stop("Invalid params argument")
  variable.names <- names(params)
  pn <- parse.varnames(variable.names)
  
  ## get parameters array length
  dim <- list()
  len <- list()
  d <- dim(params[[1]])
  if (is.null(d)) {
    n.params <- length(params[[1]])
    dim[[names(params)[1]]] <- 1
    len[[names(params)[1]]] <- 1
  } else {
    n.params <- d[[length(d)]]
    d <- d[1:(length(d)-1)]
    dim[[names(params)[1]]] <- d
    len[[names(params)[1]]] <- prod(d)
  }
  if (length(params) > 1) {
    for (i in 2:length(params)) {
      d <- dim(params[[i]])
      if (is.null(d)) {
        n <- length(params[[i]])
        dim[[names(params)[i]]] <- 1
        len[[names(params)[i]]] <- 1
      } else {
        n <- d[[length(d)]]
        d <- d[1:(length(d)-1)]
        dim[[names(params)[i]]] <- d
        len[[names(params)[i]]] <- prod(d)
      }
      # check parameters array length
      if (n != n.params)
        stop("Invalid params argument: different number of parameters.")
    }
  }
  
  ## stop biips verbosity
  verb <- .Call("verbosity", 0, PACKAGE="RBiips")
  on.exit(.Call("verbosity", verb, PACKAGE="RBiips"))
  
  ## initialize
  ##-----------
  log.marg.like <- vector(length=n.params)
  log.marg.like.pen <- vector(length=n.params)
  max.log.marg.like <- -Inf
  max.log.marg.like.pen <- -Inf
  
  .Call("message", paste("Analyzing sensitivity with", n.part, "particles"), PACKAGE="RBiips")
  ## progress bar
  bar <- .Call("progress_bar", n.params, '*', "iterations", PACKAGE="RBiips")
  
  ## Iterate 
  ##--------
  for (k in 1:n.params) {
    
    ## make one param
    param <- list()
    for (v in seq(along=variable.names)) {
      var <- variable.names[[v]]
      ind <- seq(from=(k-1)*len[[v]]+1, to=k*len[[v]])
      param[[var]] <- params[[v]][ind]
      dim(param[[var]]) <- dim[[v]]
      
      ## change param value
      if(!.Call("change_data", object$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]],
                param[[v]], FALSE, PACKAGE="RBiips"))
        stop("data change failed: invalid parameter. variable = ", var,". value = ", param[v])
    }
    
    log.prior <- 0
    for (v in seq(along=variable.names)) {
      log.p <- .Call("get_log_prior_density", object$ptr(), 
                     pn$names[[v]], pn$lower[[v]], pn$upper[[v]], PACKAGE="RBiips")
      
      if (is.na(log.p)) {
        next
      }
      
      if (is.nan(log.p) || (is.infinite(log.p) && log.p<0))
        stop("Failure evaluating parameter log prior for variable ",
                   variable.names[[v]], ". value = ", param[v])
      
      log.prior <- log.prior + log.p
    }
    
    ## run smc sampler
    ok <- run.smc.forward(object, n.part=n.part, ...)
    
    if (!ok)
      stop("Failure running smc forward sampler. param: ", paste(names(param),"=", param, sep="", collapse=";"))
    
    ## log marginal likelihood
    log.marg.like[k] <- .Call("get_log_norm_const", object$ptr(), PACKAGE="RBiips")
    if (log.marg.like[k] > max.log.marg.like) {
      max.log.marg.like <- log.marg.like[k]
      max.param <- param
    }
    log.marg.like.pen[k] <- log.marg.like[k] + log.prior
    if (log.marg.like.pen[k] > max.log.marg.like.pen) {
      max.log.marg.like.pen <- log.marg.like.pen[k]
      max.param.pen <- param
    }
    
    ## advance progress bar
    .Call("advance_progress_bar", bar, 1, PACKAGE="RBiips")
  }
  
  ## restore data
  ##-------------
  object$recompile()
  
  # FIXME: Tentative of removing data without recompiling the model
  #   for (v in seq(along=variable.names)) {
  #     if (!(pn$names[[v]] %in% names(data))) {
  #       ok <- .Call("remove_data", object$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]], PACKAGE="RBiips")
  #       if (!ok)
  #         stop("Failure restoring data")
  #       next
  #     }
  #     
  #     if (is.null(pn$lower[[v]])) {
  #       data.sub <- data[[v]]
  #     } else {
  #       ## compute offsets
  #       offsets <- NULL
  #       dim <- dim(data[[v]])
  #       if (is.null(dim))
  #         dim <- length(data[[v]])
  #       ind <- vector(len=length(dim))
  #       
  #       for (i in seq(along=data[[v]])) {
  #         r <- i
  #         for (d in seq(along=dim)) {
  #           ind[d] <- ((r-1) %% dim[d]) +1
  #           r <- ceiling(r/dim[d])
  #         }
  #         print(ind)
  #         if (any(ind<pn$lower[[v]]) || any(ind>pn$upper[[v]]))
  #           next
  #         offsets <- c(offsets, i)
  #       }
  #       
  #       data.sub <- array(data[[v]][offsets], dim=pn$upper[[v]]-pn$lower[[v]]+1)
  #     }
  #     
  #     if (all(is.na(data.sub))) {
  #       ok <- .Call("remove_data", object$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]], PACKAGE="RBiips")
  #       if (!ok)
  #         stop("Failure restoring data")
  #       next
  #     }
  #     
  #     if (any(is.na(data.sub)))
  #       stop("Failure restoring data")
  #     
  #     ok <- .Call("change_data", object$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]],
  #                 data.sub, FALSE, PACKAGE="RBiips")
  #     if (!ok)
  #       stop("Failure restoring data")
  #   }
  
  ## output
  ##--------
  ans <- list(log.marg.like=log.marg.like, max.param=max.param, max.log.marg.like=max.log.marg.like,
              log.marg.like.pen=log.marg.like.pen, max.param.pen=max.param.pen, max.log.marg.like.pen=max.log.marg.like.pen)
  
  return(ans)
}
