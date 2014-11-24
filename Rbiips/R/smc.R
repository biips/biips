
smc_forward_algo <- function(object, ...) UseMethod("smc_forward_algo")

#' @title Run a forward SMC algorithm
#' @param object    biips model object
#' @param n_part    integer. number of particles
#' @param rs_thres  real. resampling threshold.
#' @param rs_type  string. Name of the resampling algorithm
#' @param seed integer. RNG seed passed to Biips SMC algorithm
#' @param ... unused
#'
#' @return A logical. \code{TRUE} if success.
#' @keywords internal
smc_forward_algo.biips <- function(object, n_part, rs_thres = 0.5, rs_type = "stratified",
  seed = get_seed(), ...) {
  stopifnot(is.numeric(rs_thres), length(rs_thres) == 1, rs_thres >= 0, rs_thres <=
    n_part)
  rs_type <- match.arg(rs_type, c("stratified", "systematic", "residual", "multinomial"))

  ## build smc sampler
  if (!Rbiips("is_sampler_built", object$ptr()))
    Rbiips("build_smc_sampler", object$ptr(), FALSE)

  ## run smc sampler
  ok <- Rbiips("run_smc_sampler", object$ptr(), as.integer(n_part), seed, rs_thres,
    rs_type)

  return(ok)
}


#' @title Run a sequential Monte Carlo algorithm.
#' @export
biips_smc_samples <- function(object, ...) UseMethod("biips_smc_samples")

