
#' @keywords internal
pmmh_set_param <- function(object, ...) UseMethod("pmmh_set_param")

#' @keywords internal
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
#       tryCatch({
        var <- param_names[[i]]
        sample_param[[var]] <- Rbiips("sample_data", console, pn_param$names[[i]],
          pn_param$lower[[i]], pn_param$upper[[i]], get_seed())
#       }, error = function(e) {
#         print(e)
#         stop("Can not sample variable ", var, ": BUG TO BE FIXED")
#       })
    }
    #### FIXME what if the variable is observed ?
  }
  return(invisible(sample_param))
}


#' @title Create PMMH objects
#'
#' @description The function \code{biips_pmmh_init} initializes the Particle Marginal
#' Metropolis-Hastings (PMMH) algorithm.
#'
#' The PMMH algorithm is a particle MCMC algorithm using SMC within a MCMC
#' algorithm. It splits the variables in the graphical model into two sets:
#' \itemize{
#'   \item The parameters in \code{param_names} are sampled with a MH proposal
#'   (multivariate Gaussian random walk). They must be continuous and must be defined as
#'   single stochastic nodes in the model.
#'   \item The other latent variables are sampled using a SMC algorithm. They can
#'   be monitored using the \code{latent_names} parameter.
#' }
#'
#' @export
#' @param model \code{biips} model object as returned by
#'   \code{\link{biips_model}}.
#' @param param_names  character vector. The names of the variables updated with
#'   MH proposal. The names can contain subset indices which must define a valid
#'   subset of the variables of the model, e.g.: \code{c('var1',} \code{'var2[1]',}
#'   \code{'var3[1:10]',} \code{'var4[1, 5:10, 3]')}.
#' @param latent_names  character vector. The names of the variables to be
#'   updated with SMC proposal that need to be monitored.
#' @param inits       list of numeric values. Initial values for the parameters
#'   in \code{param_names}. (default = samples from the prior distribution).
#'   \code{inits} can either be a named list with no unnamed member or an
#'   unnamed list of the same length as \code{param_names}.
#' @param transform   logical. Activate automatic parameters transformation
#'   (default = \code{TRUE}). Transformations apply independently to each
#'   component of the parameters depending on their support: \itemize{ \item
#'   unbounded (-\eqn{\infty}, +\eqn{\infty}): f(x) = x \item lower bounded [L,
#'   +\eqn{\infty}): f(x) = log(x-L) \item upper bounded (-\eqn{\infty}, U]:
#'   f(x) = log(U-x) \item lower-upper bounded [L, U]: f(x) = log((x-L)/(U-x)) }
#'   so that we apply a random walk on the real-valued transformed vector.
#' @param rw_step   list of numeric values. Random walk standard deviations.
#'   \code{rw_step} can either be a named list with no unnamed member or an
#'   unnamed list of the same length as \code{param_names}. If \code{transform}
#'   is \code{TRUE} (default), the given standard deviations apply to the transformed variables.
#'   Set \code{transform} to \code{FALSE} if you wish to set the random walk standard deviation for the
#'   untransformed random variables.
#' @param n_rescale     integer. Number of iterations for rescaling (default =
#'   400).
#' @param alpha         real in [0,1]. Tuning parameter of the rescaling
#'   (default = 0.99).
#' @param beta          real in [0,1]. Tuning parameter of the proposal (default
#'   = 0.05).
#'
#' @return The function \code{biips_pmmh_init} returns an object of class
#'   \code{pmmh} which can be used to generate samples from the posterior
#'   distribution of the monitored variables in \code{param_names} and
#'   \code{latent_names}.
#'
#'   An object of class \code{pmmh} is a list of functions that share a common
#'   environment. These functions are meant for internal purpose only. They are
#'   used to query information on the current state of the algorithm.
#'   \item{model()}{Get the \code{\link{biips}} model object.}
#'   \item{param_names()}{Get a character vector with the names of the monitored
#'     parameters.}
#'   \item{latent_names()}{Get a character vector with the names of
#'     the monitored latents.}
#'   \item{sample_param(sample)}{Get and set the current sample value of the parameters.}
#'   \item{sample_param_tr(sample)}{Get and set the current sample value of the
#'     transfromed parameters.}
#'   \item{sample_latent(sample)}{Get and set the current sample value of the
#'     latents.}
#'   \item{log_prior(log_prior)}{Get and set the current value of the
#'     log prior of the parameters.}
#'   \item{log_marg_like(log_marg_like)}{Get and set the current value of
#'     the log marginal likelihood.}
#'   \item{n_iter()}{Get and set the current curent number of iterations.}
#'   \item{n_rescale()}{Get the nb of iterations for rescaling.}
#'   \item{rw_dim()}{Get the dimensions of the parameters.}
#'   \item{rw_len()}{Get the total length of the parameters.}
#'   \item{rw_step()}{Get the random walk standard deviations for the
#'     rescaling.}
#'   \item{rw_cov()}{Get the empirical covariance matrix of the samples.}
#'   \item{rw_alpha()}{Get the tuning parameter of the rescaling.}
#'   \item{rw_beta()}{Get the tuning parameter of the proposal.}
#'   \item{rw_transform(sample, funtype = 'transform')}{Applies transformation
#'     function defined by the \code{funtype} string argument. Possible values are
#'     \code{'transform'} (direct transformation), \code{'inverse'} (inverse
#'     transformation) and \code{'lderiv'} (log derivative of the transformation).}
#'   \item{rw_rescale(ar)}{rescales the random walk step to target an optimal
#'     acceptance rate.}
#'   \item{rw_learn_cov()}{update the empirical covariance matrix of the samples.}
#'
#' @seealso \code{\link{biips_model}}, \code{\link{biips_pmmh_update}},
#'   \code{\link{biips_pmmh_samples}}
#'
#' @examples
#' modelfile <- system.file('extdata', 'hmm.bug', package = 'Rbiips')
#' stopifnot(nchar(modelfile) > 0)
#' cat(readLines(modelfile), sep = '\n')
#'
#' data <- list(tmax = 10, p = c(.5, .5), logtau_true = log(1))
#' model <- biips_model(modelfile, data)
#'
#' n_part <- 50
#' obj_pmmh <- biips_pmmh_init(model, 'logtau', latent_names = c('x', 'c[2:10]'),
#'                             inits = list(logtau = -2))  # Initialize
#' is.pmmh(obj_pmmh)
#' out_pmmh_burn <- biips_pmmh_update(obj_pmmh, 100, n_part)  # Burn-in
#' out_pmmh <- biips_pmmh_samples(obj_pmmh, 100, n_part, thin = 1)  # Samples
biips_pmmh_init <- function(model, param_names, latent_names = c(), inits = list(),
  rw_step = list(), transform = TRUE, n_rescale = 400, alpha = 0.99, beta = 0.05) {
  ## check arguments
  stopifnot(is.biips(model))
  stopifnot(is.logical(transform), length(transform) == 1)
  stopifnot(is.numeric(n_rescale), length(n_rescale) == 1, n_rescale >= 1, is.finite(n_rescale))
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
    if (is.null(init_names))
      names(inits) <- param_names else {
      stopifnot(init_names %in% param_names)
      if (any(duplicated(sapply(init_names, to_biips_vname))))
        stop("duplicated names in inits")
    }
    inits <- inits[param_names]
  }

  ## check rw_step
  stopifnot(is.list(rw_step))
  if (length(rw_step) > 0) {
    stopifnot(length(rw_step) == n_param, sapply(rw_step, is.numeric))
    stopifnot(sapply(rw_step, function(x) all(is.finite(x) && x > 0)))
    step_names <- names(rw_step)
    if (is.null(step_names)) {
      names(rw_step) <- param_names
    } else {
      stopifnot(step_names %in% param_names)
      if (any(duplicated(sapply(step_names, to_biips_vname))))
        stop("duplicated names in rw_step")
    }
    rw_step <- rw_step[param_names]
  }

  ## stop biips verbosity
  verb <- Rbiips("verbosity", 0)
  on.exit(Rbiips("verbosity", verb))

  ## display message
  Rbiips("message", "Initializing PMMH")

  ## Clone console
  model <- clone_model(model)

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
    rw_step <- lapply(sample_dim, function(x) 0.1/sqrt(sample_len) * array(1,
      dim = x))
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
    log_step = log_step, beta = beta, alpha = alpha, target_prob = target_prob,
    dim = sample_dim, len = sample_len, mean = c(), cov = c(), transform = list(),
    transform_inv = list(), transform_lderiv = list())
  # we start learning the covariance matrix after n_cov iterations

  ## Define variable transformations
  k <- 0
  for (i in 1:n_param) {
    var <- param_names[[i]]
    len <- prod(sample_dim[[i]])
    if (transform) {
#       tryCatch({
        support <- Rbiips("get_fixed_support", model$ptr(), pn_param$names[[i]],
          pn_param$lower[[i]], pn_param$upper[[i]])
#       }, error = function(e) {
#         print(e)
#         stop("CANNOT GET FIXED SUPPORT OF VARIABLE ", var, ": BUG TO BE FIXED")
#       })
      if (nrow(support) != len)
        stop("support size does not match")
    } else {
      support <- matrix(c(rep(-Inf, len), rep(+Inf, len)), nrow = len)
    }
    for (j in 1:len) {
      k <- k + 1
      L <- support[j, 1]
      U <- support[j, 2]
      if (is.finite(L)) {
        if (is.finite(U)) {
          # lower-upper bounded support: logit transform
          rw$transform[[k]] <- function(l,u){ l; u;
                                              function(x) log((x - l)/(u - x))
                                              }(L, U)  # direct transformation
          rw$transform_inv[[k]] <- function(l,u){ l; u;
                                                  function(y) l + (u - l)/(1 + exp(-y))
                                                  }(L, U)  # inverse transformation
          rw$transform_lderiv[[k]] <- function(l,u){ l; u;
                                                    function(x) log(u - l) - log(x - l) - log(u - x)
                                                    }(L, U)  # log derivative
        } else {
          if (U < 0)
          stop("upper can not be -Inf")
          # lower bounded support: -log transform
          rw$transform[[k]] <- function(l){ l; function(x) log(x - l) }(L)
          rw$transform_inv[[k]] <- function(l){ l; function(y) l + exp(y) }(L)
          rw$transform_lderiv[[k]] <- function(l){ l; function(x) -log(x - l) }(L)
        }
      } else {
        if (L > 0)
          stop("lower can not be +Inf")
        if (is.finite(U)) {
          # upper bounded support: log transform
          rw$transform[[k]] <-  function(u){ u; function(x) log(u - x) }(U)
          rw$transform_inv[[k]] <-  function(u){ u; function(y) u - exp(y) }(U)
          rw$transform_lderiv[[k]] <-  function(u){ u; function(x) -log(u - x) }(U)
        } else {
          if (U < 0)
          stop("upper can not be -Inf")
          # upper bounded support: identity
          rw$transform[[k]] <- function(x) x
          rw$transform_inv[[k]] <- function(y) y
          rw$transform_lderiv[[k]] <- function(x) 0
        }
      }
    }
  }

  # function that applies transformation functions to samples
  rw_transform <- function(sample, funtype = "transform") {

    funlist <- switch(funtype, transform = rw$transform, inverse = rw$transform_inv,
      lderiv = rw$transform_lderiv)

    # concatenate all variables in a vector always in the order of rw$dim
    sample_vec <- c(unlist(sample))

    out_vec <- mapply(FUN = function(x, fun) fun(x), sample_vec, funlist)

    # rearrange vectorized parameter to list of arrays with appropriate dimensions
    out <- list()
    from <- 1
    for (n in names(rw$dim)) {
      len <- prod(rw$dim[[n]])
      to <- from + len - 1
      out[[n]] <- array(out_vec[from:to], dim = rw$dim[[n]])
      from <- from + len
    }
    return(out)
  }

  state$sample_param_tr <- rw_transform(sample_param)

  ## output object
  obj_pmmh <- list(model = function() {
    model
  }, param_names = function() {
    param_names
  }, latent_names = function() {
    latent_names
  }, sample_param = function(sample) {
    if (!missing(sample)) state$sample_param <<- sample
    invisible(state$sample_param)
  }, sample_param_tr = function(sample_tr) {
    if (!missing(sample_tr)) state$sample_param_tr <<- sample_tr
    invisible(state$sample_param_tr)
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
  }, rw_cov = function() {
    rw$cov
  }, rw_alpha = function() {
    rw$alpha
  }, rw_beta = function() {
    rw$beta
  }, rw_transform = rw_transform, rw_rescale = function(ar) {
    ar <- min(1, ar)

    rw$ar_mean <<- rw$ar_mean + 1 * (ar - rw$ar_mean)/rw$n_iter

    rw$log_step <<- rw$log_step + rw$alpha^(rw$n_iter) * (rw$ar_mean - rw$target_prob)

    return(invisible())
  }, rw_learn_cov = function() {
    n_iter <- rw$n_iter
    n_cov <- rw$n_cov

    ## Concatenate all variables in a column vector
    sample_vec_tr <- unlist(state$sample_param_tr)

    if (n_iter == n_cov + 1) {
      rw$mean <<- sample_vec_tr
      rw$cov <<- outer(sample_vec_tr, sample_vec_tr)
    } else if (n_iter > n_cov + 1) {
      ## Recursive update of the empirical mean and covariance matrix
      q <- (n_iter - n_cov - 1)/(n_iter - n_cov)
      q2 <- (n_iter - n_cov - 1)/(n_iter - n_cov)^2
      z <- sample_vec_tr - rw$mean
      rw$cov <<- q * rw$cov + q2 * outer(z, z)
      rw$mean <<- q * rw$mean + 1/(n_iter - n_cov) * sample_vec_tr
    }

    return(invisible())
  })

  class(obj_pmmh) <- "pmmh"

  return(obj_pmmh)
}

