
pmmh_set_param <- function(object, pn_param, inits, change_data = FALSE) {
  
  ## make init sample
  sample_param <- list()
  data <- object$.data_sync()
  if (length(inits) > 0) {
    # take init value in inits param
    for (i in 1:length(inits)) {
      if (change_data) {
        ok <- RBiips("change_data", object$ptr(), pn_param$names[[i]], pn_param$lower[[i]], 
                     pn_param$upper[[i]], inits[[i]], TRUE)
        if (!ok) 
          stop("Data change failed: invalid initial value for variable ", 
               pn_param$names[[i]])
        ### TODO more precise error message
        
      }
      sample_param[[i]] <- inits[[i]]
    }
  } else {
    # or sample init value
    for (i in 1:length(inits)) sample_param[[i]] <- RBiips("sample_data", object$ptr(), 
                                                           pn_param$names[[i]], pn_param$lower[[i]], pn_param$upper[[i]], get_seed())
    #### FIXME what if the variable is observed ?
  }
  return(invisible(sample_param))
}


##' @export
pmmh_init <- function(object, param_names, latent_names = c(), inits = list(), rw_step = list(), 
                      n_adapt = 400, alpha = 0.99, beta = 0.05, ...) {
  ## check arguments
  stopifnot(is.biips(object))
  stopifnot(is.numeric(n_adapt), length(n_adapt) == 1, n_adapt >= 1, is.finite(n_adapt))
  stopifnot(is.numeric(alpha), length(alpha) == 1, alpha >= 0, alpha <= 1)
  stopifnot(is.numeric(beta), length(beta) == 1, beta >= 0, beta <= 1)
  ## TODO check param_names and latent_names
  
  ## check param_names
  stopifnot(is.character(param_names), length(param_names) > 0)
  if (any(duplicated(param_names))) 
    stop("duplicated names in param_names: ", paste(param_names[duplicated(param_names)], 
                                                    collapse = " "))
  pn_param <- parse_varnames(param_names)
  n_param <- length(param_names)
  
  ## check latent_names
  if (!is.null(latent_names)) {
    stopifnot(is.character(latent_names))
    if (any(duplicated(latent_names))) 
      stop("duplicated names in latent_names: ", paste(latent_names[duplicated(latent_names)], 
                                                       collapse = " "))
    parse_varnames(latent_names)
  }
  
  ## check inits
  stopifnot(is.list(inits))
  if (length(inits) > 0) {
    stopifnot(length(inits) == n_param, sapply(inits, is.numeric))
  }
  
  ## check rw_step
  stopifnot(is.list(rw_step))
  if (length(rw_step) > 0) {
    stopifnot(length(rw_step) == n_param, sapply(rw_step, is.numeric))
  }
  
  ## stop biips verbosity
  verb <- RBiips("verbosity", 0)
  on.exit(RBiips("verbosity", verb))
  
  RBiips("message", "Initializing PMMH")
  
  ## Clone console
  model <- clone_model(object)
  
  ## Init the parameters of the random walk
  sample_param <- pmmh_set_param(model, pn_param, inits, TRUE)
  
  state <- list(sample_param = sample_param, sample_latent = list(), log_marg_like = -Inf, 
                log_prior = -Inf)
  
  ## store the dimensions of the variables
  fun_get_dim <- function(x) {
    if (is.null(dim(x))) 
      length(x) else dim(x)
  }
  sample_dim <- lapply(sample, fun_get_dim)
  sample_len <- sum(sapply(sample_dim, prod))
  
  # target_prob = target acceptance probability. The default seems to be a fairly
  # robust optimal value. Comes from Rosenthal 2009, Examples of Adaptives MCMC, p
  # 16. In one-dimensional case, we take 0.44 which is optimal in certain settings.
  target_prob <- if (sample_len == 1) 
    0.44 else 0.234
  
  ## Init random walk stepsize for the part with diagonal covariance matrix
  if (length(rw_step) == 0) {
    ## default values
    rw_step <- lapply(sample, function(x) 1/sqrt(sample_len) * array(1, dim = x))
  } else {
    ## check dimensions
    rw_step_dim <- lapply(rw_step, fun_get_dim)
    stopifnot(mapply(identical, rw_step_dim, sample_dim))
    ## check values
    stopifnot(sapply(rw_step, function(x) all(is.finite(x) && x > 0)))
  }
  
  ## Concatenate all log value
  log_step <- log(unlist(rw_step))
  
  # local initial parameters of the algorithm of adaptation of the step of random
  # walk in the PMMH algorithm
  rw <- list(n_rescale = n_rescale, n_cov = n_rescale/2, n_iter = 0, ar_mean = 0, 
             log_step = log_step, beta = 0.05, alpha = alpha, target_prob = target_prob, 
             dim = list(), len = sample_len, mean = c(), cov = c())
  # we start learning the covariance matrix after n_cov iterations
  
  obj_pmmh <- list(model = function() {
    return(model)
  }, param_names = function() {
    return(param_names)
  }, latent_names = function() {
    return(latent_names)
  }, sample_param = function(sample) {
    if (!missing(sample)) state$sample_param <<- sample
    return(invisible(state$sample_param))
  }, sample_latent = function(sample) {
    if (!missing(sample)) state$sample_latent <<- sample
    return(invisible(state$sample_latent))
  }, log_prior = function(log_prior) {
    if (!missing(log_prior)) state$log_prior <<- log_prior
    return(invisible(state$log_prior))
  }, log_marg_like = function(log_marg_like) {
    if (!missing(log_marg_like)) state$log_marg_like <<- log_marg_like
    return(invisible(state$log_marg_like))
  }, n_iter = function() {
    return(rw$n_iter)
  }, n_rescale = function() {
    return(rw$n_rescale)
  }, rw_step = function() {
    return(exp(rw$log_step))
  }, rw_proposal = function() {
    return(prop)
  }, rw_rescale = function(ar) {
    return(invisible())
  }, rw_learn_cov = function() {
    return(invisible())
  })
  
  class(obj_pmmh) <- "pmmh"
  
  return(obj_pmmh)
}



