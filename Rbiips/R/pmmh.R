
pmmh_set_param <- function(object, ...) UseMethod("pmmh_set_param")

##' @export
pmmh_set_param.biips <- function(object, param_names, pn_param, values) {

  stopifnot(is.biips(object))

  console <- object$ptr()

  n_param <- length(pn_param$names)
  sample_param <- list()

  if (length(values) > 0) {
    stopifnot(length(values) == n_param)
    # take sample values in values parameter
    for (i in 1:n_param) {
      var <- param_names[[i]]
      stopifnot(!is.null(values[[var]]))
      ok <- Rbiips("change_data", console, pn_param$names[[i]], pn_param$lower[[i]],
        pn_param$upper[[i]], values[[var]], TRUE)
      if (!ok)
        stop("Data change failed: invalid value for variable ", var)
      sample_param[[var]] <- values[[var]]
    }
  } else {
    # or sample init value
    for (i in 1:n_param) {
      tryCatch({
        var <- param_names[[i]]
        sample_param[[var]] <- Rbiips("sample_data", console, pn_param$names[[i]],
          pn_param$lower[[i]], pn_param$upper[[i]], get_seed())
      }, error = function(e) {
        print(e)
        stop("CANNOT SAMPLE VARIABLE ", var, ": BUG TO BE FIXED")
      })
    }
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

  ## TODO check param_names and latent_names are valid

  ## check param_names
  stopifnot(is.character(param_names), length(param_names) > 0)
  pn_param <- parse_varnames(param_names)
  n_param <- length(param_names)
  if (any(duplicated(sapply(param_names, to_biips_vname))))
    stop("duplicated names in param_names")

  ## check latent_names
  if (length(latent_names) > 0) {
    stopifnot(is.character(latent_names))
    parse_varnames(latent_names)
    if (any(duplicated(sapply(latent_names, to_biips_vname))))
      stop("duplicated names in latent_names")
  }

  ## check inits
  stopifnot(is.list(inits))
  if (length(inits) > 0) {
    stopifnot(length(inits) == n_param, sapply(inits, is.numeric))
    stopifnot(sapply(inits, function(x) all(is.finite(x))))
    init_names <- names(inits)
    if (!is.null(step_names)) {
      stopifnot(init_names %in% param_names)
      if (any(duplicated(sapply(init_names, to_biips_vname))))
        stop("duplicated names in inits")
    }
  }

  ## check rw_step
  stopifnot(is.list(rw_step))
  if (length(rw_step) > 0) {
    stopifnot(length(rw_step) == n_param, sapply(rw_step, is.numeric))
    stopifnot(sapply(rw_step, function(x) all(is.finite(x) && x > 0)))
    step_names <- names(rw_step)
    if (!is.null(step_names)) {
      stopifnot(step_names %in% param_names)
      if (any(duplicated(sapply(step_names, to_biips_vname))))
        stop("duplicated names in rw_step")
    }
  }

  ## stop biips verbosity
  verb <- Rbiips("verbosity", 0)
  on.exit(Rbiips("verbosity", verb))

  ## display message
  Rbiips("message", "Initializing PMMH")

  ## Clone console
  model <- clone_model(object)

  ## Init the parameters of the random walk
  sample_param <- pmmh_set_param(model, param_names, pn_param, inits)

  state <- list(sample_param = sample_param, sample_latent = list(), log_marg_like = -Inf,
    log_prior = -Inf)

  ## store the dimensions of the variables
  fun_get_dim <- function(x) dim(as.array(x))
  sample_dim <- lapply(sample_param, fun_get_dim)
  sample_len <- sum(sapply(sample_dim, prod))

  # target_prob = target acceptance probability. The default seems to be a fairly
  # robust optimal value. Comes from Rosenthal 2009, Examples of Adaptives MCMC, p
  # 16. In one-dimensional case, we take 0.44 which is optimal in certain settings.
  target_prob <- if (sample_len == 1)
    0.44 else 0.234

  ## Init random walk stepsize for the part with diagonal covariance matrix
  if (length(rw_step) == 0) {
    ## default values
    rw_step <- lapply(sample_dim, function(x) 1/sqrt(sample_len) * array(1, dim = x))
  } else {
    ## check dimensions
    rw_step_dim <- lapply(rw_step, fun_get_dim)
    stopifnot(mapply(identical, rw_step_dim, sample_dim))
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
    model
  }, param_names = function() {
    param_names
  }, latent_names = function() {
    latent_names
  }, sample_param = function(sample) {
    if (!missing(sample)) state$sample_param <<- sample
    invisible(state$sample_param)
  }, sample_latent = function(sample) {
    if (!missing(sample)) state$sample_latent <<- sample
    invisible(state$sample_latent)
  }, log_prior = function(log_prior) {
    if (!missing(log_prior)) state$log_prior <<- log_prior
    invisible(state$log_prior)
  }, log_marg_like = function(log_marg_like) {
    if (!missing(log_marg_like)) state$log_marg_like <<- log_marg_like
    invisible(state$log_marg_like)
  }, n_iter = function(n_iter) {
    if (!missing(n_iter)) rw$n_iter <<- n_iter
    invisible(rw$n_iter)
  }, n_rescale = function() {
    rw$n_rescale
  }, rw_dim = function() {
    rw$dim
  }, rw_len = function() {
    rw$len
  }, rw_step = function() {
    exp(rw$log_step)
  }, rw_mean = function(rw_mean) {
    if (!missing(rw_mean)) rw$mean <<- rw_mean
    invisible(rw$mean)
  }, rw_cov = function(rw_cov) {
    if (!missing(rw_cov)) rw$cov <<- rw_cov
    invisible(rw$cov)
  }, rw_alpha = function() {
    rw$alpha
  }, rw_beta = function() {
    rw$beta
  }, rw_rescale = function(ar) {
    ar <- min(1, ar)

    rw$ar_mean <<- rw$ar_mean + 1 * (ar - rw$ar_mean)/rw$n_iter

    rw$log_step <<- rw$log_step + rw$alpha^(rw$n_iter) * (rw$ar_mean - rw$target_prob)

    return(invisible())
  }, rw_learn_cov = function(object) {
    sample_param <- rw$sample_param
    n_iter <- rw$n_iter
    n_cov <- rw$n_cov

    ## Concatenate all variables in a column vector
    sample_vec <- unlist(sample_param)

    if (n_iter == n_cov + 1) {
      rw$mean <<- sample_vec
      rw$cov <<- outer(sample_vec, sample_vec)
    } else if (n_iter > n_cov + 1) {
      ## Recursive update of the empirical mean and covariance matrix
      q <- (n_iter - n_cov - 1)/(n_iter - n_cov)
      q2 <- (n_iter - n_cov - 1)/(n_iter - n_cov)^2
      z <- sample_vec - rw$mean
      rw$cov <<- q * rw$cov + q2 * outer(z, z)
      rw$mean <<- q * rw$mean + (1 - q) * sample_vec
    }

    return(invisible())
  })

  class(obj_pmmh) <- "pmmh"

  return(obj_pmmh)
}


