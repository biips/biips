
#' @title Create a biips model object described in BUGS language.
#'
#' @param file     string. path of the BUGS file which defines the stochastic
#'   model. Alternatively, \code{file} can be a readable text-mode connection,
#'   or a complete URL.
#' @param data     either a list or environment containing constants and
#'   observed values or a character vector containing names of variables
#'   existing in the workspace. Any numeric objects in \code{data} corresponding
#'   to node arrays used in \code{file} are taken to represent the values of
#'   observed nodes in the model.
#' @param sample_data   logical. Toggle the evaluation of the 'data' block of
#'   the BUGS model that generates or transforms data. (default = \code{TRUE}).
#' @param quiet    logical. Deactivate verbosity. (default = \code{FALSE}).
#'
#' @return An object of class \code{biips} which can be used to generate samples
#'   from the posterior distribution of the parameters.
#'
#'   An object of class \code{biips} is a list of functions that share a common
#'   environment. The functions can be used to query information on the model.
#'   \itemize{
#'     \item \code{ptr()}: Get an external pointer to the compiled model object.
#'     \item \code{file()}: Get a string. filename of the BUGS model
#'     \item \code{data()}: Get a named list of the original data of the model.
#'     \item \code{model()}: Get a character vector. BUGS language definition of the model.
#'     \item \code{recompile()}: Recompile the model using the original data set.
#'  }
#' @keywords models graphs
#' @export
#' @seealso \code{\link{biips_variable_names}}, \code{\link{biips_nodes}},
#'   \code{\link{biips_print_dot}}, \code{\link{biips_add_function}},
#'   \code{\link{biips_add_distribution}}
#' @examples
#' modelfile <- system.file('extdata', 'hmm.bug', package = 'Rbiips')
#' stopifnot(nchar(modelfile) > 0)
#' cat(readLines(modelfile), sep = '\n')
#'
#' data <- list(tmax = 10, p = c(.5, .5), logtau_true = log(1), logtau = log(1))
#' model <- biips_model(modelfile, data, sample_data = TRUE)
#'
#' \dontrun{
#' tmax <- 10
#' p <- c(.5, .5)
#' logtau_true <- log(1)
#' logtau <- logtau_true
#'
#' datanames <- c('tmax', 'p', 'logtau_true', 'logtau')
#' model <- biips_model(modelfile, datanames, sample_data = TRUE)
#' }
#'
#' is.biips(model)
#' print(model)
#'
#' model$data()
#'
#' variable.names(model)
#' biips_variable_names(model)
#'
#' biips_nodes(model)
#'
#' dotfile <- 'hmm.dot'
#' biips_print_dot(model, dotfile)
#' #cat(readLines(dotfile), sep = '\n')
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
    warning("Unused variables in data: ", paste(unused, collapse = ", "))

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