#' Manipulate PMMH objects.
#'
#' @name pmmh-object
#' @aliases pmmh biips_pmmh_update biips_pmmh_samples
#' @param object a \code{pmmh} object as returned by
#'   \code{\link{biips_pmmh_init}}.
#' @seealso \code{\link{biips_pmmh_init}}
NULL


#' @rdname pmmh-object
#' @export
#' @return The function \code{is.pmmh} returns \code{TRUE} if the object is of
#'   class \code{pmmh}.
is.pmmh <- function(object) {
  return(class(object) == "pmmh")
}

#' @keywords internal
pmmh_rw_proposal <- function(object) {
  # concatenate all variables in a vector always in the order of rw_dim
  sample_vec_tr <- c(unlist(object$sample_param_tr()))

  ## Check dimension
  rw_len <- object$rw_len()
  stopifnot(length(sample_vec_tr) == rw_len)

  n_iter <- object$n_iter()
  n_rescale <- object$n_rescale()
  beta <- object$rw_beta()
  cov <- object$rw_cov()

  if (length(cov) == 0 || (n_iter < n_rescale) || (runif(1) < beta)) {
    # Proposal with diagonal covariance
    prop_vec_tr <- sample_vec_tr + object$rw_step() * rnorm(rw_len)
  } else {
    # proposal with learnt covariance
    epsilon <- 1e-05  # For numerical stability
    cov_chol <- t(chol(cov + epsilon * diag(1, nrow = rw_len)))
    ## TODO use pivot in chol ? check positive semi-definite ?
    prop_vec_tr <- sample_vec_tr + 2.38/sqrt(rw_len) * cov_chol %*% rnorm(rw_len)
  }

  # rearrange vectorized parameter to list of arrays with appropriate dimensions
  rw_dim <- object$rw_dim()
  prop_tr <- list()
  from <- 1
  for (n in names(rw_dim)) {
    len <- prod(rw_dim[[n]])
    to <- from + len - 1
    prop_tr[[n]] <- array(prop_vec_tr[from:to], dim = rw_dim[[n]])
    from <- from + len
  }

  return(prop_tr)
}