pmmh_rw_proposal <- function(object) {
  # concatenate all variables in a vector always in the order of rw$dim

  sample_vec <- c(unlist(object$sample_param()))

  ## Check dimension
  rw_len <- object$rw_len()
  stopifnot(length(sample_vec) == rw_len)

  n_rescale <- object$n_rescale()
  beta <- object$rw_beta()
  cov <- object$rw_cov()

  if (length(cov) == 0 || (n_iter < n_rescale) || (runif(1) < beta)) {
    # Proposal with diagonal covariance
    prop_vec <- sample_vec + object$rw_step() * rnorm(rw_len)
  } else {
    # proposal with learnt covariance
    eps <- 1e-05  # For numerical stability
    cov_chol <- t(chol(cov + eps * diag(1, nrow = rw_len)))
    ## TODO use pivot in chol ? check positive semi-definite ?
    prop_vec <- sample_vec + 2.38/sqrt(rw_len) * cov_chol %*% rnorm(rw_len)
  }

  # rearrange vectorized parameter to list of arrays with appropriate dimensions
  rw_dim <- object$rw_dim()
  prop <- list()
  from <- 1
  for (n in names(rw_dim)) {
    len <- prod(rw_dim[[n]])
    to <- from + len - 1
    prop[[n]] <- array(prop_vec[from:to], dim = rw_dim[[n]])
    from <- from + len
  }

  return(prop)
}




