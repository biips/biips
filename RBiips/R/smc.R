run.smc.forward <- function(object, n.part, rs.thres = 0.5, rs.type = "stratified",
                            smc.rng.seed)
{
  if (!is.biips(object))
    stop("Invalid BiiPS model")
  
  if (!is.numeric(n.part) || length(n.part)!=1 || n.part < 1) {
    stop("Invalid n.part argument")
  } 
  if (!is.numeric(rs.thres) || length(rs.thres)!=1 || rs.thres < 0) {
    stop("Invalid rs.thres argument")
  }
  if (!is.character(rs.type) || length(rs.type)!=1) {
    stop("Invalid rs.type argument")
  }
  rs.type <- match.arg(rs.type, c("multinomial",
                                  "residual",
                                  "stratified",
                                  "systematic"))
  if (missing(smc.rng.seed)) {
    smc.rng.seed <- runif(1, 0, as.integer(Sys.time()));
  }
  if (!is.numeric(smc.rng.seed) || length(smc.rng.seed)!=1 || smc.rng.seed < 0) {
    stop("Invalid smc.rng.seed argument")
  }
  
  ## build smc sampler
  if (!.Call("is_sampler_built", object$ptr(), PACKAGE="RBiips")) {
    .Call("build_smc_sampler", object$ptr(), FALSE, PACKAGE="RBiips")
  }
  
  ## run smc sampler
  ok <- .Call("run_smc_sampler", object$ptr(), as.integer(n.part),
              as.integer(smc.rng.seed), rs.thres, rs.type, PACKAGE="RBiips")
  
  invisible(ok)
}




##' Generate conditional samples
##' 
##' Function to extract random weighted samples, aka. particles, from the
##' conditional distribution of the parameters of a \code{biips} model.
##' 
##' The \code{smc.samples} function creates monitors for the given types and
##' variables, runs the SMC algorithm for \code{n.part} particles and returns
##' the monitored samples.
##' 
##' Argument \code{type} specifies the type of particle values that will be
##' stored for each monitored node. Possible values are: \code{"filtering"},
##' \code{"smoothing"}, \code{"backward.smoothing"}
##' 
##' @param obj a biips model object
##' @param variable.names a character vector giving the names of variables to
##' be monitored
##' @param type a character vector giving the types of monitoring desired.
##' @param n.part number of particles
##' @param backward logical flag. If \code{TRUE} then a backward smoothing
##' algorithm will be run after the forward sampling step
##' @param rs.thres threshold on the ESS criterion to control the resampling
##' step
##' @param rs.type a string indicating the resampling algorithm used
##' @param ... additional arguments
##' @return A list of \code{\link[=particles.list.object]{particles.list}}
##' objects, with one element for each element of the \code{variable.names}
##' argument.
##' 
##' A \code{particles.list} object is a list of
##' \code{\link[=particles.object]{particles}} object, with one element for
##' each type of monitoring \code{type} argument.
##' @note By default, the seed of the random number generation is chosen
##' randomly. However for the sake of repatability, the optional
##' \code{smc.rng.seed} integer argument can be used to fix it.
##' @author Adrien Todeschini, Francois Caron
##' @seealso \code{\link{biips.model}}, \code{\link{pmmh.samples}},
##' \code{\link{pimh.samples}}
##' @keywords models
##' @export
##' @examples
##' 
##' ## Should be DIRECTLY executable !! 
##' ##-- ==>  Define data, use random,
##' ##--	or do  help(data=index)  for the standard data sets.
##' 
smc.samples <- function(object, variable.names, n.part, type="fs",
                        rs.thres = 0.5, rs.type = "stratified", ...)
{
  if (!is.character(type))
    stop("'type' must be a character string with characters 'f', 's', 'b'")
  
  type <- unlist(strsplit(type, NULL))
  type <- match.arg(type, c('f', 's', 'b'), several.ok=TRUE)
  backward <- ('b' %in% type)
  
  ## monitor
  if(backward) {
    .Call("set_default_monitors", object$ptr(), PACKAGE="RBiips")
  }
  if (!missing(variable.names))
    monitor.biips(object, variable.names, type)
  
  ## smc forward sampler
  run.smc.forward(object, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type, ...)
  
  log.marg.like <- .Call("get_log_norm_const", object$ptr(), PACKAGE="RBiips")
  ans <- list()
  
  mon <- .Call("get_filter_monitors", object$ptr(), PACKAGE="RBiips")
  for (n in names(mon)) {
    ans[[n]][["filtering"]] <- mon[[n]]
  }
  if (!backward) {
    clear.monitors.biips(object, type="f")
  }
  
  mon <- .Call("get_gen_tree_smooth_monitors", object$ptr(), PACKAGE="RBiips")
  for (n in names(mon)) {
    ans[[n]][["smoothing"]] <- mon[[n]]
  }
  clear.monitors.biips(object, type="s")
  
  ## smc backward smoother
  if (backward)
  {
    ## run backward smoother
    .Call("run_backward_smoother", object$ptr(), PACKAGE="RBiips")
    
    clear.monitors.biips(object, type="f")
    
    mon <- .Call("get_backward_smooth_monitors", object$ptr(), PACKAGE="RBiips")
    for (n in names(mon)) {
      ans[[n]][["backward.smoothing"]] <- mon[[n]]
    }
    clear.monitors.biips(object, type="b")
  }
  
  for (n in names(ans)) {
    class(ans[[n]]) <- "particles.list"
  }
  ans[["log.marg.like"]] <- log.marg.like
  
  return(ans)
}
