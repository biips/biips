#' Add a R function or distribution to the BUGS language.
#'
#' @export
#' @param name     string. Name of the function that will be used in the BUGS model.
#'                 must be a valid BUGS language function name.
#' @param n_param  integer. Number of arguments of the function
#' @param fun_dim  R function returning the size
#'                 vector of the output. It will be called when compiling the
#'                 model. Its arguments are the dimension vectors of the
#'                 inputs.
#' @param fun_eval R function which evaluates the function.
#'                 Its arguments are the parameters values.
#'
#' @param fun_check_param  R function which checks if
#'                      the argument values are valid. Its arguments are the parameters values.
#'                      Returns a boolean. (default returns \code{TRUE})
#' @param fun_is_discrete R function returning a boolean that is \code{TRUE} if the
#'                      output is discrete. Its arguments are booleans
#'                      indicating if the arguments are discrete.
#'                      (default returns \code{FALSE})
#'
#'   All the given R functions must have the same number of input
#'   arguments.
#' @seealso \code{\link{biips_model}}
#' @examples
#' ## Add function f to BUGS language
#' f_dim <- function(x_dim, t_dim) {
#'   # Check dimensions of the input and return
#'   # dimension of the output of function f
#'   stopifnot(prod(x_dim)==1, prod(t_dim)==1)
#'   x_dim
#' }
#' f_eval <- function(x, t) {
#'   # Evaluate function f
#'   0.5*x+25*x/(1+x^2)+8*cos(1.2*t)
#' }
#' biips_add_function('f', 2, f_dim, f_eval)
biips_add_function <- function(name, n_param, fun_dim, fun_eval,
                               fun_check_param = function(...) TRUE, fun_is_discrete = function(...) FALSE) {
  stopifnot(is.character(name), length(name) == 1, nchar(name) >
              0)
  stopifnot(is.numeric(n_param), length(n_param) == 1, n_param >
              0)
  stopifnot(is.function(fun_dim))
  stopifnot(is.function(fun_eval))
  stopifnot(is.function(fun_check_param))
  stopifnot(is.function(fun_is_discrete))

  Rbiips("add_function", name, n_param, fun_dim, fun_eval,
         fun_check_param, fun_is_discrete)
  cat("* Added function", name, "\n")

  return(invisible())
}



#' Currently, the added sampling distributions can only define
#' unobserved nodes.
#'
#' @rdname biips_add_function
#' @export
#' @param fun_sample  R function which samples from
#'   the distribution. Its arguments are the parameters values.
#'
#' @examples
#' ## Add sampling distribution dMN to BUGS language
#' dMN_dim <- function(mu_dim, Sig_dim) {
#'   # Check dimensions of the input and return
#'   # dimension of the output of distribution dMN
#'   stopifnot(prod(mu_dim)==mu_dim[1], length(Sig_dim)==2, mu_dim[1]==Sig_dim)
#'   mu_dim
#' }
#' dMN_sample <- function(mu, Sig) {
#'   # Draw a sample of distribution dMN
#'   mu+t(chol(Sig))%*%rnorm(length(mu))
#' }
#' biips_add_distribution('dMN', 2, dMN_dim, dMN_sample)
biips_add_distribution <- function(name, n_param, fun_dim, fun_sample,
                                   fun_check_param = function(...) TRUE, fun_is_discrete = function(...) FALSE) {
  stopifnot(is.character(name), length(name) == 1, nchar(name) >
              0)
  stopifnot(is.numeric(n_param), length(n_param) == 1, n_param >
              0)
  stopifnot(is.function(fun_dim))
  stopifnot(is.function(fun_sample))
  stopifnot(is.function(fun_check_param))
  stopifnot(is.function(fun_is_discrete))

  Rbiips("add_distribution", name, n_param, fun_dim, fun_sample,
         fun_check_param, fun_is_discrete)
  cat("* Added distribution", name, "\n")

  return(invisible())
}

#' Create a \code{biips} model object described in BUGS language.
#'
#' @param file     string. path of the BUGS file which defines the stochastic model.
#' Alternatively, \code{file} can be a readable text-mode connection, or a
#' complete URL.
#' @param data     either a list or environment containing constants and observed values
#'                 or a character vector containing names of variables
#'                 existing in the workspace. Any numeric objects
#'                 in \code{data} corresponding to node arrays used in \code{file} are taken
#'                 to represent the values of observed nodes in the model.
#' @param sample_data   boolean. Toggle the evaluation of the 'data' block of the
#'                 BUGS model that generates or transforms data. (default = \code{TRUE}).
#' @param quiet    boolean. Deactivate verbosity. (default = \code{FALSE}).
#'
#' @return An object of class \code{biips} which can be used to generate samples from the
#' posterior distribution of the parameters.
#'
#' An object of class \code{biips} is a list of functions that share a common
#' environment. The functions can be used to query information on the model.
#' \item{ptr()}{Returns an external pointer to the compiled model object.}
#' \item{file()}{Returns a string. filename of the BUGS model}
#' \item{data()}{Returns a list. observed data of the model.}
#' \item{model()}{Returns a character vector. BUGS language definition of the model.}
#' \item{recompile()}{Recompiles the model using the original data set.}
#' @keywords models graphs
#' @export
#' @seealso \code{\link{biips_variable_names}}, \code{\link{biips_nodes}},
#' \code{\link{biips_print_dot}}, \code{\link{biips_add_function}}, \code{\link{biips_add_distribution}}
#' @examples
#' modelfile <- system.file("extdata", "hmm.bug", package = "Rbiips")
#' cat(readLines(modelfile), sep="\n")
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
#' dotfile = "hmm.dot"
#' biips_print_dot(model, dotfile)
#' cat(readLines(dotfile), sep="\n")
#'
#' rm(model)
#'
#' tmax <- 10
#' logtau <- log(10)
#' datanames <- c("tmax", "logtau")
#' model <- biips_model(modelfile, datanames, sample_data = TRUE)
biips_model <- function(file, data = parent.frame(), sample_data = TRUE,
    quiet = FALSE) {
    if (missing(file)) {
        stop("Model file name missing")
    }
    stopifnot(is.logical(sample_data), length(sample_data) ==
        1)
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

    # check data before compiling model, which typically takes
    # more time
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

    # data after possible sampling (from 'data' block in the BUGS
    # language model)
    model_data <- Rbiips("get_data", ptr)

    ## Output object of class biips Note: We return functions in
    ## model list that use variables of the parent environment (ie
    ## the currrent function environment).  This specific R trick
    ## allows to read and write persistent variables, surrogating
    ## a class with private members and their modifiers.
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