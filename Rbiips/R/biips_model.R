
##' Create a biips model object
##'
##' \code{biips_model} is used to create an object representing a Bayesian
##' graphical model, specified with a BUGS language description of the prior
##' distribution, and a set of data.
##'
##' @param file a file containing a description of the model in the Biips/JAGS
##' dialect of the BUGS language.
##'
##' Alternatively, \code{file} can be a readable text-mode connection, or a
##' complete URL.
##' @param data a list or environment containing the data. Any numeric objects
##' in \code{data} corresponding to node arrays used in \code{file} are taken
##' to represent the values of observed nodes in the model
##' @param sample_data logical flag. If \code{FALSE} then the \code{data} block
##' of the model will be ignored.
##' @param quiet if \code{TRUE} then messages generated during compilation will
##' be suppressed.
##' @return \code{biips_model} returns an object inheriting from class
##' \code{biips} which can be used to generate dependent samples from the
##' posterior distribution of the parameters
##'
##' An object of class \code{biips} is a list of functions that share a common
##' environment. The functions can be used to query information on the model.
##' \item{ptr()}{Returns an external pointer to an object created by the Biips
##' library.}
##' \item{data()}{Returns a list containing the data that defines the
##' observed nodes in the model.}
##' \item{model()}{Returns a character vector
##' containing the BUGS-language representation of the model.}
##' \item{print_dot(file)}{Writes a description of the graph in dot language in
##' \code{file}.}
##' \item{nodes(type, observed)}{Returns a \code{data.frame}
##' containing information on all the nodes of the graph: rank in the
##' topological sort, node id, variable name or formula, type, observed. After
##' calling \code{build.sampler} or any algorithm running smc sampler, the
##' \code{data.frame} will also contain a column indicating the node sampler
##' used for unobserved stochastic nodes.}
##' \item{recompile()}{Recompiles the
##' model using the original data set.}
##' @author Adrien Todeschini, Francois Caron
##' COPY: Adapted from rjags jags.model
##' @keywords models graphs
##' @export
##' @examples
##'
##' ## Should be DIRECTLY executable !!
##' ##-- ==>  Define data, use random,
##' ##--\tor do  help(data=index)  for the standard data sets.
##'
biips_model <- function(file, data = parent.frame(), sample_data = TRUE, quiet = FALSE) {
  if (missing(file)) {
    stop("Model file name missing")
  }
  stopifnot(is.logical(sample_data), length(sample_data) == 1)
  stopifnot(is.logical(quiet), length(quiet) == 1)

  if (is.character(file)) {
    f <- try(file(file, "rt"))
    if (inherits(f, "try-error")) {
      stop("Cannot open model file \"", file, "\"")
    }
    close(f)
    model_code <- readLines(file, warn = FALSE)
    filename <- file
  } else if (!inherits(file, "connection")) {
    stop("'file' must be a character string or connection")
  } else {
    model_code <- readLines(file, warn = FALSE)
    filename <- tempfile()
    writeLines(model_code, filename)
  }

  if (quiet) {
    verb <- Rbiips("verbosity", 0)
    on.exit(Rbiips("verbosity", verb), add = TRUE)
  }

  # check data before compiling model, which typically takes more time
  if (is.character(data))
    data <- mklist(data)
  if (length(data) > 0)
    data <- data_preprocess(data)

  # make console and check model
  ptr <- Rbiips("make_console")
  Rbiips("check_model", ptr, filename)

  # discard unused data
  varnames <- Rbiips("get_variable_names", ptr)
  unused <- setdiff(names(data), varnames)
  data[unused] <- NULL
  if (length(unused) > 0)
    warning("Unused variables in data: ", paste(unused_variables, collapse = ", "))

  # compile model
  Rbiips("compile_model", ptr, data, sample_data, get_seed())

  # data after possible sampling (from 'data' block in the BUGS language model)
  model_data <- Rbiips("get_data", ptr)

  ## Output object of class biips Note: We return functions in model list that use
  ## variables of the parent environment (ie the currrent function environment).
  ## This specific R trick allows to read and write persistent variables,
  ## surrogating a class with private members and their modifiers.
  model <- list(ptr = function() {
    ptr
  }, file = function() {
    file
  }, model = function() {
    model_code
  }, data = function() {
    model_data
  }, .data_sync = function() {
    Rbiips("get_data", ptr)
  }, recompile = function() {
    ## Clear the console
    Rbiips("clear_console", ptr)
    ptr <<- Rbiips("make_console")
    ## Write the model to a temporary file so we can re-read it
    mf <- tempfile()
    writeLines(model_code, mf)
    Rbiips("check_model", ptr, mf)
    unlink(mf)
    ## Re-compile generate new data if sample_data is TRUE
    Rbiips("compile_model", ptr, data, sample_data, get_seed())
    model_data <<- Rbiips("get_data", ptr)
    invisible()
  })
  class(model) <- "biips"

  return(model)
}


##' Add the corresponding R function to the bugs model
##' @param name name of the new  function
##' @param n_param number of arguments of the new function
##' @param fun_dim R function returning a vector containg arguments sizes
##' @param fun_eval R function computing the result of function
##' @param fun_check_param R function checking the arguments
##' @param fun_is_discrete R function telling is new function is discrete wrt its arguments
##' @export
biips_add_function <- function(name, n_param, fun_dim, fun_eval, fun_check_param = function(...) TRUE,
                               fun_is_discrete = function(...) FALSE) {
  stopifnot(is.character(name), length(name) == 1, nchar(name) > 0)
  stopifnot(is.numeric(n_param), length(n_param) == 1, n_param > 0)
  stopifnot(is.function(fun_dim))
  stopifnot(is.function(fun_eval))
  stopifnot(is.function(fun_check_param))
  stopifnot(is.function(fun_is_discrete))

  Rbiips("add_function", name, n_param, fun_dim, fun_eval, fun_check_param, fun_is_discrete)
  cat("* Added function", name, "\n")

  return(invisible())
}



##' Add the corresponding R distribution to the bugs model
##' @param name name of the new distribution
##' @param n_param number of arguments of the new distribution
##' @param fun_dim R function returning a vector containg arguments sizes
##' @param fun_sample R function sampling from the distribution
##' @param fun_check_param R function checking the arguments
##' @param fun_is_discrete R function telling is new function is discrete wrt its arguments
##' @export
biips_add_distribution <- function(name, n_param, fun_dim, fun_sample, fun_check_param = function(...) TRUE,
                                   fun_is_discrete = function(...) FALSE) {
  stopifnot(is.character(name), length(name) == 1, nchar(name) > 0)
  stopifnot(is.numeric(n_param), length(n_param) == 1, n_param > 0)
  stopifnot(is.function(fun_dim))
  stopifnot(is.function(fun_sample))
  stopifnot(is.function(fun_check_param))
  stopifnot(is.function(fun_is_discrete))

  Rbiips("add_distribution", name, n_param, fun_dim, fun_sample, fun_check_param,
         fun_is_discrete)
  cat("* Added distribution", name, "\n")

  return(invisible())
}