pmmh_one_update <- function(object, ...) UseMethod("pmmh_one_update")

##' @S3method pmmh_one_update pmmh
##' heart of the pmmh algorithm : realizes one step of the MH algorithm
##' using the underlying SMC
pmmh_one_update.pmmh <- function(object, pn_param, n_part, 
                                 rw_rescale, rw_learn, ...) {
  
  console = object$model()$ptr()
  param_names <- object$param_names()
  latent_names <- object$latent_names()
  sample_param <- object$sample_param()
  sample_latent <- object$sample_latent()
  log_prior <- object$log_prior()
  log_marg_like <- object$log_marg_like()
  
  n_fail = 0
  n_param = length(param_names)
  n_latent = length(latent_names)
  
  ## Random walk proposal
  prop = object$rw_proposal()
  
  # TODO: check this is not necessary
  #   ## check NA
  #   for (var in names(prop)) {
  #     if (any(is.na(prop[[var]]))) {
  #       stop("PMMH proposal have NA: ", var, " = ", prop[[var]])
  #     }
  #   }
  
  ## compute log prior density
  log_prior_prop <- 0
  
  for (i in 1:n_param) {
    ## change model data
    ok <- RBiips("change_data", console, pn_param$names[[i]], pn_param$lower[[i]], 
                 pn_param$upper[[i]], prop[[i]], TRUE)
    
    if (!ok) {
      ## DATA CHANGE FAILED: proposed parameter value might be out of bounds ?
      ## TODO: double check this is the case and there is no other reason.
      log_prior_prop <- -Inf
#       n_fail <- n_fail + 1
#       warning("Data change failed:", param_names[[i]], "=", prop[[var]], "\n")
      break
    }
    
    log_p <- RBiips("get_log_prior_density", console, pn_param$names[[v]], 
                    pn_param$lower[[v]], pn_param$upper[[v]])
    
    if (is.na(log_p))
      stop("Failed to get log prior density: node ", param_names[[i]], "is not stochastic.")
    
    log_prior_prop <- log_prior_prop + log_p
  }
  
  if (is.nan(log_prior_prop)) {
#     accept_rate = 0
    stop('Failed to compute log prior density : ', log_prior_prop)
  }
  if (log_prior_prop == -Inf) {
    ## If proposal is not in the support of the prior
    accept_rate = 0
  } else {
    ## compute log marginal likelihood: run smc sampler
    ok <- run_smc_forward(object, n_part = n_part, ...)
    
    if (!ok) {
      log_marg_like_prop <- -Inf
      n_fail <- n_fail + 1
      warning("Failure running smc forward sampler\n")
    } else {
      log_marg_like_prop <- RBiips("get_log_norm_const", console)
      if (is.nan(log_marg_like_prop) || log_marg_like_prop == Inf)
        ### TODO error or n_fail increment ?
        stop("Failed to compute log marginal likelihood: ", log_marg_like_prop)
    }
  }
  
  ## Acceptance rate
  accept_rate <- min(1, exp(log_marg_like_prop - log_marg_like + log_prior_prop - 
                              log_prior))
  if (is.nan(accept_rate))
    stop("Failed to compute acceptance rate:" , accept_rate)
  
  ## Accept-Reject step
  if (runif(1) < accept_rate) {
    sample_param <- prop
    log_prior <- log_prior_prop
    log_marg_like <- log_marg_like_prop
    
    if (n_latent > 0) {
      ## sample one particle for the latent variables
      sampled_value <- RBiips("sample_gen_tree_smooth_particle", object$ptr(), get_seed())
      
      for (var in latent_names) {
        var_in <- to_biips_vname(var)
        sample_latent[[var]] <- sampled_value[[var_in]]
      }
    }
  }
  
  ## Update PMMH object with current state 
  object$sample_param(sample_param)
  object$sample_latent(sample_latent)
  object$log_prior(log_prior)
  object$log_marg_like(log_marg_like)
  
  ## rescale random walk step
  if (rw_rescale) {
    object$rw_rescale(accept_rate)
  }
  ## update random walk covariance matrices
  if (rw_learn) {
    object$rw_learn_cov()
  }
  
  return(list(accept_rate=accept_rate, n_fail=n_fail))
}






