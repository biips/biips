#' Manipulate \code{biips} model objects.
#'
#' A \code{biips} object is returned by function \code{\link{biips_model}}. It
#' represents a Bayesian graphical model described in BUGS language.
#'
#' @name biips-object
#' @aliases biips biips_variable_names biips_nodes biips_print_dot
#'   biips_build_sampler
#' @param ... Additional arguments to be passed to default methods.
#' @param object,x \code{biips} model object as returned by
#'   \code{\link{biips_model}}.
#' @seealso \code{\link{biips_model}}
#' @examples
#' modelfile <- system.file('extdata', 'hmm.bug', package = 'Rbiips')
#' cat(readLines(modelfile), sep='\n')
#'
#' data <- list(tmax = 10, logtau = log(10))
#' model <- biips_model(modelfile, data, sample_data = TRUE)
#'
#' is.biips(model)
#' print(model)
#'
#' model$data()
#' model$model()
#'
#' variable.names(model)
#' biips_variable_names(model)
#'
#' biips_nodes(model)
#'
#' dotfile = 'hmm.dot'
#' biips_print_dot(model, dotfile)
#' cat(readLines(dotfile), sep='\n')
#'
#' biips_build_sampler(model, proposal = 'prior')
#' biips_nodes(model, type = 'stoch', observed = FALSE)
#'
#' biips_build_sampler(model, proposal = 'auto')
#' biips_nodes(model, type = 'stoch', observed = FALSE)
#'
#' rm(model)
NULL


#' @export
#' @rdname biips-object
#' @return The function \code{is.biips} returns \code{TRUE} if the object is of
#'   class \code{biips}.
is.biips <- function(object) {
  return(class(object) == "biips")
}