#' @export
#' @rdname biips_smc_samples
#' @param object \code{biips} model object as returned by
#'   \code{\link{biips_model}}.
#' @param variable_names  character vector. The names of the unobserved
#'   variables to monitor, e.g.: \code{c('var1',} \code{'var2[1]',}
#'   \code{'var3[1:10]',} \code{'var4[1, 5:10, 3]')}. Subset indices must define
#'   a valid subset of the variables of the model.
#' @param n_part        integer. Number of particles.
#'
#' @param type   string containing the characters \code{'f'} (filtering),
#'   \code{'s'} (smoothing) and/or \code{'b'} (backward smoothing). Select the
#'   type of particles to be monitored. (default = \code{'fs'})
#' @param rs_thres     real. Threshold for the adaptive SMC resampling. (default
#'   = 0.5) \itemize{ \item if \code{rs_thres} is in \code{[0,1]}, resampling
#'   occurs when \code{ESS < rs_thres * n_part}. \item if \code{rs_thres} is in
#'   \code{[2,n_part]}, resampling occurs when \code{ESS < rs_thres}. }
#' @param rs_type      string. Name of the algorithm used for the SMC
#'   resampling. Possible values are \code{'stratified'}, \code{'systematic'},
#'   \code{'residual'}, \code{'multinomial'}. (default = \code{'stratified'})
#' @param ... additional arguments to pass to internal functions
#'
#' @return A \code{\link{smcarray.fsb.list}} object, with one named member for
#'   each monitored variable in the \code{variable_names} argument and a member
#'   named \code{log_marg_like} with an estimate of the log marginal likelihood.
#'
#'   A \code{\link{smcarray.fsb.list}} object is a named list of
#'   \code{\link{smcarray.fsb}} objects for different variables. Each
#'   \code{\link{smcarray.fsb}} object is a named list of \code{\link{smcarray}}
#'   object, with one member for each type of monitoring (\code{f}, \code{s}
#'   and/or \code{b}) in the \code{type} argument. Assuming \code{dim} is the
#'   dimension of the monitored variable, a \code{\link{smcarray}} object is a
#'   list with the members:
#'   \itemize{
#'     \item \code{values}: array of dimension \code{c(dim, n_part)} with the
#'       values of the particles.
#'     \item \code{weights}: array of dimension \code{c(dim, n_part)} with
#'       the weights of the particles.
#'     \item \code{ess}: array of dimension \code{dim} with Effective Sample Sizes (ESS)
#'       of the particles set.
#'     \item \code{discrete}: array of dimension \code{dim} with
#'       logicals indicating discreteness of each component.
#'     \item \code{iterations}: array of dimension \code{dim} with sampling iterations of
#'       each component.
#'     \item \code{conditionals}: lists of the contitioning variables
#'       (observations). Its value is: \itemize{
#'         \item for filtering: a list of dimension \code{dim}. each member is a
#'           character vector with the respective conditioning variables of the
#'           node array component.
#'         \item for smoothing/backward_smoothing: a character vector, the same
#'           for all the components of the node array.
#'         }
#'     \item \code{name}: string with the name of the variable (without subset indices).
#'     \item \code{lower}: vector with the lower bounds of the variable.
#'     \item \code{upper}: vector with the upper bounds of the variable.
#'     \item \code{type}: string with the type of monitor (\code{'filtering'},
#'       \code{'smoothing'} or \code{'backward_smoothing'}).
#'   }
#'   For instance, if \code{out_smc} is a \code{\link{smcarray.fsb.list}} object,
#'   one can access the values of the smoothing particles for the variable
#'   \code{'x'} with: \code{out_smc$x$s$values}.
#'
#' @seealso \code{\link{biips_model}}, \code{\link{biips_diagnosis}},
#'   \code{\link{biips_summary}}, \code{\link{biips_density}},
#'   \code{\link{biips_table}}
#' @examples
#' modelfile <- system.file('extdata', 'hmm.bug', package = 'Rbiips')
#' stopifnot(nchar(modelfile) > 0)
#' cat(readLines(modelfile), sep = '\n')
#'
#' data <- list(tmax = 10, p = c(.5, .5), logtau_true = log(1), logtau = log(1))
#' model <- biips_model(modelfile, data, sample_data = TRUE)
#'
#' n_part <- 100
#' out_smc <- biips_smc_samples(model, c('x', 'c[2:10]'), n_part, type = 'fs',
#'                              rs_thres = 0.5, rs_type = 'stratified')
#'
#' biips_diagnosis(out_smc)
#'
#' summ_smc_x <- biips_summary(out_smc$x, order = 2, probs = c(.025, .975))
#' dens_smc_x <- biips_density(out_smc$x, bw = 'nrd0', adjust = 1, n = 100)
#' summ_smc_c <- biips_summary(out_smc[['c[2:10]']])
#' table_smc_c <- biips_table(out_smc[['c[2:10]']])
#'
#' par(mfrow = c(2, 2))
#' plot(model$data()$x_true, type = 'l', col = 'green', xlab = 't', ylab = 'x[t]')
#' lines(summ_smc_x$f$mean, col = 'blue')
#' lines(summ_smc_x$s$mean, col = 'red')
#' matlines(matrix(unlist(summ_smc_x$f$quant), data$tmax), lty = 2, col = 'blue')
#' matlines(matrix(unlist(summ_smc_x$s$quant), data$tmax), lty = 2, col = 'red')
#' legend('topright', leg = c('true', 'SMC filtering estimate', 'SMC smoothing estimate'),
#'        lty = 1, col = c('green', 'blue', 'red'), bty = 'n')
#'
#' barplot(.5*(model$data()$c_true==1), col = 'green', border = NA, space = 0, offset=2,
#'         ylim=c(0,3), xlab='t', ylab='c[t]==1', axes = FALSE)
#' axis(1, at=1:data$tmax-.5, labels=1:data$tmax)
#' axis(2, line = 1, at=c(0,3), labels=NA)
#' text(data$tmax/2, 2.75, 'true')
#' barplot(.5*c(NA, summ_smc_c$f$mode==1), col = 'blue', border = NA, space = 0,
#'         offset=1, axes = FALSE, add = TRUE)
#' text(data$tmax/2, 1.75, 'SMC filtering mode')
#' barplot(.5*c(NA, summ_smc_c$s$mode==1), col = 'red', border = NA, space = 0,
#'         axes = FALSE, add = TRUE)
#' text(data$tmax/2, .75, 'SMC smoothing mode')
#'
#' t <- 5
#' plot(dens_smc_x[[t]], col = c('blue','red'), ylab = 'posterior density')
#' points(model$data()$x_true[t], 0, pch = 17, col = 'green')
#'
#' plot(table_smc_c[[t-1]], col = c('blue','red'), ylab = 'posterior probability mass')
#' points(model$data()$c_true[t], 0, pch = 17, col = 'green')
biips_smc_samples.biips <- function(object, variable_names, n_part, type = "fs",
  rs_thres = 0.5, rs_type = "stratified", ...) {

  ## check arguments
  stopifnot(is.biips(object))
  stopifnot(is.numeric(n_part), length(n_part) == 1, n_part >= 1, is.finite(n_part))
  type <- check_type(type, several.ok = TRUE)

  backward <- ("b" %in% type)

  ## monitor
  if (backward)
    Rbiips("set_default_monitors", object$ptr())

  if (!missing(variable_names))
    monitor(object, variable_names, type)

  ## smc forward sampler
  smc_forward_algo(object, n_part = n_part, rs_thres = rs_thres, rs_type = rs_type,
    ...)

  log_marg_like <- Rbiips("get_log_norm_const", object$ptr())
  out_smc <- list()

  mon <- Rbiips("get_filter_monitors", object$ptr())
  for (n in names(mon)) out_smc[[n]]$f <- mon[[n]]

  if (!backward)
    clear_monitors(object, type = "f")

  mon <- Rbiips("get_gen_tree_smooth_monitors", object$ptr())
  for (n in names(mon)) out_smc[[n]]$s <- mon[[n]]

  clear_monitors(object, type = "s")

  ## smc backward smoother
  if (backward) {
    ## run backward smoother
    Rbiips("run_backward_smoother", object$ptr())

    clear_monitors(object, type = "f")

    mon <- Rbiips("get_backward_smooth_monitors", object$ptr())
    for (n in names(mon)) out_smc[[n]]$b <- mon[[n]]

    clear_monitors(object, type = "b")
  }

  for (n in names(out_smc)) class(out_smc[[n]]) <- "smcarray.fsb"

  out_smc$log_marg_like <- log_marg_like

  class(out_smc) <- "smcarray.fsb.list"

  return(out_smc)
}
