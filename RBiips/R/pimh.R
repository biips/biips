pimh.init.biips <- function(object, variable.names,
                            n_part, rs_thres=0.5, rs_type="stratified")
{
  monitored <- is.monitored.biips(object, variable.names, "s", FALSE)
  if (!monitored) {
    ## monitor variables
    monitor.biips(object, variable.names, type="s")
  }
  
  if (!RBiips("is_sampler_built",  object$ptr())) {
    ## build smc sampler
    RBiips("build_smc_sampler",  object$ptr(), FALSE)
  }
  
  atend <- RBiips("is_smc_sampler_at_end",  object$ptr())
  ## get log normalizing constant
  if (!monitored || !atend) {
    ## run smc sampler
    RBiips("message",  "Initializing PIMH")
    run_smc_forward(object, n_part=n.part, rs_thres=rs.thres, rs_type=rs.type)
  }
  log.marg.like <- RBiips("get_log_norm_const",  object$ptr())
  
  ## get sampled value
  sampled.value <- RBiips("get_sampled_gen_tree_smooth_particle",  object$ptr())
  if (length(sampled.value)==0) {
    ## sample one particle
    rng.seed <- runif(1, 0, as.integer(Sys.time()))
    RBiips("sample_gen_tree_smooth_particle",  object$ptr(), as.integer(rng.seed))
    
    sampled.value <- RBiips("get_sampled_gen_tree_smooth_particle",  object$ptr())
  }
  sample <- list()
  for (var in variable.names) {
    sample[[var]] <- sampled.value[[var]]
  }
  
  ans <- list(sample=sample, log.marg.like=log.marg.like)
  invisible(ans)
}


one.pimh.update.biips <- function(object, variable.names,
                                  n_part, rs_thres=0.5, rs_type="stratified",
                                  sample, log.marg.like)
{
  ## SMC
  run_smc_forward(object, n_part=n.part, rs_thres=rs.thres, rs_type=rs.type)
  
  ## Acceptance rate
  log.marg.like.prop <- RBiips("get_log_norm_const",  object$ptr())
  log.ar <- log.marg.like.prop - log.marg.like
  
  ## Accept/Reject step
  accepted <- (runif(1) < exp(log.ar))
  if (accepted) {
    log.marg.like <- log.marg.like.prop
    
    ## sample one particle
    rng.seed <- runif(1, 0, as.integer(Sys.time()))
    RBiips("sample_gen_tree_smooth_particle",  object$ptr(), as.integer(rng.seed))
    
    ## get sampled value
    sampled.value <- RBiips("get_sampled_gen_tree_smooth_particle",  object$ptr())
    for (var in variable.names) {
      sample[[var]] <- sampled.value[[var]]
    }
  }
  
  ans <- list(sample=sample, log.marg.like=log.marg.like, accepted=accepted)
  invisible(ans)
}


##' @export
pimh.update <- function(object, ...)
  UseMethod("pimh.update")


##' @S3method pimh.update biips
pimh.update.biips <- function(object, variable.names, n_iter,
                              n_part, ...)
{
  if (!is.biips(object))
    stop("Invalid BiiPS model")
  
  ## check variable.names
  if (!is.character(variable.names))
    stop("Invalid variable.names argument")
  parse.varnames(variable.names)
  
  if (!is.numeric(n_iter) || length(n_iter)!=1 || n_iter < 1)
    stop("Invalid n_iter argument")
  n_iter <- as.integer(n_iter)
  
  ## stop biips verbosity
  verb <- RBiips("verbosity",  0)
  on.exit(RBiips("verbosity",  verb))
  
  ## initialize
  out <- pimh.init.biips(object, variable.names=variable.names,
                         n_part=n.part, ...)
  sample <- out$sample
  log.marg.like <- out$log.marg.like
  
  RBiips("message",  paste("Updating PIMH with", n_part, "particles"))
  ## progress bar
  bar <- RBiips("progress_bar",  n_iter, '*', "iterations")
  
  n.samples <- 0
  
  ## Independant Metropolis-Hastings iterations
  ##-------------------------------------------
  for(i in 1:n_iter) {
    out <- one.pimh.update.biips(object, variable.names=variable.names, 
                                 sample=sample, log.marg.like=log.marg.like,
                                 n_part=n.part, ...)
    sample <- out$sample
    log.marg.like <- out$log.marg.like
    accepted <- out$accepted
    
    ## advance progress bar
    RBiips("advance_progress_bar",  bar, 1)
  }
  
  ## release monitors memory
  clear.monitors.biips(object, type="s", TRUE)
  
  ## reset log norm const and sampled value
  if (n_iter > 0 && !accepted) {
    RBiips("set_log_norm_const",  object$ptr(), log.marg.like)
    RBiips("set_sampled_gen_tree_smooth_particle",  object$ptr(), sample)
  }
  
  invisible(NULL)
}