#' @export
#' @rdname biips-object
print.biips <- function(x, ...) {
  stopifnot(is.biips(x))

  cat("Biips model:\n\n")

  model <- x$model()
  for (i in 1:length(model)) {
    cat(model[i], "\n", sep = "")
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
  return(invisible())
}

#' @export
biips_variable_names <- function(object, ...) UseMethod("biips_variable_names")

#' @export
#' @rdname biips-object
#' @return The method \code{biips_variable_names} returns a character vector.
#'   Names of node arrays used in the model.
biips_variable_names.biips <- function(object, ...) {
  stopifnot(is.biips(object))
  variable_names <- Rbiips("get_variable_names", object$ptr())
  return(variable_names)
}

#' @importFrom stats variable.names
#' @export
#' @rdname biips-object
#' @return The method \code{variable.names} is an alias for
#'   \code{biips_variable_names}.
#' @method variable.names biips
variable.names.biips <- function(object, ...) {
  return(biips_variable_names(object, ...))
}

#' @export
biips_nodes <- function(object, ...) UseMethod("biips_nodes")

#' @export
#' @rdname biips-object
#' @param type   string. Return only a specific type of node. Possible values
#'   are \code{'const'}, \code{'logic'} or \code{'stoch'}. default returns all
#'   types of nodes.
#' @param observed  logical. Return only observed or unobserved nodes. default
#'   returns all.
#'
#' @return The method \code{biips_nodes} returns a \code{data.frame} with a row
#'   for each node of the graphical model sorted in a topological order with the
#'   following columns: \itemize{ \item{\code{id}: integer. node ids.}
#'   \item{\code{name}: string. node names.} \item{\code{type}: string. node
#'   types (\code{'const'}, \code{'logic'} or \code{'stoch'}).}
#'   \item{\code{observed}: logical. \code{TRUE} if the node is observed.}
#'
#'   If the function is called after the sampler is built (by calling
#'   \code{\link{biips_build_sampler}} or \code{\link{biips_smc_samples}}), it
#'   will also contain the additional variables: \item{\code{iteration}:
#'   integer. node sampling iteration of unobserved nodes, starting at 0.
#'   \code{NA} if the node if observed} \item{\code{sampler}: string. node
#'   sampler name for stochastic unobserved nodes. An empty string for other
#'   types of nodes.} }
biips_nodes.biips <- function(object, type, observed, ...) {
  stopifnot(is.biips(object))

  sorted_nodes <- data.frame(Rbiips("get_sorted_nodes", object$ptr()))

  # add samplers and iterations if sampler is built
  if (Rbiips("is_sampler_built", object$ptr())) {
    samplers <- data.frame(Rbiips("get_node_samplers", object$ptr()))
    sorted_nodes <- cbind(sorted_nodes, samplers)
  }

  # filter by type
  if (!missing(type)) {
    stopifnot(is.character(type), length(type) == 1)

    type <- match.arg(type, c("const", "logic", "stoch"))
    sorted_nodes <- sorted_nodes[sorted_nodes["type"] == type, ]
  }

  # filter by observed
  if (!missing(observed)) {
    stopifnot(is.logical(observed), length(observed) == 1)
    sorted_nodes <- sorted_nodes[sorted_nodes["observed"] == observed, ]
  }

  return(sorted_nodes)
}

#' @export
biips_print_dot <- function(object, ...) UseMethod("biips_print_dot")

#' @export
#' @rdname biips-object
#' @param file  string. Path of the output file.
#' @return The method \code{biips_print_dot} prints the graph in a file in dot
#'   format.
biips_print_dot.biips <- function(object, file, ...) {
  stopifnot(is.biips(object))

  Rbiips("print_graphviz", object$ptr(), file)
  return(invisible())
}

#' @export
biips_build_sampler <- function(object, ...) UseMethod("biips_build_sampler")

#' The method \code{biips_build_sampler} assigns a sampler to each node
#' of the graph. In order to specify the proposal used by the SMC
#' algorithm, this function has to be called before \code{\link{biips_smc_samples}}.
#' Otherwise, it will be automatically called by \code{\link{biips_smc_samples}}
#' with the default parameters.
#'
#' @rdname biips-object
#' @export
#' @param proposal string. The type of proposal used by the SMC algorithm.
#'   Possible values are \code{'auto'} and \code{'prior'}. \code{'auto'} selects the
#'   best sampler among available ones automatically. \code{'prior'} forces
#'   asignment of the prior sampler to every node. \code{'prior'} switches off
#'   lots of instructions and can speed up the startup of the SMC for large
#'   models. (default = \code{'auto'}).
#'
#' @return The method \code{biips_build_sampler} returns \code{NULL}.
biips_build_sampler.biips <- function(object, proposal = "auto", ...) {
  stopifnot(is.biips(object))
  proposal <- match.arg(proposal, c("auto", "prior"))

  ## build smc sampler
  Rbiips("build_smc_sampler", object$ptr(), proposal == "prior")

  return(invisible())
}

monitor <- function(object, ...) UseMethod("monitor")

#' @keywords internal
monitor.biips <- function(object, variable_names, type = "s", ...) {
  stopifnot(is.biips(object))
  stopifnot(is.character(variable_names), length(variable_names) > 0)
  type <- check_type(type, several.ok = TRUE)

  pn <- parse_varnames(variable_names)

  for (t in type) {
    switch(t, f = Rbiips("set_filter_monitors", object$ptr(), pn$names, pn$lower,
      pn$upper), s = Rbiips("set_gen_tree_smooth_monitors", object$ptr(), pn$names,
      pn$lower, pn$upper), b = Rbiips("set_backward_smooth_monitors", object$ptr(),
      pn$names, pn$lower, pn$upper))
  }

  return(invisible())
}

is_monitored <- function(object, ...) UseMethod("is_monitored")

#' @keywords internal
is_monitored.biips <- function(object, variable_names, type = "s", check_released = TRUE) {
  stopifnot(is.biips(object))
  stopifnot(is.character(variable_names), length(variable_names) > 0)
  type <- check_type(type, several.ok = FALSE)
  stopifnot(is.logical(check_released), length(check_released) == 1)

  pn <- parse_varnames(variable_names)

  ok <- switch(type, f = Rbiips("is_filter_monitored", object$ptr(), pn$names,
    pn$lower, pn$upper, check_released), s = Rbiips("is_gen_tree_smooth_monitored",
    object$ptr(), pn$names, pn$lower, pn$upper, check_released), b = Rbiips("is_backward_smooth_monitored",
    object$ptr(), pn$names, pn$lower, pn$upper, check_released))

  return(ok)
}


clear_monitors <- function(object, ...) UseMethod("clear_monitors")

#' Clear monitors
#' @param object \code{biips} model object
#' @param type string with characters \code{'f'} (filtering), \code{'s'}
#'   (smoothing) and/or \code{'b'} (backward smoothing).
#' @param release_only logical. If \code{TRUE}, only releases memory occupied by
#'   monitors. Information about monitored nodes is still present. If
#'   \code{FALSE} clears all information about monitored nodes as well as
#'   memory.
#' @keywords internal
clear_monitors.biips <- function(object, type = "fsb", release_only = FALSE, ...) {
  stopifnot(is.biips(object))
  type <- check_type(type, several.ok = TRUE)
  stopifnot(is.logical(release_only), length(release_only) == 1)

  for (t in type) {
    switch(t, f = Rbiips("clear_filter_monitors", object$ptr(), release_only),
      s = Rbiips("clear_gen_tree_smooth_monitors", object$ptr(), release_only),
      b = Rbiips("clear_backward_smooth_monitors", object$ptr(), release_only))
  }

  return(invisible())
}

clone_model <- function(object, ...) UseMethod("clone_model")

#' @keywords internal
clone_model.biips <- function(object, ...) {
  stopifnot(is.biips(object))

  mf <- tempfile()
  writeLines(object$model(), mf)
  model <- biips_model(mf, data = object$data(), sample_data = FALSE, quiet = TRUE)
  unlink(mf)
  return(model)
}