##' heart of the pmmh algorithm: realizes one step of the MH algorithm
##' using the underlying SMC
pmmh_one_update <- function(object, pn_param, n_part, rw_rescale, rw_learn, ...) {

  console <- object$model()$ptr()
  param_names <- object$param_names()
  latent_names <- object$latent_names()
  sample_param <- object$sample_param()
  sample_latent <- object$sample_latent()
  log_prior <- object$log_prior()
  log_marg_like <- object$log_marg_like()

  n_fail <- 0
  n_param <- length(param_names)
  n_latent <- length(latent_names)

  ## Increment iterations counter
  n_iter <- object$n_iter() + 1
  object$n_iter(n_iter)

  ## Random walk proposal
  prop <- pmmh_rw_proposal(object)

  ### TODO: check NA ?

  # for (var in names(prop)) { if (any(is.na(prop[[var]]))) { stop('PMMH proposal
  # have NA: ', var, ' = ', prop[[var]]) } }

  ## compute log prior density
  log_prior_prop <- 0

  for (i in 1:n_param) {
    ## change model data
    var <- param_names[[i]]
    stopifnot(!is.null(prop[[var]]))
    ok <- Rbiips("change_data", console, pn_param$names[[i]], pn_param$lower[[i]],
      pn_param$upper[[i]], prop[[var]], TRUE)

    if (!ok) {
      ## DATA CHANGE FAILED: proposed parameter value might be out of bounds ?  TODO:
      ## double check this is the case and there is no other reason.
      log_prior_prop <- -Inf
      # n_fail <- n_fail + 1

      # warning('Data change failed:', var, '=', prop[[var]], '\n')
      break
    }

    log_p <- Rbiips("get_log_prior_density", console, pn_param$names[[i]], pn_param$lower[[i]],
      pn_param$upper[[i]])

    if (is.na(log_p))
      stop("Failed to get log prior density: node ", var, " is not stochastic.")

    log_prior_prop <- log_prior_prop + log_p
  }

  if (is.nan(log_prior_prop)) {
    # accept_rate = 0
    stop("Failed to compute log prior density : ", log_prior_prop)
  }
  if (log_prior_prop == -Inf) {
    ## If proposal is not in the support of the prior
    accept_rate <- 0
  } else {
    ## compute log marginal likelihood: run smc sampler
    ok <- run_smc_forward(object$model(), n_part = n_part, ...)

    if (!ok) {
      log_marg_like_prop <- -Inf
      n_fail <- n_fail + 1
      warning("Failure running smc forward sampler\n")
    } else {
      log_marg_like_prop <- Rbiips("get_log_norm_const", console)
      if (is.nan(log_marg_like_prop) || log_marg_like_prop == Inf)
        ### TODO error or n_fail increment ?
      stop("Failed to compute log marginal likelihood: ", log_marg_like_prop)
    }
  }

  ## Acceptance rate
  accept_rate <- min(1, exp(log_marg_like_prop - log_marg_like + log_prior_prop -
    log_prior))
  if (is.nan(accept_rate))
    stop("Failed to compute acceptance rate: ", accept_rate)

  ## Accept-Reject step
  if (runif(1) < accept_rate) {
    sample_param <- prop
    log_prior <- log_prior_prop
    log_marg_like <- log_marg_like_prop

    if (n_latent > 0) {
      ## sample one particle for the latent variables
      sampled_value <- Rbiips("sample_gen_tree_smooth_particle", object$ptr(),
        get_seed())

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
  if (rw_rescale)
    object$rw_rescale(accept_rate)
  ## update random walk covariance matrices
  if (rw_learn)
    object$rw_learn_cov()

  return(list(accept_rate = accept_rate, n_fail = n_fail))
}





pmmh_algo <- function(object, ...) UseMethod("pmmh_algo")

##' @export
pmmh_algo.pmmh <- function(object, n_iter, n_part, return_samples, thin = 1, max_fail = 0,
  rw_adapt = FALSE, output = "p", ...) {
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

  stopifnot(is.character(output))
  output <- unlist(strsplit(output, NULL))
  output <- match.arg(output, c("p", "l", "a", "f", "s"), several.ok = several.ok)
  output <- unique(type)


  ## stop biips verbosity
  verb <- Rbiips("verbosity", 0)
  # reset verbosity when function terminates
  on.exit(Rbiips("verbosity", verb))

  ## Initialization -------------------------
  console <- object$model()$ptr()

  # monitor variables
  param_names <- object$param_names()
  latent_names <- object$latent_names()
  n_param <- length(param_names)
  n_latent <- length(latent_names)

  if (n_latent > 0)
    monitor(object$model(), latent_names, "s")

  # build smc sampler
  if (!Rbiips("is_sampler_built", console))
    Rbiips("build_smc_sampler", console, FALSE)

  # toggle rescaling adaptation
  rw_rescale <- rw_adapt && object$n_iter() < object$n_rescale()

  ## FIXME if (!rw.adapt) { ## check adaptation if (object$.rw_adapt() &&
  ## !object$.rw_check_adapt()) message('NOTE: Stopping adaptation of the PMMH
  ## random walk.\n') ## turn off adaptation object$.rw_adapt_off() }

  # set current param value to the model
  sample_param <- object$sample_param()
  pn_param <- parse_varnames(param_names)
  pmmh_set_param(object$model(), param_names, pn_param, sample_param)

  # Initialize counters
  n_samples <- ceil(n_iter/thin)
  ind_sample <- 0
  n_fail <- 0

  # Output structure with MCMC samples
  out <- list()
  if ("p" %in% output)
    out$log_marg_like_pen <- mcmcarray(dim = c(1, n_samples))
  if ("l" %in% output)
    out$log_marg_like <- mcmcarray(dim = c(1, n_samples))
  if ("a" %in% output)
    out$accept_rate <- mcmcarray(dim = c(1, n_samples))
  if ("s" %in% output) {
    len <- length(object$rw_step())
    out$rw_step <- mcmcarray(dim = c(len, n_samples))
  }

  if (return_samples) {
    for (var in param_names) {
      dimen <- dim(samples_param[[var]])
      out[[var]] <- mcmcarray(dim = c(dimen, n_samples))
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
  Rbiips("message", paste(mess, n_part, "particles"))
  bar <- Rbiips("progress_bar", n_iter, symbol, "iterations")
  ### TODO: display expected time of run

  ## Metropolis-Hastings iterations -------------------------------
  for (i in 1:n_iter) {
    out_step <- pmmh_one_update(object, pn_param = pn_param, n_part = n_part,
      rw_rescale = rw_rescale, rw_learn = rw_adapt, ...)

    n_fail <- n_fail + out_step$n_fail

    ## Check nb of failures FC: MODIFY THIS EVENTUALLY
    if (n_fail > max_fail)
      stop("Number of failures exceeds max_fail: ", n_fail, " failures.")

    # Stop rescale
    if (rw_rescale && (object$n_iter() == object$n_rescale())) {
      ## FIXME problem if n_rescale > n_iter should compare to total n_iter
      rw_rescale <- FALSE
    }

    ## Store output
    if ((i - 1)%%thin == 0) {
      ind_sample <- ind_sample + 1
      log_marg_like <- out_step$log_marg_like
      log_prior <- out_step$log_prior

      if ("p" %in% output)
        out$log_marg_like_pen[ind_sample] <- log_marg_like + log_prior
      if ("l" %in% output)
        out$log_marg_like[ind_sample] <- log_marg_like
      if ("a" %in% output)
        out$accept_rate[ind_sample] <- out_step$accept_rate
      if ("s" %in% output)
        out$rw_step[, ind_sample] <- exp(object$rw_step())

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
          out[[var]] <- mcmcarray(dim = c(dimen, n_samples))
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
    Rbiips("advance_progress_bar", bar, 1)
  }


  ## number of failures
  if ("f" %in% output)
    out$n_fail <- n_fail

  return(out)
}




##' Update Particle Marginal Metropolis-Hastings samples
##'
##' The \code{update.pmmh} function creates monitors for the given variables,
##' runs the model for \code{n_iter} iterations and returns the monitored
##' samples.
##'
##' @param model a biips model object
##' @param param_names character vector. names of the variables uptaded with MCMC
##' @param n_iter integer. number of iterations of the Markov chain
##' @param n_part integer. number of particles of the SMC
##' @param max_fail integer. maximum number of failures allowed
##' @param inits named list of initial values for the variables in param_names.
##' If empty, inits are sampled from the prior.
##' @param rw_step positive steps of the random walk (std. dev. of the proposal
##' kernel). If numeric,  the value is duplicated for all variables.
##' If named list, the numeric components are assigned to the named variables.
##' If unnamed list, the numeric components are assigned to the variables in
##' param_names with same ordering.
##' @param rw.rescale boolean. Toggle the rescaling of the rw.step.
##' @param rw.learn boolean. Toggle the online learning the empirical covariance
##' matrix of the parameters
##' @param ... additional arguments to be passed to the SMC algorithm
##' @return A list of \code{\link[rjags:mcmcarray.object]{mcmcarray}}
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
##' ##--  or do  help(data=index)  for the standard data sets.
##'
pmmh_update.pmmh <- function(object, n_iter, n_part, max_fail = 0, rw_adapt = TRUE,
                             output = "p", ...) {
  out <- pmmh_algo(object, n_iter, n_part, return_samples = FALSE, max_fail = max_fail,
    rw_adapt = rw_adapt, output = output, ...)
  return(invisible(out))
}


##' Generate Particle Marginal Metropolis-Hastings samples
##'
##' The \code{pmmh.samples} function creates monitors for the given variables,
##' runs the model for \code{n_iter} iterations and returns the monitored
##' samples.
##'
##' @param model a biips model object
##' @param param_names a character vector giving the variables uptaded with MCMC
##' @param latent_names a character vector giving the variables uptaded with SMC
##' that you want to monitor
##' @param n_iter number of iterations of the Markov chain
##' @param thin thinning interval for monitors
##' @param n_part number of particles of the SMC
##' @param max_fail maximum number of failures allowed
##' @param rw.rescale boolean. Toggle the rescaling of the rw.step.
##' @param rw.learn boolean. Toggle online learning the empirical covariance matrix of
##' the parameters
##' @param ... additional arguments to be passed to the SMC algorithm
##' @return A list of \code{\link[rjags:mcmcarray.object]{mcmcarray}}
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
pmmh_samples <- function(object, n_iter, n_part, thin = 1, max_fail = 0, output = "p", ...) {

  out <- pmmh_algo(object, n_iter, n_part, return_samples = TRUE, max_fail = max_fail,
                   output = output, ...)
  return(out)
}
