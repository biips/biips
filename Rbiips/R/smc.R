
smc_forward_algo <- function(object, ...) UseMethod("smc_forward_algo")

##' Runs a forward SMC algorithm
##' @param object    biips model object
##' @param n_part    integer. number of particles
##' @param rs_thres  real. resampling threshold.
##' @param rs_type  string. Name of the resampling algorithm
##' @param seed integer. RNG seed passed to Biips SMC algorithm
##' @param ... unused
##'
##' @return A boolean. True if success.
##' @keywords internal
smc_forward_algo.biips <- function(object, n_part, rs_thres = 0.5,
    rs_type = "stratified", seed = get_seed(), ...) {
    stopifnot(is.numeric(rs_thres), length(rs_thres) == 1, rs_thres >=
        0, rs_thres <= n_part)
    rs_type <- match.arg(rs_type, c("stratified", "systematic",
        "residual", "multinomial"))

    ## build smc sampler
    if (!Rbiips("is_sampler_built", object$ptr()))
        Rbiips("build_smc_sampler", object$ptr(), FALSE)

    ## run smc sampler
    ok <- Rbiips("run_smc_sampler", object$ptr(), as.integer(n_part),
        seed, rs_thres, rs_type)

    return(ok)
}


#' Run a sequential Monte Carlo algorithm.
#' @export
biips_smc_samples <- function(object, ...) UseMethod("biips_smc_samples")

#' @export
#' @rdname biips_smc_samples
#' @param object \code{biips} model object as returned by \code{\link{biips_model}}.
#' @param variable_names  character vector. Contains the names of the
#'                      unobserved variables to monitor.
#'                      Example: \code{c("var1", "var2[1]", "var3[1:10]", "var4[1, 5:10, 3]")}.
#'                      Subset indices must define a valid subset of the variables
#'                      of the model.
#' @param n_part        integer. Number of particles.
#'
#' @param type   string containing the characters \code{'f'} (filtering), \code{'s'} (smoothing)
#'               and/or \code{'b'} (backward smoothing). Select the type of particles to be monitored.
#'               (default = \code{'fs'})
#' @param rs_thres     real. Threshold for the adaptive SMC resampling. (default = 0.5)
#'   \itemize{
#'     \item if \code{rs_thres} is in \code{[0,1]}, resampling occurs when
#'                     \code{ESS < rs_thres * n_part}.
#'     \item if \code{rs_thres} is in \code{[2,n_part]}, resampling occurs when
#'                     \code{ESS < rs_thres}.
#'   }
#' @param rs_type      string. Name of the algorithm used for the SMC resampling.
#'                   Possible values are \code{"stratified"}, \code{"systematic"},
#'                   \code{"residual"}, \code{"multinomial"}. (default = \code{"stratified"})
#' @param ... additional arguments to pass to internal functions
#'
#' @return A \code{\link{smcarray.fsb.list}} object, with one member for each variable in the \code{variable_names}
#' argument and a member named \code{log_marg_like} with an estimate of the log marginal likelihood.
#'
#'   A \code{\link{smcarray.fsb.list}} object is a list of \code{smcarray.fsb} objects for
#'   different variables.
#'   Each \code{smcarray.fsb} object is a list of
#'   \code{\link{smcarray}} object, with one member for each type of monitoring
#'   (\code{f}, \code{s} and/or \code{b}) in the \code{type} argument.
#'   Assuming \code{dim} is the dimension of the monitored variable, a \code{\link{smcarray}}
#'   object is a list with the members:
#'   \item{values}{array of dimension \code{c(dim, n_part)} with the values of the particles.}
#'   \item{weights}{array of dimension \code{c(dim, n_part)} with the weights of the particles.}
#'   \item{ess}{array of dimension \code{dim} with Effective Sample Sizes (ESS) of the particles set.}
#'   \item{discrete}{array of dimension \code{dim} with booleans indicating discreteness of each component.}
#'   \item{iterations}{array of dimension \code{dim} with sampling iterations of each component.}
#'   \item{conditionals}{lists of the contitioning variables (observations).
#'     Its value is:
#'     \itemize{
#'       \item for filtering: a list of dimension \code{dim}. each member is a character vector
#'                         with the respective conditioning variables of the node array component.
#'       \item for smoothing/backward_smoothing: a character vector, the same for all the
#'                         components of the node array.
#'     }}
#'   \item{name}{string with the name of the variable.}
#'   \item{lower}{vector with the lower bounds of the variable.}
#'   \item{upper}{vector with the upper bounds of the variable.}
#'   \item{type}{string with the type of monitor (\code{"filtering"}, \code{"smoothing"}
#'     or \code{"backward_smoothing"}).}
#'   For instance, if \code{out_smc} is a \code{\link{smcarray.fsb.list}} object,
#'   one can access the values of the smoothing particles for the variable \code{"x"} with:
#'     \code{out_smc$x$s$values}.
#'
#' @seealso \code{\link{biips_model}}, \code{\link{biips_diagnosis}}, \code{\link{biips_summary}},
#'   \code{\link{biips_density}}, \code{\link{biips_table}}
#' @examples
#' modelfile <- system.file("extdata", "hmm.bug", package = "Rbiips")
#' cat(readLines(modelfile), sep="\n")
#'
#' data <- list(tmax = 10, logtau = log(10))
#' model <- biips_model(modelfile, data, sample_data = TRUE)
#'
#' n_part <- 100
#' out_smc <- biips_smc_samples(model, c("x[1]", "x[5:10]"), n_part, type = "fs",
#'                              rs_thres = 0.5, rs_type = "stratified")
#'
#' names(out_smc)
#' out_smc
#' names(out_smc[["x[5:10]"]])
#' out_smc[["x[5:10]"]]
#' names(out_smc[["x[5:10]"]]$f)
#' out_smc[["x[5:10]"]]$f
#' out_smc[["x[5:10]"]]$s
#'
#' out_smc <- biips_smc_samples(model, "x", n_part)
#'
#' biips_diagnosis(out_smc)
#' summ_smc <- biips_summary(out_smc, order = 2, probs = c(0.025, 0.975))
#' dens_smc <- biips_density(out_smc)
biips_smc_samples.biips <- function(object, variable_names, n_part,
    type = "fs", rs_thres = 0.5, rs_type = "stratified", ...) {

    ## check arguments
    stopifnot(is.biips(object))
    stopifnot(is.numeric(n_part), length(n_part) == 1, n_part >=
        1, is.finite(n_part))
    type <- check_type(type, several.ok = TRUE)

    backward <- ("b" %in% type)

    ## monitor
    if (backward)
        Rbiips("set_default_monitors", object$ptr())

    if (!missing(variable_names))
        monitor(object, variable_names, type)

    ## smc forward sampler
    smc_forward_algo(object, n_part = n_part, rs_thres = rs_thres,
        rs_type = rs_type, ...)

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
