
##' initialize pmmh algorithm
##' 
##' @S3method pmmh.init biips
##' @param object the Biips model
##' @param param.names vector of params
##'
pmmh.init.biips <- function(object, param.names, latent.names = c(), inits = list(), 
  n_part, rs_thres = 0.5, rs_type = "stratified", inits.rng.seed, ...) {
  if (!is.biips(object)) 
    stop("Invalid BiiPS model")
  
  ## check param.names
  if (!is.character(param.names)) 
    stop("Invalid param.names argument")
  if (any(duplicated(param.names))) {
    stop("duplicated names in param.names: ", paste(param.names[duplicated(param.names)], 
      collapse = " "))
  }
  pn.param <- parse.varnames(param.names)
  
  ## check latent.names
  if (length(latent.names) > 0) {
    if (!is.character(latent.names)) 
      stop("Invalid latent.names argument")
    if (any(duplicated(latent.names))) {
      stop("duplicated names in latent.names: ", paste(latent.names[duplicated(latent.names)], 
        collapse = " "))
    }
    parse.varnames(latent.names)
  }
  
  ## check inits
  init.values <- list()
  if (length(inits) > 0) {
    if (is.function(inits)) {
      init.values <- inits()
      if (!is.list(init.values)) 
        stop("inits function must return a list.")
    } else if (is.list(inits)) {
      init.values <- inits
    } else {
      stop("inits must be a list or a function.")
    }
    
    inames <- names(init.values)
    
    if (is.null(inames)) {
      ## case unnamed list
      
      if (length(init.values) != length(param.names)) 
        stop("Invalid inits argument.")
      names(init.values) <- param.names
    } else {
      ## case named list
      
      if (any(nchar(inames) == 0)) 
        stop("inits has unnamed values.")
      
      if (any(duplicated(inames))) 
        stop("inits has duplicated names:", paste(inames[duplicated(inames)], 
          sep = ","))
      
      ## Warn missing init values
      miss.inits <- !param.names %in% inames
      if (any(miss.inits)) {
        warning("Missing initial values for variable:", paste(param.names[miss.inits], 
          sep = ","))
      }
      ## Strip unkown variables from initial values, but give a warning
      unknown.inits <- !inames %in% param.names
      if (any(unknown.inits)) {
        warning("Ignoring unkown variable in initial values:", paste(inames[unknown.inits], 
          sep = ","))
        init.values <- init.values[!unknown.inits]
        inames <- names(init.values)
      }
      ## Strip null initial values, but give a warning
      null.inits <- sapply(init.values, is.null)
      if (any(null.inits)) {
        warning("Ignoring null initial values supplied for variable:", paste(inames[null.inits], 
          sep = ","))
        init.values <- init.values[!null.inits]
        inames <- names(init.values)
      }
    }
    
    nonnum.inits <- !sapply(init.values, is.numeric)
    if (any(nonnum.inits)) 
      stop("non numeric initial values supplied for variable:", paste(inames[nonnum.inits], 
        sep = ","))
  }
  
  
  ## check inits.rng.seed
  if (missing(inits.rng.seed)) {
    inits.rng.seed <- runif(1, 0, as.integer(Sys.time()))
  }
  if (!is.numeric(inits.rng.seed) || length(inits.rng.seed) != 1 || inits.rng.seed < 
    0) {
    stop("Invalid inits.rng.seed argument")
  }
  
  ## stop biips verbosity
  verb <- RBiips("verbosity", 0)
  on.exit(RBiips("verbosity", verb))
  
  ## make init sample
  sample <- list()
  for (v in seq(along = param.names)) {
    var <- param.names[[v]]
    if (var %in% names(init.values)) {
      # take init value in inits param
      if (!.Call("change_data", object$ptr(), pn.param$names[[v]], pn.param$lower[[v]], 
        pn.param$upper[[v]], init.values[[var]], TRUE, PACKAGE = "RBiips")) 
        stop("data change failed: invalid initial value for variable ", var)
      sample[[var]] <- init.values[[var]]
    } else {
      # or sample init value
      data <- object$.data_sync()
      if (var %in% names(data)) 
        sample[[var]] <- data[[var]] else sample[[var]] <- .Call("sample_data", object$ptr(), pn.param$names[[v]], 
        pn.param$lower[[v]], pn.param$upper[[v]], inits.rng.seed, PACKAGE = "RBiips")
    }
  }
  
  ## check NA
  has.na <- sapply(sample, function(x) {
    any(is.na(x))
  })
  if (any(has.na)) {
    stop("PMMH initial values have NA: ", paste(names(sample[has.na]), "=", sample[has.na], 
      sep = "", collapse = "; "), "\n")
  }
  
  ## initialize rw
  object$.rw_init(sample)
  
  ## log prior density
  log.prior <- 0
  for (v in seq(along = param.names)) {
    var <- param.names[[v]]
    log.p <- .Call("get_log_prior_density", object$ptr(), pn.param$names[[v]], 
      pn.param$lower[[v]], pn.param$upper[[v]], PACKAGE = "RBiips")
    
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
      stop("Negative infinite log prior density: invalid initial value: ", 
        var, "=", sample[[var]])
    
    log.prior <- log.prior + log.p
  }
  
  latent.monitored <- TRUE
  ## check latent variable is monitored
  if (length(latent.names) > 0) {
    if (!is_monitored(object, latent.names, "s", FALSE)) {
      ## monitor variables
      monitor.biips(object, latent.names, type = "s")
      latent.monitored <- FALSE
    }
  }
  
  ## build smc sampler
  if (!RBiips("is_sampler_built", object$ptr())) {
    RBiips("build_smc_sampler", object$ptr(), FALSE)
  }
  
  sampler.atend <- RBiips("is_smc_sampler_at_end", object$ptr())
  
  ## get log normalizing constant
  if (!sampler.atend || !latent.monitored) {
    ## run smc
    if (!sampler.atend) 
      RBiips("message", "Initializing PMMH") else if (!latent.monitored) 
      RBiips("message", "Initializing PMMH latent variables")
    if (!run_smc_forward(object, n_part = n_part, rs_thres = rs_thres, rs_type = rs_type)) 
      stop("run smc forward sampler: invalid initial values.")
  }
  log_marg_like <- RBiips("get_log_norm_const", object$ptr())
  
  if (is.nan(log_marg_like)) {
    stop("Failed to get log marginal likelihood: NaN.")
  }
  if (is.infinite(log_marg_like)) {
    stop("Failed to get log marginal likelihood: infinite value.")
  }
  
  ## get latent variable sampled value
  if (length(latent.names) > 0) {
    sampled.value <- RBiips("get_sampled_gen_tree_smooth_particle", object$ptr())
    if (length(sampled.value) == 0) {
      ## sample one particle
      rng.seed <- runif(1, 0, as.integer(Sys.time()))
      RBiips("sample_gen_tree_smooth_particle", object$ptr(), as.integer(rng.seed))
      
      sampled.value <- RBiips("get_sampled_gen_tree_smooth_particle", object$ptr())
    }
    for (var in latent.names) {
      sample[[var]] <- sampled.value[[var]]
    }
  }
  
  ans <- list(sample = sample, log.prior = log.prior, log_marg_like = log_marg_like)
  
  invisible(ans)
}


