#' @title Sensitivity analysis of parameters with SMC.
#'
#' @export
#' @param object \code{biips} model object as returned by
#'   \code{\link{biips_model}}.
#' @param param_values  named list of numeric values. Each member is named after
#'   a parameter for which we want to study sensitivity. Each member value is a
#'   vector or array whose last dimension is equal to the number of values for
#'   which we want to study sensitivity.
#' @param n_part        integer. Number of particles.
#' @param ... Additional arguments to be passed to the SMC
#'   algorithm such as \code{rs_thres} and \code{rs_type}.
#'   See \code{\link{biips_smc_samples}}  for more details.
#'
#' @return \code{biips_smc_sensitivity} returns a list with the following
#'   members: \itemize{
#'     \item \code{log_marg_like}: vector of log marginal likelihood estimates
#'     at the different values of the parameters.
#'     \item \code{log_marg_like_pen}: vector of penalized log marginal likelihood
#'       estimates at the different values of the parameters.
#'     \item \code{max_param}: parameters value with maximum \code{log_marg_like}.
#'     \item \code{max_log_marg_like}: maximum \code{log_marg_like} value.
#'     \item \code{max_param_pen}: parameters value with maximum \code{log_marg_like_pen}.
#'     \item \code{max_log_marg_like_pen}: maximum \code{log_marg_like_pen} value.
#'   }
#'
#' @seealso \code{\link{biips_model}}, \code{\link{biips_smc_samples}}
#' @examples
#' modelfile <- system.file('extdata', 'hmm.bug', package = 'Rbiips')
#' stopifnot(nchar(modelfile) > 0)
#' cat(readLines(modelfile), sep = '\n')
#'
#' data <- list(tmax = 10, p = c(.5, .5), logtau_true = log(1), logtau = log(1))
#' model <- biips_model(modelfile, data)
#'
#' n_part <- 50
#' logtau_val <- -10:10
#' out_sens <- biips_smc_sensitivity(model, list(logtau = logtau_val), n_part)
#'
#' par(mfrow = c(2, 1))
#' plot(logtau_val, out_sens$log_marg_like, type = 'l', col = 'blue',
#'      xlab = 'logtau', ylab = 'log p(y|logtau) ', main = 'SMC sensitivity')
#' points(data$logtau, min(out_sens$log_marg_like), pch = 17, col = 'green')
#'
#' plot(logtau_val, out_sens$log_marg_like_pen, type = 'l', col = 'blue',
#'      xlab = 'logtau', ylab = 'log p(y|logtau) + log p(logtau)')
#' plml <- out_sens$log_marg_like_pen
#' ymin <- min(plml[is.finite(plml)])
#' points(data$logtau, ymin, pch = 17, col = 'green')
biips_smc_sensitivity <- function(object, param_values, n_part, ...) {
  stopifnot(is.biips(object))

  n_param <- length(param_values)

  stopifnot(is.list(param_values), n_param > 0)
  stopifnot(sapply(param_values, is.numeric))
  varnames <- names(param_values)
  stopifnot(!is.null(varnames), nchar(varnames) > 0)
  pn <- parse_varnames(varnames)

  ## Get the sizes of parameter values
  sizes <- lapply(param_values, function(x) dim(as.array(x)))

  ## Get the number of parameter values to evaluate (the last dimension)
  n_val <- lapply(sizes, function(x) x[length(x)])
  n_values <- n_val[[1]]

  ## Check if number of parameter values match
  if (any(n_val != n_values))
    stop("Invalid param_values argument: number of values to evaluate does not match")

  ## Get the dimensions of parameters
  dimen <- lapply(sizes, function(x) {
    if (length(x) == 1)
      1 else x[-length(x)]
  })
  len <- lapply(dimen, prod)

  ## stop biips verbosity
  verb <- Rbiips("verbosity", 0)
  on.exit(Rbiips("verbosity", verb))  # set verbosity on again when function terminates

  ## Clone console
  model <- clone_model(object)

  ## initialize -----------
  log_marg_like <- rep(NA, times = n_values)
  log_marg_like_pen <- rep(NA, times = n_values)
  max_log_marg_like <- -Inf
  max_log_marg_like_pen <- -Inf
  max_param <- NULL
  max_param_pen <- NULL

  Rbiips("message", paste("Analyzing sensitivity with", n_part, "particles"))
  ## progress bar
  bar <- Rbiips("progress_bar", n_values, "*", "iterations")

  ## Iterate over different values --------
  for (k in 1:n_values) {

    ## Update value of each parameter
    value <- list()
    for (i in 1:n_param) {
      var <- varnames[[i]]
      ind <- seq(from = (k - 1) * len[[i]] + 1, to = k * len[[i]])
      value[[var]] <- param_values[[i]][ind]
      dim(value[[var]]) <- dimen[[i]]

      ## change param value
      if (!.Call("change_data", model$ptr(), pn$names[[i]], pn$lower[[i]],
        pn$upper[[i]], value[[i]], FALSE, PACKAGE = "Rbiips"))
        stop("data change failed: invalid parameter. ", var, " = ", paste0(value[i],
          collapse = " "))
    }

    ## get log prior
    log_prior <- 0
    for (i in 1:n_param) {
      log_p <- .Call("get_log_prior_density", model$ptr(), pn$names[[i]], pn$lower[[i]],
        pn$upper[[i]], PACKAGE = "Rbiips")

      ## FIXME: why?
      if (is.na(log_p))
        next
      if (is.nan(log_p))
        stop("Failure evaluating parameter log prior for variable ", varnames[[i]],
          " = ", value[i])

      log_prior <- log_prior + log_p
    }

    ## run smc sampler
    ok <- smc_forward_algo(model, n_part = n_part, ...)

    if (!ok)
      stop("Failure running SMC forward sampler. parameter: ", paste(names(value),
        "=", value, sep = "", collapse = ";"))

    ## log marginal likelihood
    log_marg_like[k] <- Rbiips("get_log_norm_const", model$ptr())
    if (log_marg_like[k] > max_log_marg_like) {
      max_log_marg_like <- log_marg_like[k]
      max_param <- value
    }
    log_marg_like_pen[k] <- log_marg_like[k] + log_prior
    if (log_marg_like_pen[k] > max_log_marg_like_pen) {
      max_log_marg_like_pen <- log_marg_like_pen[k]
      max_param_pen <- value
    }

    ## advance progress bar
    Rbiips("advance_progress_bar", bar, 1)
  }

  ## output --------
  out <- list(log_marg_like = log_marg_like, max_param = max_param, max_log_marg_like = max_log_marg_like,
    log_marg_like_pen = log_marg_like_pen, max_param_pen = max_param_pen, max_log_marg_like_pen = max_log_marg_like_pen)

  return(out)
}
