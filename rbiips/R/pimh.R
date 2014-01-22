init.pimh.biips <- function(object, variable.names,
                            n.part, rs.thres=0.5, rs.type="stratified")
{
  monitored <- is.monitored.biips(object, variable.names, "s", FALSE)
  if (!monitored) {
    ## monitor variables
    monitor.biips(object, variable.names, type="s")
  }
  
  if (!.Call("is_sampler_built", object$ptr(), PACKAGE="RBiips")) {
    ## build smc sampler
    .Call("build_smc_sampler", object$ptr(), FALSE, PACKAGE="RBiips")
  }
  
  atend <- .Call("is_smc_sampler_at_end", object$ptr(), PACKAGE="RBiips")
  ## get log normalizing constant
  if (!monitored || !atend) {
    ## run smc sampler
    .Call("message", "Initializing PIMH", PACKAGE="RBiips")
    run.smc.forward(object, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type)
  }
  log.marg.like <- .Call("get_log_norm_const", object$ptr(), PACKAGE="RBiips")
  
  ## get sampled value
  sampled.value <- .Call("get_sampled_gen_tree_smooth_particle", object$ptr(), PACKAGE="RBiips")
  if (length(sampled.value)==0) {
    ## sample one particle
    rng.seed <- runif(1, 0, as.integer(Sys.time()))
    .Call("sample_gen_tree_smooth_particle", object$ptr(), as.integer(rng.seed), PACKAGE="RBiips")
    
    sampled.value <- .Call("get_sampled_gen_tree_smooth_particle", object$ptr(), PACKAGE="RBiips")
  }
  sample <- list()
  for (var in variable.names) {
    sample[[var]] <- sampled.value[[var]]
  }
  
  ans <- list(sample=sample, log.marg.like=log.marg.like)
  invisible(ans)
}


one.update.pimh.biips <- function(object, variable.names,
                                  n.part, rs.thres=0.5, rs.type="stratified",
                                  sample, log.marg.like)
{
  ## SMC
  run.smc.forward(object, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type)
  
  ## Acceptance rate
  log.marg.like.prop <- .Call("get_log_norm_const", object$ptr(), PACKAGE="RBiips")
  log.ar <- log.marg.like.prop - log.marg.like
  
  ## Accept/Reject step
  accepted <- (runif(1) < exp(log.ar))
  if (accepted) {
    log.marg.like <- log.marg.like.prop
    
    ## sample one particle
    rng.seed <- runif(1, 0, as.integer(Sys.time()))
    .Call("sample_gen_tree_smooth_particle", object$ptr(), as.integer(rng.seed), PACKAGE="RBiips")
    
    ## get sampled value
    sampled.value <- .Call("get_sampled_gen_tree_smooth_particle", object$ptr(), PACKAGE="RBiips")
    for (var in variable.names) {
      sample[[var]] <- sampled.value[[var]]
    }
  }
  
  ans <- list(sample=sample, log.marg.like=log.marg.like, accepted=accepted)
  invisible(ans)
}


##' @export
update.pimh <- function(object, ...)
  UseMethod("update.pimh")


##' @S3method update.pimh biips
update.pimh.biips <- function(object, variable.names, n.iter,
                              n.part, ...)
{
  if (!is.biips(object))
    stop("Invalid BiiPS model")
  
  ## check variable.names
  if (!is.character(variable.names))
    stop("Invalid variable.names argument")
  parse.varnames(variable.names)
  
  if (!is.numeric(n.iter) || length(n.iter)!=1 || n.iter < 1)
    stop("Invalid n.iter argument")
  n.iter <- as.integer(n.iter)
  
  ## stop biips verbosity
  verb <- .Call("verbosity", 0, PACKAGE="RBiips")
  on.exit(.Call("verbosity", verb, PACKAGE="RBiips"))
  
  ## initialize
  out <- init.pimh.biips(object, variable.names=variable.names,
                         n.part=n.part, ...)
  sample <- out$sample
  log.marg.like <- out$log.marg.like
  
  .Call("message", paste("Updating PIMH with", n.part, "particles"), PACKAGE="RBiips")
  ## progress bar
  bar <- .Call("progress_bar", n.iter, '*', "iterations", PACKAGE="RBiips")
  
  n.samples <- 0
  
  ## Independant Metropolis-Hastings iterations
  ##-------------------------------------------
  for(i in 1:n.iter) {
    out <- one.update.pimh.biips(object, variable.names=variable.names, 
                                 sample=sample, log.marg.like=log.marg.like,
                                 n.part=n.part, ...)
    sample <- out$sample
    log.marg.like <- out$log.marg.like
    accepted <- out$accepted
    
    ## advance progress bar
    .Call("advance_progress_bar", bar, 1, PACKAGE="RBiips")
  }
  
  ## release monitors memory
  clear.monitors.biips(object, type="s", TRUE)
  
  ## reset log norm const and sampled value
  if (n.iter > 0 && !accepted) {
    .Call("set_log_norm_const", object$ptr(), log.marg.like, PACKAGE="RBiips")
    .Call("set_sampled_gen_tree_smooth_particle", object$ptr(), sample, PACKAGE="RBiips")
  }
  
  invisible(NULL)
}




