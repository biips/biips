##' Sensitivity analysis
##'
##' %% ~~ A concise (1-5 lines) description of what the function does. ~~
##' %% ~~ If necessary, more details than the description above ~~
##'
##' @param model a biips model object
##' @param params
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
smc_sensitivity <- function(object, params, n_part, ...) {
  if (!is.biips(object))
    stop("Invalid Biips model")

  if (missing(n_part))
    stop("missing n_part argument")

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
    d <- d[1:(length(d) - 1)]
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
        d <- d[1:(length(d) - 1)]
        dim[[names(params)[i]]] <- d
        len[[names(params)[i]]] <- prod(d)
      }
      # check parameters array length
      if (n != n.params)
        stop("Invalid params argument: different number of parameters.")
    }
  }

  ## stop biips verbosity
  verb <- Rbiips("verbosity", 0)
  on.exit(Rbiips("verbosity", verb))

  ## initialize -----------
  log_marg_like <- vector(length = n.params)
  log_marg_like_pen <- vector(length = n.params)
  max_log_marg_like <- -Inf
  max_log_marg_like_pen <- -Inf

  Rbiips("message", paste("Analyzing sensitivity with", n_part, "particles"))
  ## progress bar
  bar <- Rbiips("progress_bar", n.params, "*", "iterations")

  ## Iterate --------
  for (k in 1:n.params) {

    ## make one param
    param <- list()
    for (v in seq(along = variable.names)) {
      var <- variable.names[[v]]
      ind <- seq(from = (k - 1) * len[[v]] + 1, to = k * len[[v]])
      param[[var]] <- params[[v]][ind]
      dim(param[[var]]) <- dim[[v]]

      ## change param value
      if (!.Call("change_data", object$ptr(), pn$names[[v]], pn$lower[[v]],
        pn$upper[[v]], param[[v]], FALSE, PACKAGE = "Rbiips"))
        stop("data change failed: invalid parameter. ", var, " = ",
          paste0(param[v], collapse=" "))
    }

    log_prior <- 0
    for (v in seq(along = variable.names)) {
      log_p <- .Call("get_log_prior_density", object$ptr(), pn$names[[v]],
        pn$lower[[v]], pn$upper[[v]], PACKAGE = "Rbiips")

      if (is.na(log_p)) {
        next
      }

      if (is.nan(log_p) || (is.infinite(log_p) && log_p < 0))
        stop("Failure evaluating parameter log prior for variable ", variable.names[[v]],
          ". value = ", param[v])

      log_prior <- log_prior + log_p
    }

    ## run smc sampler
    ok <- run_smc_forward(object, n_part = n_part, ...)

    if (!ok)
      stop("Failure running smc forward sampler. param: ", paste(names(param),
        "=", param, sep = "", collapse = ";"))

    ## log marginal likelihood
    log_marg_like[k] <- Rbiips("get_log_norm_const", object$ptr())
    if (log_marg_like[k] > max_log_marg_like) {
      max_log_marg_like <- log_marg_like[k]
      max.param <- param
    }
    log_marg_like_pen[k] <- log_marg_like[k] + log_prior
    if (log_marg_like_pen[k] > max_log_marg_like_pen) {
      max_log_marg_like_pen <- log_marg_like_pen[k]
      max.param.pen <- param
    }

    ## advance progress bar
    Rbiips("advance_progress_bar", bar, 1)
  }

  ## restore data ------------- object$recompile()

  ## FIXME: Tentative of removing data without recompiling the model
  for (v in seq(along = variable.names)) {

    ## if the variable is not present in the data then it was not observed hence
    ## remove the data and go to the next variable
    if (!(pn$names[[v]] %in% names(data))) {
      ok <- Rbiips("remove_data", object$ptr(), pn$names[[v]], pn$lower[[v]],
        pn$upper[[v]])
      if (!ok)
        stop("Failure restoring data")
      next
    }

    ## otherwise: the variable might be observed then retrieve the original data

    if (is.null(pn$lower[[v]])) {
      ## if there is no bounds, then get the full array
      data.sub <- data[[v]]
    } else {
      ## otherwise get the corresponding components of the data array

      ## compute offsets
      offsets <- NULL
      dim <- dim(data[[v]])
      if (is.null(dim))
        dim <- length(data[[v]])
      ind <- vector(len = length(dim))

      for (i in seq(along = data[[v]])) {
        r <- i
        for (d in seq(along = dim)) {
          ind[d] <- ((r - 1)%%dim[d]) + 1
          r <- ceiling(r/dim[d])
        }
        print(ind)
        if (any(ind < pn$lower[[v]]) || any(ind > pn$upper[[v]]))
          next
        offsets <- c(offsets, i)
      }

      data.sub <- array(data[[v]][offsets], dim = pn$upper[[v]] - pn$lower[[v]] +
        1)
    }

    ## if all the components are NA then the variable was not observed hence remove
    ## the data and go to the next variable
    if (all(is.na(data.sub))) {
      ok <- Rbiips("remove_data", object$ptr(), pn$names[[v]], pn$lower[[v]],
        pn$upper[[v]])
      if (!ok)
        stop("Failure restoring data")
      next
    }

    ## check that no component is NA either all are NA or no one is
    if (any(is.na(data.sub)))
      stop("Failure restoring data")

    ## finally restore the data
    ok <- .Call("change_data", object$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]],
      data.sub, FALSE, PACKAGE = "Rbiips")
    if (!ok)
      stop("Failure restoring data")
  }

  ## output --------
  ans <- list(log_marg_like = log_marg_like, max.param = max.param, max_log_marg_like = max_log_marg_like,
    log_marg_like_pen = log_marg_like_pen, max.param.pen = max.param.pen, max_log_marg_like_pen = max_log_marg_like_pen)

  return(ans)
}
