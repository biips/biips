##' Runs a SMC algorithm
##' @param id        id (integer) of the console
##' @param n_part    number (integer) of particules
##' @param rs_thres  resampling threshold.
##'                  If real in  [0,1] --> percentage of n_part
##'                  If integer in ]1,nb_part] --> number of particules
##' @param rs_type string belonging to c('stratified', 'systematic', 'residual', 'multinomial') indicating 
##'                the algorithm used for resampling
##'
##' @return ok boolean. True if success
run_smc_forward <- function(object, n_part, rs_thres = 0.5, rs_type = "stratified") {
    stopifnot(is.biips(x))
    stopifnot(is.numeric(n_part), length(n_part) == 1, n_part >= 1)
    stopifnot(is.numeric(rs_thres), length(rs_thres) == 1, rs_thres >= 0)
    rs_type <- match.arg(rs_type, c("multinomial", "residual", "stratified", "systematic"))
    
    ## build smc sampler
    if (!RBiips("is_sampler_built", object$ptr())) {
        RBiips("build_smc_sampler", object$ptr(), FALSE)
    }
    
    ## run smc sampler
    ok <- RBiips("run_smc_sampler", object$ptr(), as.integer(n_part), get_seed(), 
        rs_thres, rs_type)
    
    return(ok)
}




##' Generate conditional samples
##' 
##' Function to extract random weighted samples, aka. particles, from the
##' conditional distribution of the parameters of a \code{biips} model.
##' 
##' The \code{smc_samples} function creates monitors for the given types and
##' variables, runs the SMC algorithm for \code{n_part} particles and returns
##' the monitored samples.
##' 
##' Argument \code{type} specifies the type of particle values that will be
##' stored for each monitored node. Possible values are: \code{'filtering'},
##' \code{'smoothing'}, \code{'backward.smoothing'}
##' 
##' @param obj a biips model object
##' @param var_names a character vector giving the names of variables to
##' be monitored
##' @param type a character vector giving the types of monitoring desired.
##' @param n_part number of particles
##' @param backward logical flag. If \code{TRUE} then a backward smoothing
##' algorithm will be run after the forward sampling step
##' @param rs_thres threshold on the ESS criterion to control the resampling
##' step
##' @param rs_type a string indicating the resampling algorithm used
##' @param ... additional arguments
##' @return A list of \code{\link[=particles.list.object]{particles.list}}
##' objects, with one element for each element of the \code{var_names}
##' argument.
##' 
##' A \code{particles.list} object is a list of
##' \code{\link[=particles.object]{particles}} object, with one element for
##' each type of monitoring \code{type} argument.
##' @note By default, the seed of the random number generation is chosen
##' randomly. However for the sake of repatability, the optional
##' \code{smc.rng.seed} integer argument can be used to fix it.
##' @author Adrien Todeschini, Francois Caron
##' @seealso \code{\link{biips_model}}, \code{\link{pmmh.samples}},
##' \code{\link{pimh.samples}}
##' @keywords models
##' @export
##' @examples
##' 
##' ## Should be DIRECTLY executable !! 
##' ##-- ==>  Define data, use random,
##' ##--\tor do  help(data=index)  for the standard data sets.
##' 
smc_samples <- function(object, var_names, n_part, type = "fs", rs_thres = 0.5, rs_type = "stratified", 
    ...) {
    type <- check_type(type, several.ok = TRUE)
    backward <- ("b" %in% type)
    
    ## monitor
    if (backward) {
        RBiips("set_default_monitors", object$ptr())
    }
    if (!missing(var_names)) 
        monitor.biips(object, var_names, type)
    
    ## smc forward sampler
    run_smc_forward(object, n_part = n.part, rs_thres = rs.thres, rs_type = rs.type, 
        ...)
    
    log.marg.like <- RBiips("get_log_norm_const", object$ptr())
    ans <- list()
    
    mon <- RBiips("get_filter_monitors", object$ptr())
    for (n in names(mon)) {
        ans[[n]][["filtering"]] <- mon[[n]]
    }
    if (!backward) {
        clear.monitors.biips(object, type = "f")
    }
    
    mon <- RBiips("get_gen_tree_smooth_monitors", object$ptr())
    for (n in names(mon)) {
        ans[[n]][["smoothing"]] <- mon[[n]]
    }
    clear.monitors.biips(object, type = "s")
    
    ## smc backward smoother
    if (backward) {
        ## run backward smoother
        RBiips("run_backward_smoother", object$ptr())
        
        clear.monitors.biips(object, type = "f")
        
        mon <- RBiips("get_backward_smooth_monitors", object$ptr())
        for (n in names(mon)) {
            ans[[n]][["backward.smoothing"]] <- mon[[n]]
        }
        clear.monitors.biips(object, type = "b")
    }
    
    for (n in names(ans)) {
        class(ans[[n]]) <- "particles.list"
    }
    ans[["log.marg.like"]] <- log.marg.like
    
    return(ans)
} 