pmmh_algo <- function(object, ...) UseMethod("pmmh_algo")

##' @S3method pmmh_algo pmmh
pmmh_algo.pmmh <- function(object, n_iter, n_part, return_samples, thin = 1, max_fail = 0, 
                           rw_adapt = FALSE, ...) {
  stopifnot(is.biips(object))
  stopifnot(is.numeric(n_iter), length(n_iter) == 1, n_iter > 0, is.finite(n_iter))
  n_iter <- as.integer(n_iter)
  stopifnot(is.numeric(n_part), length(n_part) == 1, n_part >= 1, is.finite(n_part))
  n_part <- as.integer(n_part)
  stopifnot(is.logical(return_samples), length(return_samples) == 1)
  stopifnot(is.numeric(thin), length(thin) == 1, thin >= 1, thin <= n_iter)
  thin <- as.integer(thin)
  stopifnot(is.numeric(max_fail), length(max_fail) == 1, max_fail > 0)
  max_fail <- as.integer(max_fail)
  stopifnot(is.logical(rw_adapt), length(rw_adapt) == 1)
  
  ## stop biips verbosity
  verb <- RBiips("verbosity", 0)
  # reset verbosity when function terminates
  on.exit(RBiips("verbosity", verb))
  
  ## Initialization -------------------------
  console <- object$model()$ptr()
  
  # monitor variables
  param_names <- object$param_names()
  latent_names <- object$latent_names()
  n_param <- length(param_names)
  n_latent <- length(latent_names)
  
  if (n_latent > 0) 
    monitor(console, latent_names, "s")
  
  # build smc sampler
  if (!RBiips("is_sampler_built", console)) 
    RBiips("build_smc_sampler", console, FALSE)
  
  # toggle rescaling adaptation
  rw_rescale <- rw_adapt && object$n_iter() < object$n_rescale()
  
  ## FIXME
  # if (!rw.adapt) { ## check adaptation if (object$.rw_adapt() &&
  # !object$.rw_check_adapt()) message('NOTE: Stopping adaptation of the PMMH
  # random walk.\n') ## turn off adaptation object$.rw_adapt_off() }
  
  # set current param value to the model
  sample_param <- object$sample_param()
  pn_param <- parse_varnames(param_names)
  pmmh_set_param(console, pn_param, sample_param, TRUE)
  
  # Initialize counters
  n_samples <- ceil(n_iter/thin)
  ind_sample <- 0
  n_fail <- 0
  
  # Output structure with MCMC samples
  out <- list()  
  out$accept_rate <- mcarray(dim = c(1, n_samples))
  len <- length(object$rw_step())
  out$rw_step <- mcarray(dim = c(len, n_samples))
  out$log_marg_like <- mcarray(dim = c(1, n_samples))
  out$log_post <- mcarray(dim = c(1, n_samples))
  
  if (return_samples) {
    for (var in param_names) {
      dimen <- dim(samples_param[[var]])
      out[[var]] <- mcarray(dim = c(dimen, n_samples))
    }
  }
  
  ## display message and progress bar
  mess <- if (return_samples) 
    "Generating PMMH samples with" else {
      if (rw_adapt) 
        "Adapting PMMH with" else "Updating PMMH with"
    }
  symbol <- if (rw_adapt) 
    "+" else "*"
  RBiips("message", paste(mess, n_part, "particles"))
  bar <- RBiips("progress_bar", n_iter, symbol, "iterations")
  ### TODO: display expected time of run
  
  ## Metropolis-Hastings iterations -------------------------------
  for (i in 1:n_iter) {
    out_step <- pmmh_one_update(object, pn_param = pn_param, 
                                n_part = n_part, 
                                rw_rescale = rw_rescale, rw_learn = rw_adapt, ...)
    
    n_fail <- n_fail + out_step$n_fail
    
    ## Check nb of failures FC: MODIFY THIS EVENTUALLY
    if (n_fail > max_fail)
      stop("Number of failures exceeds max_fail: ", n_fail, " failures.")
    
    # Stop rescale
    if (rw_rescale && (object$n_iter()==object$n_rescale())) {
      ## FIXME problem if n_rescale > n_iter
      ## should compare to total n_iter
      rw_rescale <- FALSE
    }
    
    ## Store output
    if ((i - 1)%%thin == 0) {
      ind_sample <- ind_sample + 1
      log_marg_like <- out_step$log_marg_like
      log_prior <- out_step$log_prior
      
      out$accept_rate[ind_sample] <- out_step$accept_rate
      out$rw_step[,ind_sample] <- exp(object$rw_step())
      out$log_marg_like[ind_sample] <- log_marg_like
      out$log_post[ind_sample] <- log_marg_like + log_prior
      
      if (return_samples) {
        sample_param <- out_step$sample_param
        sample_latent <- out_step$sample_latent
        
        for (var in param_names) {
          len <- length(sample_param[[var]])
          from <- (ind_sample - 1) * len + 1
          to <- (ind_sample - 1) * len + len
          out[[var]][from:to] <- sample_param[[var]]
        }
        
        if (ind_samples == 1) {
          ## pre-allocation here to be sure that sample is not empty
          for (var in latent_names) {
            dimen <- dim(sample_latent[[var]])
            out[[var]] <- mcarray(dim = c(dimen, n_samples))
          }
        }
        
        for (var in latent_names) {
          len <- length(sample_latent[[var]])
          from <- (ind_sample - 1) * len + 1
          to <- (ind_sample - 1) * len + len
          out[[var]][from:to] <- sample_latent[[var]]
        }
      }
    }
    
    ## advance progress bar
    RBiips("advance_progress_bar", bar, 1)
  }
  
  
  ## number of failures
  out$n_fail <- n_fail
  
  return(out)
} 
