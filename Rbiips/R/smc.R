
smc_forward_algo <- function(object, ...) UseMethod("smc_forward_algo")

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
##' @export
smc_forward_algo.biips <- function(object, n_part, rs_thres = 0.5, rs_type = "stratified", 
  ...) {
  stopifnot(is.biips(object))
  stopifnot(is.numeric(n_part), length(n_part) == 1, n_part >= 1, is.finite(n_part))
  stopifnot(is.numeric(rs_thres), length(rs_thres) == 1, rs_thres >= 0, rs_thres <= 
    n_part)
  rs_type <- match.arg(rs_type, c("stratified", "systematic", "residual", "multinomial"))
  
  ## build smc sampler
  if (!Rbiips("is_sampler_built", object$ptr())) 
    Rbiips("build_smc_sampler", object$ptr(), FALSE)
  
  ## run smc sampler
  ok <- Rbiips("run_smc_sampler", object$ptr(), as.integer(n_part), get_seed(), 
    rs_thres, rs_type)
  
  return(ok)
}



##' @export
smc_samples <- function(object, ...) UseMethod("smc_samples")

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
##' stored for each monitored node. Possible values are: \code{'f'},
##' \code{'s'}, \code{'b'}
##' 
##' @param obj a biips model object
##' @param variable_names a character vector giving the names of variables to
##' be monitored
##' @param type a character vector giving the types of monitoring desired.
##' @param n_part number of particles
##' @param backward logical flag. If \code{TRUE} then a backward smoothing
##' algorithm will be run after the forward sampling step
##' @param rs_thres threshold on the ESS criterion to control the resampling
##' step
##' @param rs_type a string indicating the resampling algorithm used
##' @param ... additional arguments to be passed to the SMC algorithm (currently unused)
##' @return A list of \code{\link[=smcarray.list.object]{smcarray.list}}
##' objects, with one element for each element of the \code{variable_names}
##' argument.
##' 
##' A \code{smcarray.list} object is a list of
##' \code{\link[=smcarray.object]{smcarray}} object, with one element for
##' each type of monitoring \code{type} argument.
##' @note By default, the seed of the random number generation is chosen
##' randomly.
##' @author Adrien Todeschini, Francois Caron
##' @seealso \code{\link{biips_model}}, \code{\link{pmmh.samples}},
##' \code{\link{pimh_samples}}
##' @keywords models
##' @export
##' @examples
##' 
##' ## Should be DIRECTLY executable !! 
##' ##-- ==>  Define data, use random,
##' ##--\tor do  help(data=index)  for the standard data sets.
##' 
smc_samples.biips <- function(object, variable_names, n_part, type = "fs", rs_thres = 0.5, 
  rs_type = "stratified", ...) {
  
  ## check arguments
  stopifnot(is.biips(object))
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
  
  for (n in names(out_smc)) class(out_smc[[n]]) <- "smcarray.list"
  
  out_smc$log_marg_like <- log_marg_like
  
  class(out_smc) <- "smcarray.list.list"
  
  return(out_smc)
} 