##' heart of the pmmh algorithm : realizes one step of the MH algorithm
##' using the underlying SMC
one.pmmh.update.biips <- function(object, param.names, latent.names = c(), pn.param, 
  sample, log.prior, log_marg_like, n_part, rw.rescale, rw.learn, rw.rescale.type, 
  ...) {
  n.fail <- 0
  
  ## Random walk proposal
  prop <- object$.rw_proposal(sample[param.names])
  
  ## check NA
  for (var in names(prop)) {
    if (any(is.na(prop[[var]]))) {
      stop("PMMH proposal have NA: ", var, " = ", prop[[var]])
    }
  }
  
  ## compute log prior density
  log.prior.prop <- 0
  
  for (v in seq(along = param.names)) {
    var <- param.names[[v]]
    
    ## change model data
    ok <- .Call("change_data", object$ptr(), pn.param$names[[v]], pn.param$lower[[v]], 
      pn.param$upper[[v]], prop[[var]], TRUE, PACKAGE = "RBiips")
    
    if (!ok) {
      log.prior.prop <- -Inf
      n.fail <- n.fail + 1
      warning("Failure changing data. proposal: ", var, "=", prop[[var]], "\n")
      break
    }
    
    log.p <- .Call("get_log_prior_density", object$ptr(), pn.param$names[[v]], 
      pn.param$lower[[v]], pn.param$upper[[v]], PACKAGE = "RBiips")
    
    if (is.na(log.p)) {
      stop("Failed to get log prior density: node ", var, "is not stochastic.")
    }
    
    log.prior.prop <- log.prior.prop + log.p
    
    if (log.prior.prop == -Inf) {
      break
    }
  }
  
  ## compute log marginal likelihood: run smc sampler
  log_marg_like_prop <- 0
  
  if (log.prior.prop != -Inf) {
    ok <- run_smc_forward(object, n_part = n_part, ...)
    
    if (!ok) {
      log_marg_like_prop <- -Inf
      n.fail <- n.fail + 1
      warning("Failure running smc forward sampler. proposal: ", paste(names(prop), 
        "=", prop, sep = "", collapse = "; "), "\n")
    } else {
      log_marg_like_prop <- RBiips("get_log_norm_const", object$ptr())
      if (is.nan(log_marg_like_prop)) {
        stop("Failed to get log marginal likelihood: NaN.")
      }
      if (log_marg_like_prop == Inf) {
        stop("Failed to get log marginal likelihood: +Inf.")
      }
    }
  }
  
  ## Acceptance rate
  accept.rate <- min(1, exp(log_marg_like_prop - log_marg_like + log.prior.prop - 
    log.prior))
  if (is.nan(accept.rate)) {
    stop("Failed to compute acceptanc rate: NaN.")
  }
  
  ## Accept/Reject step
  accepted <- (runif(1) < accept.rate)
  if (accepted) {
    sample <- prop
    log.prior <- log.prior.prop
    log_marg_like <- log_marg_like_prop
    
    if (length(latent.names) > 0) {
      ## sample one particle for the latent variables
      rng.seed <- runif(1, 0, as.integer(Sys.time()))
      RBiips("sample_gen_tree_smooth_particle", object$ptr(), as.integer(rng.seed))
      
      ## get sampled value
      sampled.value <- RBiips("get_sampled_gen_tree_smooth_particle", object$ptr())
      for (var in latent.names) {
        sample[[var]] <- sampled.value[[var]]
      }
    }
  }
  
  ## rescale random walk step
  object$.rw_rescale(accept.rate, rw.rescale.type)
  ## update random walk covariance matrices
  object$.rw_learn_cov(sample[param.names], accepted)
  
  ans <- list(sample = sample, log.prior = log.prior, log_marg_like = log_marg_like, 
    accept.rate = accept.rate, accepted = accepted, n.fail = n.fail)
  invisible(ans)
}


