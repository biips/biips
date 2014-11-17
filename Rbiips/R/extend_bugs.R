#' Add a custom R function or distribution to the BUGS language.
#'
#' @export
#' @name extend-bugs
#' @param name     string. Name of the custom function that will be used in the BUGS
#'   model. must be a valid BUGS language function name.
#' @param n_param  integer. Number of arguments of the custom function or distribution.
#' @param fun_dim  custom R function returning the size vector of the output. It will
#'   be called when compiling the model. Its arguments are the dimension vectors
#'   of the inputs.
#' @param fun_eval custom R function which evaluates the function. Its arguments are
#'   the parameters values.
#'
#' @param fun_check_param  custom R function which checks if the argument values are
#'   valid. Its arguments are the parameters values. Returns a logical. (default
#'   returns \code{TRUE})
#' @param fun_is_discrete custom R function returning a logical that is \code{TRUE} if
#'   the output is discrete. Its arguments are logicals indicating if the
#'   arguments are discrete. (default returns \code{FALSE})
#'
#'   All the given R functions must have the same number of input arguments.
#' @seealso \code{\link{biips_model}}
#' @examples
#' #' # Add custom functions and distributions to BUGS language
#' #' Add custom function `f`
#' f_dim <- function(x_dim, t_dim) {
#'   # Check dimensions of the input and return dimension of the output of function f
#'   stopifnot(prod(x_dim) == 1, prod(t_dim) == 1)
#'   x_dim
#' }
#' f_eval <- function(x, t) {
#'   # Evaluate function f
#'   0.5 * x + 25 * x/(1 + x^2) + 8 * cos(1.2 * t)
#' }
#' biips_add_function('f', 2, f_dim, f_eval)
#'
#' #' Add custom sampling distribution `dMN`
#' dMN_dim <- function(mu_dim, Sig_dim) {
#'   # Check dimensions of the input and return dimension of the output of
#'   # distribution dMN
#'   stopifnot(prod(mu_dim) == mu_dim[1], length(Sig_dim) == 2, mu_dim[1] == Sig_dim)
#'   mu_dim
#' }
#' dMN_sample <- function(mu, Sig) {
#'   # Draw a sample of distribution dMN
#'   mu + t(chol(Sig)) %*% rnorm(length(mu))
#' }
#' biips_add_distribution('dMN', 2, dMN_dim, dMN_sample)
#'
#' #' # Compile model
#' modelfile <- system.file('extdata', 'hmm_f.bug', package = 'Rbiips')
#' stopifnot(nchar(modelfile) > 0)
#' cat(readLines(modelfile), sep = '\n')
#'
#' data <- list(tmax = 10, p = c(.5, .5), logtau_true = log(1), logtau = log(1))
#' model <- biips_model(modelfile, data, sample_data = TRUE)
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



#' In the current version, the custom distributions can only be used for unobserved nodes.
#'
#' @rdname extend-bugs
#' @export
#' @param fun_sample custom R function which samples from the distribution. Its
#'   arguments are the parameters values.
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