#' Realize one step of the PMMH algorithm.
#' @keywords internal
pmmh_one_update <- function(object, pn_param, n_part, rw_rescale, rw_learn, ...) {

  console <- object$model()$ptr()
  param_names <- object$param_names()
  latent_names <- object$latent_names()
  sample_param <- object$sample_param()
  sample_param_tr <- object$sample_param_tr()
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
  prop_tr <- pmmh_rw_proposal(object)
  prop <- object$rw_transform(prop_tr, "inverse")
  lderiv <- object$rw_transform(prop, "lderiv")

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

      # warning('Data change failed:', var, '=', prop[[var]], '\n')
      break
    }

    log_p <- Rbiips("get_log_prior_density", console, pn_param$names[[i]], pn_param$lower[[i]],
      pn_param$upper[[i]])

    log_p <- log_p - sum(abs(lderiv[[var]]))

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
    ok <- smc_forward_algo(object$model(), n_part = n_part, ...)

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

    ## Acceptance rate
    accept_rate <- min(1, exp(log_marg_like_prop - log_marg_like + log_prior_prop -
      log_prior))
    if (is.nan(accept_rate))
      stop("Failed to compute acceptance rate: ", accept_rate)

    ## Accept-Reject step
    if (runif(1) < accept_rate) {
      sample_param <- prop
      sample_param_tr <- prop_tr
      log_prior <- log_prior_prop
      log_marg_like <- log_marg_like_prop

      if (n_latent > 0) {
        ## sample one particle for the latent variables
        sampled_value <- Rbiips("sample_gen_tree_smooth_particle", console,
          get_seed())

        for (var in latent_names) {
          var_in <- to_biips_vname(var)
          sample_latent[[var]] <- sampled_value[[var_in]]
        }
      }
    }
  }

  ## Update PMMH object with current state
  object$sample_param(sample_param)
  object$sample_param_tr(sample_param_tr)
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