##' @export
pmmh.update <- function(object, ...) UseMethod("pmmh.update")


##' Update Particle Marginal Metropolis-Hastings samples
##' 
##' The \code{pmmh.update} function creates monitors for the given variables,
##' runs the model for \code{n_iter} iterations and returns the monitored
##' samples.
##' 
##' @param model a biips model object
##' @param param.names character vector. names of the variables uptaded with MCMC
##' @param n_iter integer. number of iterations of the Markov chain
##' @param n_part integer. number of particles of the SMC
##' @param max_fail integer. maximum number of failures allowed
##' @param inits named list of initial values for the variables in param.names.
##' If empty, inits are sampled from the prior.
##' @param rw_step positive steps of the random walk (std. dev. of the proposal
##' kernel). If numeric,  the value is duplicated for all variables.
##' If named list, the numeric components are assigned to the named variables.
##' If unnamed list, the numeric components are assigned to the variables in 
##' param.names with same ordering.
##' @param rw.rescale boolean. Toggle the rescaling of the rw.step.
##' @param rw.learn boolean. Toggle the online learning the empirical covariance
##' matrix of the parameters
##' @param ... additional arguments to be passed to the SMC algorithm
##' @return A list of \code{\link[rjags:mcarray.object]{mcarray}}
##' objects, with one element for each element of the \code{variable.names}
##' argument.
##' @author Adrien Todeschini, Francois Caron
##' @seealso \code{\link{biips_model}}, \code{\link{pimh_samples}},
##' \code{\link{smc_samples}}
##' @keywords models
##' @export
##' @S3method pmmh.update biips
##' @examples
##' 
##' ## Should be DIRECTLY executable !! 
##' ##-- ==>  Define data, use random,
##' ##--  or do  help(data=index)  for the standard data sets.
##' 
pmmh.update.biips <- function(object, param.names, n_iter, n_part, max_fail = 0, 
  inits = list(), rw_step, rw.rescale = TRUE, rw.learn = TRUE, rw.rescale.type = "p", 
  n.rescale = n_iter/4, ...) {
  ### check arguments -------------------------------
  stopifnot(is.biips(object))
  stopifnot(is.numeric(n_iter), length(n_iter) == 1, n_iter > 0)
  n_iter <- as.integer(n_iter)
  stopifnot(is.logical(rw.rescale), length(rw.rescale) == 1)
  stopifnot(is.logical(rw.learn), length(rw.learn) == 1)
  rw.adapt <- rw.rescale | rw.learn
  stopifnot(is.numeric(n.rescale), length(n.rescale) == 1, n.rescale <= n_iter)
  n.rescale <- as.integer(n.rescale)
  
  ## check rw.step
  if (!missing(rw_step)) {
    rw.step.values <- list()
    
    if (is.numeric(rw_step)) {
      ## duplicate value for each variable
      for (n in param.names) {
        rw.step.values[[n]] <- rw.step
      }
    } else {
      if (!is.list(rw_step)) 
        stop("rw_step must be a numeric or list of numeric.")
      rw.step.values <- rw.step
      step.names <- names(rw.step.values)
      if (is.null(step.names)) {
        ## case unnamed list
        if (length(rw.step.values) != length(param.names)) 
          stop("rw_step length does not match param.names length.")
        names(rw.step.values) <- param.names
      } else {
        ## case named list
        if (any(nchar(step.names) == 0)) 
          stop("rw_step has unnamed values.")
        if (any(duplicated(step.names))) 
          stop("rw_step has duplicated names:", paste(step.names[duplicated(step.names)], 
          collapse = ", "))
        ## check missing rw.step
        miss.step <- !param.names %in% step.names
        if (any(miss.step)) {
          stop("Missing rw_step values for variable:", paste(param.names[miss.step], 
          collapse = ", "))
        }
        ## check unkown variables from rw.step
        unknown.step <- !step.names %in% param.names
        if (any(unknown.step)) {
          stop("Unkown variable in rw_step values:", paste(step.names[unknown.step], 
          collapse = ", "))
        }
      }
      ## check non numeric values
      nonnum.step <- !sapply(rw.step.values, is.numeric)
      if (any(nonnum.step)) 
        stop("Non numeric rw_step values for variables:", paste(names(rw.step.values)[nonnum.step], 
          collapse = ", "))
    }
  }
  
  ### stop biips verbosity
  verb <- RBiips("verbosity", 0)
  on.exit(RBiips("verbosity", verb))
  
  ### Initialize -------------------------------
  out <- pmmh.init.biips(object, param.names = param.names, n_part = n_part, inits = inits, 
    ...)
  
  pn.param <- parse.varnames(param.names)
  
  ### assign rw step
  if (!missing(rw_step)) {
    # if (!object$.rw_adapt()) { ### FIXME warning('Ignoring rw_step argument:
    # adaptation has been stopped.') } else {
    object$.rw_set_step(rw.step.values)
    # }
  }
  
  ### stop adaptation if necessary
  if (!rw.rescale) {
    object$.rw_rescale_off()
  }
  if (!rw.learn) {
    object$.rw_learn_off()
  }
  # if (!rw.adapt) { if (object$.rw_adapt() && !object$.rw_check_adapt())
  # message('NOTE: Stopping adaptation of the PMMH random walk.\n')
  # object$.rw_adapt_off() }
  
  ### display message and progress bar
  title <- ifelse(object$.rw_adapt(), "Adapting", "Updating")
  RBiips("message", paste(title, "PMMH with", n_part, "particles"))
  symbol <- ifelse(object$.rw_adapt(), "+", "*")
  bar <- RBiips("progress_bar", n_iter, symbol, "iterations")
  
  ## initialize counters
  n.fail <- 0
  n.accept <- 0
  accept.rate <- vector(length = n_iter)
  step <- list()
  
  ### Metropolis-Hastings iterations -------------------------------
  for (i in 1:n_iter) {
    ## iterate
    out <- one.pmmh.update.biips(object, param.names = param.names, pn.param = pn.param, 
      sample = out$sample, log.prior = out$log.prior, log_marg_like = out$log_marg_like, 
      n_part = n_part, rw.rescale = rw.rescale, rw.learn = rw.learn, rw.rescale.type = rw.rescale.type, 
      ...)
    
    ## advance progress bar
    RBiips("advance_progress_bar", bar, 1)
    
    ## increment counters
    n.fail <- n.fail + out$n.fail
    n.accept <- n.accept + out$accepted
    accept.rate[i] <- out$accept.rate
    step[[i]] <- object$.rw_get_step()
    
    ## check failures
    if (n.fail > max_fail) {
      stop("Number of failures exceeds max_fail: ", n.fail, " failures.")
    }
    
    # deactivate rescale and activate learning
    if (rw.rescale && rw.learn) {
      if (i == n.rescale) {
        object$.rw_rescale_off()
        object$.rw_learn_on()
      }
    }
  }
  
  ### turn off adaptation if checked
  if (object$.rw_check_adapt()) 
    object$.rw_adapt_off()
  
  ### reset log norm const and sampled values if not accepted to store the last value
  ### of loglikelihood in the biips model(using set_log_norm_const)
  if (n_iter > 0 && !out$accepted) {
    for (v in seq(along = param.names)) {
      var <- param.names[[v]]
      if (!.Call("change_data", object$ptr(), pn.param$names[[v]], pn.param$lower[[v]], 
        pn.param$upper[[v]], out$sample[[var]], TRUE, PACKAGE = "RBiips")) 
        stop("can not reset previous data: ", var, "=", out$sample[[var]])
    }
    RBiips("set_log_norm_const", object$ptr(), out$log_marg_like)
  }
  
  
  ans <- list(ar.mean = mean(accept.rate), accept.rate = accept.rate, n.fail = n.fail, 
    step = step)
  invisible(ans)
}




