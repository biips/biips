##' @export
pimh_init <- function(object, variable_names, ...) {
  stopifnot(is.biips(object))
  stopifnot(is.character(variable_names), length(variable_names) > 0)
  ## TODO check variable_names
  
  RBiips("message", "Initializing PIMH")
  
  state <- list(sample = list(), log_marg_like = -Inf)
  
  obj_pimh <- list(model = function() {
    object
  }, variable_names = function() {
    variable_names
  }, sample = function(sample) {
    if (!missing(sample)) state$sample <<- sample
    invisible(state$sample)
  }, log_marg_like = function(log_marg_like) {
    if (!missing(log_marg_like)) state$log_marg_like <<- log_marg_like
    invisible(state$log_marg_like)
  })
  class(obj_pimh) <- "pimh"
  
  return(obj_pimh)
}

##' @export
is.pimh <- function(object) {
  return(class(object) == "pimh")
}


pimh_algo <- function(object, ...) UseMethod("pimh_algo")

##' @S3method pimh_algo pimh
pimh_algo.pimh <- function(object, n_iter, n_part, return_samples, thin = 1, ...) {
  ## check arguments
  stopifnot(is.pimh(object))
  stopifnot(is.numeric(n_iter), length(n_iter) == 1, n_iter >= 1, is.finite(n_iter))
  n_iter <- as.integer(n_iter)
  stopifnot(is.numeric(n_part), length(n_part) == 1, n_part >= 1, is.finite(n_part))
  n_part <- as.integer(n_part)
  stopifnot(is.logical(return_samples), length(return_samples) == 1)
  stopifnot(is.numeric(thin), length(thin) == 1, thin >= 1, thin <= n_iter)
  thin <- as.integer(thin)
  
  ## stop biips verbosity
  verb <- RBiips("verbosity", 0)
  on.exit(RBiips("verbosity", verb))
  
  ## Initialization --------------------------------
  
  ## monitor variables
  monitored <- is_monitored(object$model(), variable_names, "s", FALSE)
  if (!monitored) 
    monitor(object$model(), variable_names, type = "s")
  
  ## build smc sampler
  if (!RBiips("is_sampler_built", object$model()$ptr())) 
    RBiips("build_smc_sampler", object$model()$ptr(), FALSE)
  
  ## Get sample and log likelihood from PIMH object
  sample <- object$sample()
  log_marg_like <- object$log_marg_like()
  
  ## Output structure with MCMC samples
  n_samples <- ceil(n_iter/thin)
  ind_sample <- 0
  
  out <- list()
  out$log_marg_like <- mcarray(dim = c(1, n_samples))
  
  ## display message and progress bar
  mess <- if (return_samples) 
    "Generating PIMH samples with" else "Updating PIMH with"
  RBiips("message", paste(mess, n_part, "particles"))
  bar <- RBiips("progress_bar", n_iter, "*", "iterations")
  ### TODO: display expected time of run
  
  ## Independant Metropolis-Hastings iterations
  ## -------------------------------------------
  for (i in 1:n_iter) {
    ## SMC
    smc_forward_algo(object, n_part = n_part, ...)
    
    ## Acceptance rate
    log_marg_like_prop <- RBiips("get_log_norm_const", object$ptr())
    log_ar <- log_marg_like_prop - log_marg_like
    
    ## Accept/Reject step
    if (runif(1) < exp(log_ar)) {
      log_marg_like <- log_marg_like_prop
      
      ## sample one particle
      sampled_value <- RBiips("sample_gen_tree_smooth_particle", object$ptr(), 
        get_seed())
      for (var in variable_names) {
        var_in <- to_biips_vname(var)
        sample[[var]] <- sampled_value[[var_in]]
      }
    }
    
    ## Store output
    if ((i - 1)%%thin == 0) {
      ind_sample <- ind_sample + 1
      out$log_marg_like[ind_sample] <- log_marg_like
      
      if (return_samples) {
        if (ind_samples == 1) {
          ## pre-allocation here to be sure that sample is not empty
          for (var in variable_names) {
          dimen <- dim(sample[[var]])
          out[[var]] <- mcarray(dim = c(dimen, n_samples))
          }
        }
        
        for (var in variable_names) {
          len <- length(sample[[var]])
          from <- (ind_sample - 1) * len + 1
          to <- (ind_sample - 1) * len + len
          out[[var]][from:to] <- sample[[var]]
        }
      }
    }
    
    ## progress bar
    RBiips("advance_progress_bar", bar, 1)
  }
  
  ## Release monitor memory
  clear_monitors(object, type = "s", release_only = TRUE)
  
  ## Output PIMH object with current sample and log marginal likelihood
  object$sample(sample)
  object$log_marg_like(log_marg_like)
  
  ### TODO: Remove singleton dimensions for vectors? (cf matbiips)
  if (return_samples) 
    return(out) else return(invisible(out))
}



##' @S3method update pimh
update.pimh <- function(object, n_iter, n_part, ...) {
  pimh_algo(object, n_iter = n_iter, n_part = n_part, return_samples = FALSE, ...)
  return(invisible())
}


##' Generate Particle Independent Metropolis-Hastings samples
##' 
##' The \code{pimh_samples} function creates monitors for the given variables,
##' runs the model for \code{n_iter} iterations and returns the monitored
##' samples.
##' 
##' @param model a biips model object
##' @param variable_names a character vector giving the names of variables to
##' be monitored
##' @param n_iter number of iterations of the Markov chain to run
##' @param thin thinning interval for monitors
##' @param n_part number of particles
##' @param ... additional arguments to be passed to the SMC algorithm
##' @return A list of \code{\link[rjags:mcarray.object]{mcarray}}
##' objects, with one element for each element of the \code{variable_names}
##' argument.
##' @author Adrien Todeschini, Francois Caron
##' @seealso \code{\link{biips_model}}, \code{\link{pmmh.samples}},
##' \code{\link{smc_samples}}
##' @keywords models
##' @examples
##' 
##' ## Should be DIRECTLY executable !! 
##' ##-- ==>  Define data, use random,
##' ##--\tor do  help(data=index)  for the standard data sets.
##' 
pimh_samples <- function(object, n_iter, n_part, thin = 1, ...) {
  out <- pimh_algo(object, n_iter = n_iter, n_part = n_part, thin = thin, return_samples = TRUE, 
    ...)
  return(out)
} 