#' @keywords internal
pmmh_algo.pmmh <- function(object, n_iter, n_part, return_samples, thin = 1, max_fail = 0,
  rw_adapt = FALSE, output = "p", ...) {
  stopifnot(is.pmmh(object))
  stopifnot(is.numeric(n_iter), length(n_iter) == 1, n_iter > 0, is.finite(n_iter))
  n_iter <- as.integer(n_iter)
  stopifnot(is.numeric(n_part), length(n_part) == 1, n_part >= 1, is.finite(n_part))
  n_part <- as.integer(n_part)
  stopifnot(is.logical(return_samples), length(return_samples) == 1)
  stopifnot(is.numeric(thin), length(thin) == 1, thin >= 1, thin <= n_iter)
  thin <- as.integer(thin)
  stopifnot(is.numeric(max_fail), length(max_fail) == 1, max_fail >= 0)
  max_fail <- as.integer(max_fail)
  stopifnot(is.logical(rw_adapt), length(rw_adapt) == 1)

  stopifnot(is.character(output))
  output <- unlist(strsplit(output, NULL))
  output <- match.arg(output, c("p", "l", "a", "f", "s"), several.ok = TRUE)
  output <- unique(output)


  ## stop biips verbosity
  verb <- Rbiips("verbosity", 0)
  # reset verbosity when function terminates
  on.exit(Rbiips("verbosity", verb))

  ## Initialization -------------------------
  console <- object$model()$ptr()

  # monitor variables
  param_names <- object$param_names()
  pn_param <- parse_varnames(param_names)
  latent_names <- object$latent_names()
  pn_latent <- parse_varnames(latent_names)
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
  pmmh_set_param(object$model(), param_names, pn_param, sample_param)

  # Initialize counters
  n_samples <- floor(n_iter/thin)
  ind_sample <- 0
  n_fail <- 0

  # Output structure with MCMC samples
  out <- list()
  if ("p" %in% output)
    out$log_marg_like_pen <- mcmcarray(dim = c(1, n_samples), name = "log_marg_like_pen")
  if ("l" %in% output)
    out$log_marg_like <- mcmcarray(dim = c(1, n_samples), name = "log_marg_like")
  if ("a" %in% output)
    out$info$accept_rate <- mcmcarray(dim = c(1, n_samples), name = "accept_rate")
  if ("s" %in% output) {
    len <- length(object$rw_step())
    out$info$rw_step <- mcmcarray(dim = c(len, n_samples), name = "rw_step")
  }

  if (return_samples) {
    for (ivar in 1:length(param_names)) {
      var <- param_names[[ivar]]
      dimen <- dim(sample_param[[var]])
      out[[var]] <- mcmcarray(dim = c(dimen, n_samples),
                              name = pn_param$names[[ivar]],
                              lower = pn_param$lower[[ivar]],
                              upper = pn_param$upper[[ivar]])
    }
  }

  ## display message and progress bar
  mess <- if (return_samples)
    paste("Generating", n_samples, "PMMH samples with") else {
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
    if (rw_rescale && (object$n_iter() >= object$n_rescale())) {
      rw_rescale <- FALSE
    }

    ## Store output
    if ((i%%thin) == 0) {
      ind_sample <- ind_sample + 1
      log_marg_like <- object$log_marg_like()
      log_prior <- object$log_prior()

      if ("p" %in% output)
        out$log_marg_like_pen[ind_sample] <- log_marg_like + log_prior
      if ("l" %in% output)
        out$log_marg_like[ind_sample] <- log_marg_like
      if ("a" %in% output)
        out$info$accept_rate[ind_sample] <- out_step$accept_rate
      if ("s" %in% output)
        out$info$rw_step[, ind_sample] <- object$rw_step()

      if (return_samples) {
        sample_param <- object$sample_param()
        sample_latent <- object$sample_latent()

        for (var in param_names) {
          len <- length(sample_param[[var]])
          from <- (ind_sample - 1) * len + 1
          to <- (ind_sample - 1) * len + len
          out[[var]][from:to] <- sample_param[[var]]
        }

        if (ind_sample == 1) {
          ## pre-allocation here to be sure that sample is not empty
          for (ivar in 1:length(latent_names)) {
            var <- latent_names[[ivar]]
            dimen <- dim(sample_latent[[var]])
            out[[var]] <- mcmcarray(dim = c(dimen, n_samples),
                                    name = pn_latent$names[[ivar]],
                                    lower = pn_latent$lower[[ivar]],
                                    upper = pn_latent$upper[[ivar]])
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
    out$info$n_fail <- n_fail

  class(out) <- "mcmcarray.list"

  return(out)
}



#' @export
biips_pmmh_update <- function(object, ...) UseMethod("biips_pmmh_update")

#' The method \code{biips_pmmh_update} performs adaptation and burn-in
#' iterations for the PMMH algorithm.
#'
#' @export
#' @rdname pmmh-object
#' @param n_iter   integer. Number of burn-in iterations.
#' @param thin     integer. Thinning interval. Returns samples every \code{thin}
#'   iterations (default = 1)
#' @param n_part   integer. Number of particles used in SMC algorithms.
#' @param ... Additional arguments to be passed to the SMC
#'   algorithm such as \code{rs_thres} and \code{rs_type}.
#'   See \code{\link{biips_smc_samples}}  for more details.
#' @param output string. Select additional members to be returned in the
#'   \code{\link{mcmcarray.list}} output. The string can contain several
#'   characters in \code{('p', 'l', 'a', 's', 'f')}. See details. (default = \code{'p'})
#'
#' @details The \code{output} string arguments can be used to query additional
#'   members in the \code{\link{mcmcarray.list}} output. If \code{output}
#'   contains:
#'   \describe{
#'     \item{p}{returns member \code{log_marg_like_pen}. \code{\link{mcmcarray}} with penalized
#'     log marginal likelihood estimates over iterations.}
#'     \item{l}{returns member \code{log_marg_like}. \code{\link{mcmcarray}}
#'     with log marginal likelihood estimates over iterations.}
#'     \item{a}{returns member \code{info$accept_rate}. \code{\link{mcmcarray}} with acceptance
#'     rate over iterations.}
#'     \item{s}{returns member \code{info$rw_step}. \code{\link{mcmcarray}}
#'     with standard deviations of the random walk over iterations.}
#'     \item{f}{returns member \code{info$n_fail}. number of failed SMC algorithms.}
#'   }
#'
#' @param max_fail integer. maximum number of failed SMC algorithms allowed.
#'   (default=0).
#' @param rw_adapt      logical. Activate adaptation of the proposal
#'   (default=\code{TRUE}).
#'
#' @return The methods \code{biips_pmmh_update} and \code{biips_pmmh_update}
#'   return an object of class \code{\link{mcmcarray.list}}.
#'
#' @return \code{biips_pmmh_samples} output contains one \code{\link{mcmcarray}}
#'   member for each monitored variable returned by the \code{param_names()} and
#'   \code{latent_names()} member functions of the \code{pmmh} object.
#'
#' @return The members of the \code{\link{mcmcarray.list}} object are
#'   \code{\link{mcmcarray}} objects for different variables. Assuming
#'   \code{dim} is the dimension of the monitored variable, the
#'   \code{\link{mcmcarray}} object is an array of dimension \code{c(dim,
#'   n_iter)} with the following attributes (accessible with
#'   \code{\link[base]{attr}}):
#'   \item{name}{string with the name of the variable.}
#'   \item{lower}{vector with the lower bounds of the variable.}
#'   \item{upper}{vector with the upper bounds of the variable.}
#'
#' @return If the \code{output} argument is not empty, the output contains
#'   additional members. See details.
#'
#' @examples
#' modelfile <- system.file('extdata', 'hmm.bug', package = 'Rbiips')
#' stopifnot(nchar(modelfile) > 0)
#' cat(readLines(modelfile), sep = '\n')
#'
#' data <- list(tmax = 10, p = c(.5, .5), logtau_true = log(1))
#' model <- biips_model(modelfile, data)
#'
#' n_part <- 50
#' obj_pmmh <- biips_pmmh_init(model, 'logtau', latent_names = c('x', 'c[2:10]'),
#'                             inits = list(logtau = -2))  # Initialize
#' is.pmmh(obj_pmmh)
#' out_pmmh_burn <- biips_pmmh_update(obj_pmmh, 100, n_part)  # Burn-in
#' out_pmmh <- biips_pmmh_samples(obj_pmmh, 100, n_part, thin = 1)  # Samples
#'
#' dens_pmmh_lt <- biips_density(out_pmmh$logtau)
#' summ_pmmh_x <- biips_summary(out_pmmh$x, order = 2, probs = c(0.025, 0.975))
#' dens_pmmh_x <- biips_density(out_pmmh$x)
#' summ_pmmh_c <- biips_summary(out_pmmh[['c[2:10]']])
#' table_pmmh_c <- biips_table(out_pmmh[['c[2:10]']])
#'
#' par(mfrow = c(2, 2))
#' plot(c(out_pmmh_burn$log_marg_like_pen, out_pmmh$log_marg_like_pen), type = 'l',
#'      col = 'blue', xlab = 'PMMH iteration', ylab = 'log p(y|logtau) + log p(logtau)')
#'
#' plot(out_pmmh$logtau[1, ], type = 'l', col = 'blue',
#'      xlab = 'PMMH iteration', ylab = 'logtau')
#' points(0, model$data()$logtau_true, pch = 17, col = 'green')
#'
#' plot(dens_pmmh_lt, col = 'blue', ylab = 'posterior density')
#' points(model$data()$logtau_true, 0, pch = 17, col = 'green')
#'
#' biips_hist(out_pmmh$logtau, col = 'blue', ylab = 'posterior density')
#' points(model$data()$logtau_true, 0, pch = 17, col = 'green')
#'
#' par(mfrow = c(2, 2))
#' plot(model$data()$x_true, type = 'l', col = 'green', xlab = 't', ylab = 'x[t]')
#' lines(summ_pmmh_x$mean, col = 'blue')
#' matlines(matrix(unlist(summ_pmmh_x$quant), data$tmax), lty = 2, col = 'blue')
#' legend('topright', leg = c('true', 'PMMH estimate'), lty = c(2, 1),
#'        col = c('green', 'blue'), bty = 'n')
#'
#' barplot(.5*(model$data()$c_true==1), col = 'green', border = NA, space = 0, offset = 1,
#'         ylim=c(0,2), xlab='t', ylab='c[t]==1', axes = FALSE)
#' axis(1, at=1:data$tmax-.5, labels=1:data$tmax)
#' axis(2, line = 1, at=c(0,2), labels=NA)
#' text(data$tmax/2, 1.75, 'true')
#' barplot(.5*c(NA, summ_pmmh_c$mode==1), col = 'blue', border = NA, space = 0,
#'         axes = FALSE, add = TRUE)
#' text(data$tmax/2, .75, 'PMMH mode')
#'
#' t <- 5
#' plot(dens_pmmh_x[[t]], col='blue', ylab = 'posterior density')
#' points(model$data()$x_true[t], 0, pch = 17, col = 'green')
#'
#' plot(table_pmmh_c[[t-1]], col='blue', ylab = 'posterior probability mass')
#' points(model$data()$c_true[t], 0, pch = 17, col = 'green')
biips_pmmh_update.pmmh <- function(object, n_iter, n_part, thin = 1, max_fail = 0,
  rw_adapt = TRUE, output = "p", ...) {
  out <- pmmh_algo(object, n_iter, n_part, thin = thin, return_samples = FALSE,
    max_fail = max_fail, rw_adapt = rw_adapt, output = output, ...)
  return(invisible(out))
}

#' @export
biips_pmmh_samples <- function(object, ...) UseMethod("biips_pmmh_samples")

#' The method \code{biips_pmmh_samples} performs iterations for the PMMH
#' algorithm and returns samples.
#'
#' @export
#' @rdname pmmh-object
biips_pmmh_samples.pmmh <- function(object, n_iter, n_part, thin = 1, max_fail = 0,
  output = "p", ...) {

  out <- pmmh_algo(object, n_iter, n_part, thin = thin, return_samples = TRUE,
    max_fail = max_fail, output = output, ...)
  return(out)
}
