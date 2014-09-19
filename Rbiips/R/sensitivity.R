##' Sensitivity analysis
##'
##' %% ~~ A concise (1-5 lines) description of what the function does. ~~
##' %% ~~ If necessary, more details than the description above ~~
##'
##' @param model a biips model object
##' @param param_values
##' @param n_part number of particles
##' @param ... additional arguments to be passed to the SMC algorithm
##' @return %% ~Describe the value returned
##' @author Adrien Todeschini, Francois Caron
##' @keywords models
##' @export
##' @examples
##'
##' ## Should be DIRECTLY executable !!
##' ##-- ==>  Define data, use random,
##' ##--\tor do  help(data=index)  for the standard data sets.
##'
smc_sensitivity <- function(object, param_values, n_part, ...) {
  stopifnot(is.biips(object))

  n_param <- length(param_values)

  stopifnot(is.list(param_values), n_param>0)
  stopifnot(sapply(param_values, is.numeric))
  varnames <- names(param_values)
  stopifnot(!is.null(varnames), nchar(varnames)>0)
  pn <- parse_varnames(varnames)

  ## Get the sizes of parameter values
  sizes <- lapply(param_values, function(x) {
    d <- dim(x)
    if (is.null(d))
      length(x)
    else
      d
  })

  ## Get the number of parameter values to evaluate (the last dimension)
  n_val <- lapply(sizes, function(x) x[length(x)])
  n_values <- n_val[[1]]

  ## Check if number of parameter values match
  if (any(n_val != n_values))
    stop('Invalid param_values argument: number of values to evaluate does not match')

  ## Get the dimensions of parameters
  dimen <- lapply(sizes, function(x) {
    if (length(x)==1)
      1
    else
      x[-length(x)]
  })
  len <- lapply(dimen, prod)

  ## stop biips verbosity
  verb <- Rbiips("verbosity", 0)
  on.exit(Rbiips("verbosity", verb)) # set verbosity on again when function terminates

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
        stop("data change failed: invalid parameter. ", var, " = ",
          paste0(value[i], collapse=" "))
    }

    ## get log prior
    log_prior <- 0
    for (i in 1:n_param) {
      log_p <- .Call("get_log_prior_density", model$ptr(), pn$names[[i]],
        pn$lower[[i]], pn$upper[[i]], PACKAGE = "Rbiips")

      ## FIXME : why ?
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

#   ## FIXME: Tentative of removing data without recompiling the model
#   for (v in seq(along = varnames)) {
#
#     ## if the variable is not present in the data then it was not observed hence
#     ## remove the data and go to the next variable
#     if (!(pn$names[[v]] %in% names(data))) {
#       ok <- Rbiips("remove_data", object$ptr(), pn$names[[v]], pn$lower[[v]],
#         pn$upper[[v]])
#       if (!ok)
#         stop("Failure restoring data")
#       next
#     }
#
#     ## otherwise: the variable might be observed then retrieve the original data
#
#     if (is.null(pn$lower[[v]])) {
#       ## if there is no bounds, then get the full array
#       data_sub <- data[[v]]
#     } else {
#       ## otherwise get the corresponding components of the data array
#
#       ## compute offsets
#       offsets <- NULL
#       dimen <- dim(data[[v]])
#       if (is.null(dimen))
#         dimen <- length(data[[v]])
#       ind <- vector(len = length(dimen))
#
#       for (i in seq(along = data[[v]])) {
#         r <- i
#         for (d in seq(along = dimen)) {
#           ind[d] <- ((r - 1)%%dimen[d]) + 1
#           r <- ceiling(r/dimen[d])
#         }
#         print(ind)
#         if (any(ind < pn$lower[[v]]) || any(ind > pn$upper[[v]]))
#           next
#         offsets <- c(offsets, i)
#       }
#
#       data_sub <- array(data[[v]][offsets], dim = pn$upper[[v]] - pn$lower[[v]] +
#         1)
#     }
#
#     ## if all the components are NA then the variable was not observed hence remove
#     ## the data and go to the next variable
#     if (all(is.na(data_sub))) {
#       ok <- Rbiips("remove_data", object$ptr(), pn$names[[v]], pn$lower[[v]],
#         pn$upper[[v]])
#       if (!ok)
#         stop("Failure restoring data")
#       next
#     }
#
#     ## check that no component is NA either all are NA or no one is
#     if (any(is.na(data_sub)))
#       stop("Failure restoring data")
#
#     ## finally restore the data
#     ok <- .Call("change_data", object$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]],
#       data_sub, FALSE, PACKAGE = "Rbiips")
#     if (!ok)
#       stop("Failure restoring data")
#   }

  ## output --------
  out <- list(log_marg_like = log_marg_like, max_param = max_param, max_log_marg_like = max_log_marg_like,
    log_marg_like_pen = log_marg_like_pen, max_param_pen = max_param_pen, max_log_marg_like_pen = max_log_marg_like_pen)

  return(out)
}
