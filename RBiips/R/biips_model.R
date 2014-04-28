
##' Create a biips model object
##' 
##' \code{biips_model} is used to create an object representing a Bayesian
##' graphical model, specified with a BUGS language description of the prior
##' distribution, and a set of data.
##' 
##' @param file a file containing a description of the model in the BiiPS/JAGS
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
##' \item{ptr()}{Returns an external pointer to an object created by the BiiPS
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
    modfile <- file
  } else if (!inherits(file, "connection")) {
    stop("'file' must be a character string or connection")
  } else {
    model_code <- readLines(file, warn = FALSE)
    modfile <- tempfile()
    writeLines(model_code, modfile)
  }
  
  if (quiet) {
    verb <- RBiips("verbosity", 0)
    on.exit(RBiips("verbosity", verb), add = TRUE)
  }
  
  # check data before compiling model, which typically takes more time
  if (is.character(data)) 
    data <- mklist(data)
  if (length(data) > 0) 
    data <- data_preprocess(data)
  
  # make console and check model
  ptr <- RBiips("make_console")
  RBiips("check_model", ptr, modfile)
  
  # discard unused data
  varnames <- RBiips("get_variable_names", ptr)
  
  v <- names(data)
  relevant_variables <- v %in% varnames
  data <- data[relevant_variables]
  unused_variables <- setdiff(v, varnames)
  if (length(unused_variables) > 0) 
    warning("Unused variables in data: ", paste(unused_variables, collapse = ", "))
  
  # compile model
  RBiips("compile_model", ptr, data, sample_data, get_seed())
  
  # data after possible sampling (from 'data' block in the BUGS language model)
  model_data <- RBiips("get_data", ptr)
  
  ## Output object of class biips Note: We return functions in model list that use
  ## variables of the parent environment (ie the currrent function environment).
  ## This specific R trick allows to read and write persistent variables,
  ## surrogating a class with private members and their modifiers.
  model <- list(ptr = function() {
    ptr
  }, model = function() {
    model_code
  }, data = function() {
    model_data
  }, .data_sync = function() {
    RBiips("get_data", ptr)
  }, recompile = function() {
    ## Clear the console
    RBiips("clear_console", ptr)
    ptr <<- RBiips("make_console")
    ## Write the model to a temporary file so we can re-read it
    mf <- tempfile()
    writeLines(model_code, mf)
    RBiips("check_model", ptr, mf)
    unlink(mf)
    ## Re-compile
    RBiips("compile_model", ptr, data, FALSE, get_seed())
    return(invisible())
  })
  class(model) <- "biips"
  
  return(model)
} 