##' Generate Particle Independent Metropolis-Hastings samples
##' 
##' The \code{pimh.samples} function creates monitors for the given variables,
##' runs the model for \code{n.iter} iterations and returns the monitored
##' samples.
##' 
##' @param model a biips model object
##' @param variable.names a character vector giving the names of variables to
##' be monitored
##' @param n.iter number of iterations of the Markov chain to run
##' @param thin thinning interval for monitors
##' @param n.part number of particles
##' @param ... additional arguments to be passed to the SMC algorithm
##' @return A list of \code{\link[rjags:mcarray.object]{mcarray}}
##' objects, with one element for each element of the \code{variable.names}
##' argument.
##' @author Adrien Todeschini, Francois Caron
##' @seealso \code{\link{biips.model}}, \code{\link{pmmh.samples}},
##' \code{\link{smc.samples}}
##' @keywords models
##' @export
##' @examples
##' 
##' ## Should be DIRECTLY executable !! 
##' ##-- ==>  Define data, use random,
##' ##--	or do  help(data=index)  for the standard data sets.
##' 
pimh.samples <- function(object, variable.names, n.iter, thin = 1,
                         n.part, ...)
{
  if (!is.biips(object))
    stop("Invalid BiiPS model")
  
  ## check variable.names
  if (!is.character(variable.names))
    stop("Invalid variable.names argument")
  parse.varnames(variable.names)
  
  if (!is.numeric(n.iter) || length(n.iter)!=1 || n.iter < 1)
    stop("Invalid n.iter argument")
  n.iter <- as.integer(n.iter)
  if (!is.numeric(thin) || length(thin)!=1 || thin < 1)
    stop("Invalid thin argument")
  thin <- as.integer(thin)
  
  ## stop biips verbosity
  verb <- .Call("verbosity", 0, PACKAGE="RBiips")
  on.exit(.Call("verbosity", verb, PACKAGE="RBiips"))
  
  ## Initialization
  ##---------------  
  out <- init.pimh.biips(object, variable.names=variable.names,
                         n.part=n.part, ...)
  sample <- out$sample
  log.marg.like <- out$log.marg.like
  
  ans <- list()
  n.samples <- 0
  
  .Call("message", paste("Generating PIMH samples with", n.part, "particles"), PACKAGE="RBiips")
  ## progress bar
  bar <- .Call("progress_bar", n.iter, '*', "iterations", PACKAGE="RBiips")
  
  ## Independant Metropolis-Hastings iterations
  ##-------------------------------------------
  for(i in 1:n.iter) {
    out <- one.update.pimh.biips(object, variable.names=variable.names,
                                 n.part=n.part,
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
    .Call("advance_progress_bar", bar, 1, PACKAGE="RBiips")
  }
  
  clear.monitors.biips(object, type="s")
  
  ## reset log norm const and sampled value
  if (n.iter > 0 && !accepted) {
    .Call("set_log_norm_const", object$ptr(), log.marg.like, PACKAGE="RBiips")
    .Call("set_sampled_gen_tree_smooth_particle", object$ptr(), sample, PACKAGE="RBiips")
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