##' Generate Particle Independent Metropolis-Hastings samples
##' 
##' The \code{pimh.samples} function creates monitors for the given variables,
##' runs the model for \code{n_iter} iterations and returns the monitored
##' samples.
##' 
##' @param model a biips model object
##' @param variable.names a character vector giving the names of variables to
##' be monitored
##' @param n_iter number of iterations of the Markov chain to run
##' @param thin thinning interval for monitors
##' @param n_part number of particles
##' @param ... additional arguments to be passed to the SMC algorithm
##' @return A list of \code{\link[rjags:mcarray.object]{mcarray}}
##' objects, with one element for each element of the \code{variable.names}
##' argument.
##' @author Adrien Todeschini, Francois Caron
##' @seealso \code{\link{biips_model}}, \code{\link{pmmh.samples}},
##' \code{\link{smc_samples}}
##' @keywords models
##' @export
##' @examples
##' 
##' ## Should be DIRECTLY executable !! 
##' ##-- ==>  Define data, use random,
##' ##--	or do  help(data=index)  for the standard data sets.
##' 
pimh.samples <- function(object, variable.names, n_iter, thin = 1,
                         n_part, ...)
{
  if (!is.biips(object))
    stop("Invalid BiiPS model")
  
  ## check variable.names
  if (!is.character(variable.names))
    stop("Invalid variable.names argument")
  parse.varnames(variable.names)
  
  if (!is.numeric(n_iter) || length(n_iter)!=1 || n_iter < 1)
    stop("Invalid n_iter argument")
  n_iter <- as.integer(n_iter)
  if (!is.numeric(thin) || length(thin)!=1 || thin < 1)
    stop("Invalid thin argument")
  thin <- as.integer(thin)
  
  ## stop biips verbosity
  verb <- RBiips("verbosity",  0)
  on.exit(RBiips("verbosity",  verb))
  
  ## Initialization
  ##---------------  
  out <- pimh.init.biips(object, variable.names=variable.names,
                         n_part=n.part, ...)
  sample <- out$sample
  log.marg.like <- out$log.marg.like
  
  ans <- list()
  n.samples <- 0
  
  RBiips("message",  paste("Generating PIMH samples with", n_part, "particles"))
  ## progress bar
  bar <- RBiips("progress_bar",  n_iter, '*', "iterations")
  
  ## Independant Metropolis-Hastings iterations
  ##-------------------------------------------
  for(i in 1:n_iter) {
    out <- one.pimh.update.biips(object, variable.names=variable.names,
                                 n_part=n.part,
                                 sample=sample, log.marg.like=log.marg.like, ...)
    sample <- out$sample
    log.marg.like <- out$log.marg.like
    accepted <- out$accepted
    
    ## Store output
    if ((i-1)%%thin == 0) {
      n.samples <- n.samples + 1
      ans[["log.marg.like"]][n.samples] <- log.marg.like
      for (var in variable.names)
        ans[[var]] <- c(ans[[var]], sample[[var]])
    }
    
    ## advance progress bar
    RBiips("advance_progress_bar",  bar, 1)
  }
  
  clear.monitors.biips(object, type="s")
  
  ## reset log norm const and sampled value
  if (n_iter > 0 && !accepted) {
    RBiips("set_log_norm_const",  object$ptr(), log.marg.like)
    RBiips("set_sampled_gen_tree_smooth_particle",  object$ptr(), sample)
  }
  
  
  ## set output dimensions
  for (var in variable.names) {
    n.dim <- length(dim(sample[[var]]))
    dim(ans[[var]]) <- c(dim(sample[[var]]), n.samples)
    names(dim(ans[[var]])) <- c(rep("", n.dim), "iteration")
    class(ans[[var]]) <- "mcarray"
  }
  dim(ans[["log.marg.like"]]) <- c(1, n.samples)
  names(dim(ans[["log.marg.like"]])) <- c("", "iteration")
  class(ans[["log.marg.like"]]) <- "mcarray"
  
  return(ans)
}
