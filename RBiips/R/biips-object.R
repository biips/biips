
##' Manipulate biips model objects
##' 
##' A \code{biips} object represents a Bayesian graphical model described using
##' the BUGS language.
##' 
##' The \code{variable.names} function returns a character vector of names of
##' node arrays used in the model.
##' 
##' @name biips-object 
##' @aliases biips-object build_sampler build_sampler.biips pmmh_init
##' pmmh_init.biips pmmh_update pmmh_update.biips pimh_update pimh_update.biips
##' variable.names.biips is.biips
##' @param object a biips model object
##' @param proposal
##' @param variable_names a character vector giving the names of variables to
##' be monitored
##' @param inits
##' @param n_part number of particles
##' @param rs_thres threshold on the ESS criterion to control the resampling
##' step
##' @param rs_type a string indicating the resampling algorithm used
##' @param n_iter number of iterations of the Markov chain to run
##' @param max_fail maximum number of failures allowed
##' @param ... additional arguments
##' @return %% ~Describe the value returned
##' @author Adrien Todeschini, Francois Caron
##' @keywords models
##' @examples
##' 
##' ## Should be DIRECTLY executable !! 
##' ##-- ==>  Define data, use random,
##' ##--  or do  help(data=index)  for the standard data sets.
##' 
NULL



##' @export
is.biips <- function(object) {
  return(class(object) == "biips")
}

##' @S3method print biips
print.biips <- function(x, ...) {
  stopifnot(is.biips(x))
  
  cat("BiiPS model:\n\n")
  
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



##' @importFrom stats variable.names
##' @S3method variable.names biips
variable.names.biips <- function(object, ...) {
  stopifnot(is.biips(object))
  
  variable_names <- RBiips("get_variable_names", object$ptr())
  
  return(variable_names)
}



##' @export
nodes <- function(object, ...) UseMethod("nodes")

##' @S3method nodes biips
nodes.biips <- function(object, type, observed, ...) {
  stopifnot(is.biips(object))
  
  sorted_nodes <- data.frame(RBiips("get_sorted_nodes", object$ptr()))
  
  if (RBiips("is_sampler_built", object$ptr())) {
    samplers <- data.frame(RBiips("get_node_samplers", object$ptr()))
    sorted_nodes <- cbind(sorted_nodes, samplers)
  }
  
  if (!missing(type)) {
    if (!is.character(type) || length(type) != 1) 
      stop("Invalid type argument.")
    
    type <- match.arg(type, c("Constant", "Logical", "Stochastic"))
    sorted_nodes <- sorted_nodes[sorted_nodes["type"] == type, ]
  }
  
  if (!missing(observed)) {
    if (!is.logical(observed) || length(observed) != 1) 
      stop("Invalid observed argument.")
    sorted_nodes <- sorted_nodes[sorted_nodes["observed"] == observed, ]
  }
  
  return(sorted_nodes)
}


##' @export
print_dot <- function(object, ...) UseMethod("print_dot")

##' @S3method print_dot biips
print_dot.biips <- function(object, file, ...) {
  stopifnot(is.biips(x))
  
  RBiips("print_graphviz", object$ptr(), file)
  return(invisible())
}


##' @export
build_sampler <- function(object, ...) UseMethod("build_sampler")


##' Assigns a sampler to each node of the graph
##' @param object a biips model object
##' @param proposal string. Keyword defining the type of proposal desired.
##'              Possible values are 'auto' and 'prior'. 'auto' selects the best sampler
##'              among available ones automatically. 'prior' forces asignment of the prior
##'              sampler to every node. 'prior' switches off lots of instructions and can
##'              speed up the startup of the SMC for big models.
##'              default is 'auto'
##' @param ... unused
##' @S3method build_sampler biips
build_sampler.biips <- function(object, proposal = "auto", ...) {
  stopifnot(is.biips(object))
  proposal <- match.arg(proposal, c("auto", "prior"))
  
  ## build smc sampler
  RBiips("build_smc_sampler", object$ptr(), proposal == "prior")
  
  return(invisible())
}


monitor <- function(object, ...) UseMethod("monitor")

##' @S3method monitor biips
monitor.biips <- function(object, variable_names, type = "s", ...) {
  stopifnot(is.biips(object))
  stopifnot(is.character(variable_names), length(variable_names) > 0)
  type <- check_type(type, several.ok = TRUE)
  
  pn <- parse_varnames(variable_names)
  
  for (t in type) {
    switch(t, f = RBiips("set_filter_monitors", object$ptr(), pn$names, pn$lower, 
      pn$upper), s = RBiips("set_gen_tree_smooth_monitors", object$ptr(), pn$names, 
      pn$lower, pn$upper), b = RBiips("set_backward_smooth_monitors", object$ptr(), 
      pn$names, pn$lower, pn$upper))
  }
  
  return(invisible())
}


is_monitored <- function(object, ...) UseMethod("is_monitored")

##' @S3method is_monitored biips
is_monitored.biips <- function(object, variable_names, type = "s", check_released = TRUE) {
  stopifnot(is.biips(object))
  stopifnot(is.character(variable_names), length(variable_names) > 0)
  type <- check_type(type, several.ok = FALSE)
  stopifnot(is.logical(check_released), length(check_released) == 1)
  
  pn <- parse_varnames(variable_names)
  
  ok <- switch(type, f = RBiips("is_filter_monitored", object$ptr(), pn$names, 
    pn$lower, pn$upper, check_released), s = RBiips("is_gen_tree_smooth_monitored", 
    object$ptr(), pn$names, pn$lower, pn$upper, check_released), b = RBiips("is_backward_smooth_monitored", 
    object$ptr(), pn$names, pn$lower, pn$upper, check_released))
  
  return(ok)
}


clear_monitors <- function(object, ...) UseMethod("clear_monitors")

##' Clears some monitors  
##' @param console id of the current console
##' @param type character vector with 'f' - forward
##'                                   's' - smoothing
##'                                   'b' - backward smoothing
##' @param release_only boolean flag to indicate what kind of clearing has to be done
##' @S3method clear_monitors biips
clear_monitors.biips <- function(object, type = "fsb", release_only = FALSE, ...) {
  stopifnot(is.biips(object))
  type <- check_type(type, several.ok = TRUE)
  stopifnot(is.logical(release_only), length(release_only) == 1)
  
  for (t in type) {
    switch(t, f = RBiips("clear_filter_monitors", object$ptr(), release_only), 
      s = RBiips("clear_gen_tree_smooth_monitors", object$ptr(), release_only), 
      b = RBiips("clear_backward_smooth_monitors", object$ptr(), release_only))
  }
  
  return(invisible())
}


clone_model <- function(object, ...) UseMethod("clone_model")

##' @S3method clone_model biips
clone_model.biips <- function(object, ...) {
  stopifnot(is.biips(object))
  
  mf <- tempfile()
  writeLines(object$model(), mf)
  model <- biips_model(mf, data = object$data(), sample_data = FALSE, quiet = TRUE)
  unlink(mf)
  return(model)
} 