##' Generate Particle Marginal Metropolis-Hastings samples
##' 
##' The \code{pmmh.samples} function creates monitors for the given variables,
##' runs the model for \code{n_iter} iterations and returns the monitored
##' samples.
##' 
##' @param model a biips model object
##' @param param.names a character vector giving the variables uptaded with MCMC
##' @param latent.names a character vector giving the variables uptaded with SMC
##' that you want to monitor
##' @param n_iter number of iterations of the Markov chain
##' @param thin thinning interval for monitors
##' @param n_part number of particles of the SMC
##' @param max_fail maximum number of failures allowed
##' @param rw.rescale boolean. Toggle the rescaling of the rw.step.
##' @param rw.learn boolean. Toggle online learning the empirical covariance matrix of
##' the parameters
##' @param ... additional arguments to be passed to the SMC algorithm
##' @return A list of \code{\link[rjags:mcarray.object]{mcarray}}
##' objects, with one element for each element of the \code{variable.names}
##' argument.
##' @author Adrien Todeschini, Francois Caron
##' @seealso \code{\link{biips_model}}, \code{\link{pimh_samples}},
##' \code{\link{smc_samples}}
##' @keywords models
##' @export
##' @examples
##' 
##' ## Should be DIRECTLY executable !! 
##' ##-- ==>  Define data, use random,
##' ##--\tor do  help(data=index)  for the standard data sets.
##' 
pmmh.samples <- function(object, param.names, latent.names = c(), n_iter, thin = 1, 
  n_part, max_fail = 0, rw.rescale = FALSE, rw.learn = FALSE, rw.rescale.type = "p", 
  n.rescale = n_iter/4, ...) {
  stopifnot(is.biips(object))
  stopifnot(is.numeric(n_iter), length(n_iter) == 1, n_iter > 0)
  n_iter <- as.integer(n_iter)
  stopifnot(is.logical(rw.rescale), length(rw.rescale) == 1)
  stopifnot(is.logical(rw.learn), length(rw.learn) == 1)
  rw.adapt <- rw.rescale | rw.learn
  stopifnot(is.numeric(n.rescale), length(n.rescale) == 1, n.rescale <= n_iter)
  n.rescale <- as.integer(n.rescale)
  stopifnot(is.numeric(thin), length(thin) == 1, thin > 0)
  thin <- as.integer(thin)
  
  ## stop biips verbosity
  verb <- RBiips("verbosity", 0)
  on.exit(RBiips("verbosity", verb))
  
  ## Initialization ----------------
  out <- pmmh.init.biips(object, param.names = param.names, latent.names = latent.names, 
    n_part = n_part, ...)
  sample <- out$sample
  log.prior <- out$log.prior
  log_marg_like <- out$log_marg_like
  
  pn.param <- parse.varnames(param.names)
  
  ## reset log normalizing constant on exit
  on.exit(if (n_iter > 0 && !accepted) {
    RBiips("set_log_norm_const", object$ptr(), log_marg_like)
  }, add = TRUE)
  
  ans <- list()
  
  ## initialize counters
  n_samples <- 0
  n.fail <- 0
  accepted <- TRUE
  n.accept <- 0
  
  accept.rate <- vector(length = n_iter)
  step <- list()
  
  ### stop adaptation if necessary
  if (!rw.rescale) {
    object$.rw_rescale_off()
  }
  if (!rw.learn) {
    object$.rw_learn_off()
  }
  # if (!rw.adapt) { ## check adaptation if (object$.rw_adapt() &&
  # !object$.rw_check_adapt()) message('NOTE: Stopping adaptation of the PMMH
  # random walk.\n') ## turn off adaptation object$.rw_adapt_off() }
  
  ### display message and progress bar
  title <- ifelse(object$.rw_adapt(), " adaptive", "")
  RBiips("message", paste("Generating", title, " PMMH samples with ", n_part, " particles", 
    sep = ""))
  symbol <- ifelse(object$.rw_adapt(), "+", "*")
  bar <- RBiips("progress_bar", n_iter, symbol, "iterations")
  
  ## Metropolis-Hastings iterations -------------------------------
  for (i in 1:n_iter) {
    out <- one.pmmh.update.biips(object, param.names = param.names, latent.names = latent.names, 
      pn.param = pn.param, sample = sample, log.prior = log.prior, log_marg_like = log_marg_like, 
      n_part = n_part, rw.rescale = rw.rescale, rw.learn = rw.learn, rw.rescale.type = rw.rescale.type, 
      ...)
    sample <- out$sample
    log.prior <- out$log.prior
    log_marg_like <- out$log_marg_like
    accepted <- out$accepted
    n.fail <- n.fail + out$n.fail
    n.accept <- n.accept + out$accepted
    accept.rate[i] <- out$accept.rate
    step[[i]] <- object$.rw_get_step()
    
    ## advance progress bar
    RBiips("advance_progress_bar", bar, 1)
    
    if (n.fail > max_fail) {
      stop("Number of failures exceeds max_fail: ", n.fail, " failures.")
    }
    
    ## Store output
    if ((i - 1)%%thin == 0) {
      n_samples <- n_samples + 1
      for (var in c(param.names, latent.names)) ans[[var]] <- c(ans[[var]], 
        sample[[var]])
    }
    
    if (rw.rescale && rw.learn) {
      if (i == n.rescale) {
        object$.rw_rescale_off()
        object$.rw_learn_on()
      }
    }
  }
  
  clear_monitors(object, type = "s")
  
  ## reset log norm const and sampled value for the latent variables
  if (n_iter > 0 && !accepted) {
    for (v in seq(along = param.names)) {
      var <- param.names[[v]]
      if (!.Call("change_data", object$ptr(), pn.param$names[[v]], pn.param$lower[[v]], 
        pn.param$upper[[v]], sample[[var]], TRUE, PACKAGE = "RBiips")) 
        stop("can not reset previous data: ", var, "=", sample[[var]])
    }
    RBiips("set_log_norm_const", object$ptr(), log_marg_like)
    if (length(latent.names) > 0) 
      RBiips("set_sampled_gen_tree_smooth_particle", object$ptr(), sample[latent.names])
  }
  
  ## output mean acceptance rate
  ans[["ar.mean"]] <- mean(accept.rate)
  
  ## number of failures
  ans[["n.fail"]] <- n.fail
  
  ans[["step"]] <- step
  ans[["accept.rate"]] <- accept.rate
  
  ## set output dimensions
  for (var in c(param.names, latent.names)) {
    dim(ans[[var]]) <- c(dim(sample[[var]]), n_samples)
    names(dim(ans[[var]])) <- c(rep("", length(dim(sample[[var]]))), "iteration")
    class(ans[[var]]) <- "mcarray"
  }
  
  return(ans)
} 
