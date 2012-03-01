monitor.biips <- function(obj, variable.names, type="smoothing")
{
  if (!is.biips(obj))
    stop("Invalid BiiPS model.")
  
  if (!is.character(variable.names) || length(variable.names) == 0)
    stop("variable.names must be a character vector")
    
  pn <- parse.varnames(variable.names)
  
  type <- match.arg(type, c("filtering", "smoothing", "backward.smoothing"), several.ok = TRUE)
  if ("filtering" %in% type) {
    .Call("set_filter_monitors", obj$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
  }
  if ("smoothing" %in% type) {
    .Call("set_smooth_tree_monitors", obj$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
  }
  if ("backward.smoothing" %in% type) {
    .Call("set_smooth_monitors", obj$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
  }
  invisible(NULL)
}


clear.monitors.biips <- function(obj, type="smoothing")
{
  if (!is.biips(obj))
    stop("Invalid BiiPS model.")
  
  type <- match.arg(type, c("filtering", "smoothing", "backward.smoothing"), several.ok = TRUE)
  if ("filtering" %in% type) {
    .Call("clear_filter_monitors", obj$ptr(), PACKAGE="RBiips")
  }
  if ("smoothing" %in% type) {
    .Call("clear_smooth_tree_monitors", obj$ptr(), PACKAGE="RBiips")
  }
  if ("backward.smoothing" %in% type) {
    .Call("clear_smooth_monitors", obj$ptr(), PACKAGE="RBiips")
  }
  invisible(NULL)
}


release.monitors.biips <- function(obj, type="smoothing")
{
  if (!is.biips(obj))
    stop("Invalid BiiPS model.")
  
  type <- match.arg(type, c("filtering", "smoothing", "backward.smoothing"), several.ok = TRUE)
  if ("filtering" %in% type) {
    .Call("release_filter_monitors", obj$ptr(), PACKAGE="RBiips")
  }
  if ("smoothing" %in% type) {
    .Call("release_smooth_tree_monitors", obj$ptr(), PACKAGE="RBiips")
  }
  if ("backward.smoothing" %in% type) {
    .Call("release_smooth_monitors", obj$ptr(), PACKAGE="RBiips")
  }
  invisible(NULL)
}


build.sampler <- function(x, ...)
  UseMethod("build.sampler")


build.sampler.biips <- function(obj, proposal= "auto")
{
  if (!is.biips(obj))
    stop("Invalid BiiPS model")
      
  if (!is.character(proposal) || !is.atomic(proposal)) {
    stop("Invalid proposal argument")
  }
  proposal <- match.arg(proposal, c("auto",
                                  "prior"))
    
  ## build smc sampler
  .Call("build_smc_sampler", obj$ptr(), proposal=="prior", PACKAGE="RBiips")
  
  invisible(NULL)
}


run.smc.forward <- function(obj, n.part,
                      rs.thres = 0.5, rs.type = "stratified",
                      smc.rng.seed)
{
  if (!is.biips(obj))
    stop("Invalid BiiPS model")
      
  if (!is.numeric(n.part) || !is.atomic(n.part) || n.part < 1) {
    stop("Invalid n.part argument")
  } 
  if (!is.numeric(rs.thres) || !is.atomic(rs.thres) || rs.thres < 0) {
    stop("Invalid rs.thres argument")
  }
  if (!is.character(rs.type) || !is.atomic(rs.type)) {
    stop("Invalid rs.type argument")
  }
  rs.type <- match.arg(rs.type, c("multinomial",
                                  "residual",
                                  "stratified",
                                  "systematic"))
  if (missing(smc.rng.seed)) {
    smc.rng.seed <- runif(1, 0, as.integer(Sys.time()));
  }
  if (!is.numeric(smc.rng.seed) || !is.atomic(smc.rng.seed) || smc.rng.seed < 0) {
    stop("Invalid smc.rng.seed argument")
  }
  
  ## build smc sampler
  if (!.Call("is_sampler_built", obj$ptr(), PACKAGE="RBiips")) {
    .Call("build_smc_sampler", obj$ptr(), FALSE, PACKAGE="RBiips")
  }
  
  ## run smc sampler
  ok <- .Call("run_smc_sampler", obj$ptr(), as.integer(n.part), as.integer(smc.rng.seed), rs.thres, rs.type, PACKAGE="RBiips")
  
  invisible(ok)
}


init.pmmh.biips <- function(obj, variable.names, pn,
                            n.part, rs.thres=0.5, rs.type="stratified")
{  
  
  sample <- list()
  for (var in variable.names) {
    sample[[var]] <- obj$data()[[var]]
  }
  
  log.prior <- list()
  for (v in seq(along=variable.names))
  {
    log.p <- .Call("get_log_prior_density", obj$ptr(),
                   pn$names[[v]], pn$lower[[v]], pn$upper[[v]], PACKAGE="RBiips")
    if (is.infinite(log.p) && log.p<0) 
      stop("get log prior density: bad initial values.")
    
    log.prior[[variable.names[[v]]]] <- log.p
  }
  
  ## build smc sampler
  if (!.Call("is_sampler_built", obj$ptr(), PACKAGE="RBiips")) {
    .Call("build_smc_sampler", obj$ptr(), FALSE, PACKAGE="RBiips")
  }
  
  ## get log normalizing constant
  if (!.Call("is_smc_sampler_at_end", obj$ptr(), PACKAGE="RBiips")) {
    ## run smc
    if (!run.smc.forward(obj, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type))
      stop("run smc forward sampler: bad initial values.")
  }
  log.norm.const <- .Call("get_log_norm_const", obj$ptr(), PACKAGE="RBiips")
  
  ans <- list(sample=sample, log.prior=log.prior, log.norm.const=log.norm.const)
  return(ans)
}


one.update.pmmh.biips <- function(obj, variable.names, pn,
                                  rw.sd, n.part, rs.thres=0.5, rs.type="stratified",
                                  sample, log.prior, log.norm.const)
{
  n.fail <- 0
  
  ## Metropolis-Hastings iteration
  ##------------------------------
  for (v in seq(along=variable.names)) {
    var <- variable.names[[v]]
    ## Random walk proposal
    prop <- list()
    prop[[var]] <- rnorm(1, sample[[var]], rw.sd[[var]])
    dim(prop[[var]]) <- dim(sample[[var]])
    
    ## change model data
    ok <- .Call("change_data", obj$ptr(), prop[var], PACKAGE="RBiips")
    
    if (!ok) {
      accepted <- FALSE
      ## reset previous value
      prop[[var]] <- sample[[var]]
      if(!.Call("change_data", obj$ptr(), sample[var], PACKAGE="RBiips"))
        stop("proposed data change failed: Can not reset previous data.")
      n.fail <- n.fail+1
      next
    }
    
    log.prior.prop <- .Call("get_log_prior_density", obj$ptr(),
                            pn$names[[v]], pn$lower[[v]], pn$upper[[v]], PACKAGE="RBiips")
    
    if (is.infinite(log.prior.prop) && log.prior.prop<0) {
      accepted <- FALSE
      ## reset previous value
      if(!.Call("change_data", obj$ptr(), sample[var], PACKAGE="RBiips"))
        stop("get log prior failed: Can not reset previous data.")
      next
    }
   
    ## run smc sampler
    ok <- run.smc.forward(obj, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type)
    
    if (!ok) {
      accepted <- FALSE
      ## reset previous value
      if(!.Call("change_data", obj$ptr(), sample[var], PACKAGE="RBiips"))
        stop("run smc forward sampler failed: Can not reset previous data.")
      n.fail <- n.fail+1
      next
    }
    
    ## Acceptance rate
    log.norm.const.prop <- .Call("get_log_norm_const", obj$ptr(), PACKAGE="RBiips")
    log.ar <- log.norm.const.prop - log.norm.const + log.prior.prop - log.prior[[var]]
  
    ## Accept/Reject step
    accepted <- (runif(1) < exp(log.ar))
    if (accepted) {
      sample[[var]] <- prop[[var]]
      log.prior[[var]] <- log.prior.prop
      log.norm.const <- log.norm.const.prop
    } else {
      ## reset previous value
      if(!.Call("change_data", obj$ptr(), sample[var], PACKAGE="RBiips"))
        stop("rejected proposed value: Can not reset previous data.")
    }
  }
  
  ans <- list(sample=sample, log.prior=log.prior, log.norm.const=log.norm.const, accepted=accepted, n.fail=n.fail)
  return(ans)
}


update.pmmh <- function(x, ...)
  UseMethod("update.pmmh")


update.pmmh.biips <- function(obj, variable.names, n.iter,
                       rw.sd, n.part, rs.thres=0.5, rs.type="stratified", max.fail = 0)
{
  if (!is.biips(obj))
    stop("Invalid BiiPS model")
  
  ## check variable.names
  if (!is.character(variable.names))
    stop("Invalid variable.names argument")
  pn <- parse.varnames(variable.names)
  
  if (!is.numeric(n.iter) || !is.atomic(n.iter) || n.iter < 1)
    stop("Invalid n.iter argument")
  n.iter <- as.integer(n.iter)
  
  if (!is.list(rw.sd))
    stop("Invalid rw.sd argument")
  for (var in variable.names) {
    if (!var %in% names(rw.sd))
      stop(paste("Missing rw.sd value for variable", var))
  }
  
  ## stop biips verbosity
  set.biips.verbosity(0)
  
  ## restart biips verbosity on exit
  on.exit(set.biips.verbosity(1))
  
  ## initialize
  out <- init.pmmh.biips(obj, variable.names=variable.names, pn=pn,
                         n.part=n.part, rs.thres=rs.thres, rs.type=rs.type)
  sample <- out$sample
  log.prior <- out$log.prior
  log.norm.const <- out$log.norm.const
  
  ## reset data to sample on exit
  on.exit(
      if (n.iter > 0 && !accepted) {
        .Call("set_log_norm_const", obj$ptr(), log.norm.const, PACKAGE="RBiips")
      }, add=TRUE)
  
  ## progress bar
  bar <- .Call("progress_bar", n.iter, '*', "iterations", PACKAGE="RBiips")
  
  n.fail <- 0
  accepted <- TRUE
  
  ## Metropolis-Hastings iterations
  ##-------------------------------
  for(i in 1:n.iter) {
    out <- one.update.pmmh.biips(obj, variable.names=variable.names, pn=pn,
                                  rw.sd=rw.sd, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type,
                                  sample=sample, log.prior=log.prior, log.norm.const=log.norm.const)
    sample <- out$sample
    log.prior <- out$log.prior
    log.norm.const <- out$log.norm.const
    accepted <- out$accepted
    n.fail <- n.fail + out$n.fail
    
    ## advance progress bar
    .Call("advance_progress_bar", bar, 1, PACKAGE="RBiips")
    
    if (n.fail > max.fail) {
      stop(paste("Number of failures exceeds max.fail:", n.fail, "failures."))
    }
  }
    
  invisible(NULL)
}


init.pimh.biips <- function(obj, variable.names,
                            n.part, rs.thres=0.5, rs.type="stratified")
{
  ## monitor variables
  monitor.biips(obj, variable.names, type="smoothing")
  
  ## build smc sampler
  if (!.Call("is_sampler_built", obj$ptr(), PACKAGE="RBiips")) {
    .Call("build_smc_sampler", obj$ptr(), FALSE, PACKAGE="RBiips")
  }
  
  ## get log normalizing constant
  if (!.Call("is_smc_sampler_at_end", obj$ptr(), PACKAGE="RBiips")) {
    ## run smc sampler
    run.smc.forward(obj, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type)
    
    ## sample one particle
    rng.seed <- runif(1, 0, as.integer(Sys.time()))
    .Call("sample_smooth_tree_particle", obj$ptr(), as.integer(rng.seed), PACKAGE="RBiips")
  }
  log.norm.const <- .Call("get_log_norm_const", obj$ptr(), PACKAGE="RBiips")
  
  ## get sampled value
  sampled.value <- .Call("get_sampled_smooth_tree_particle", obj$ptr(), PACKAGE="RBiips")
  sample <- list()
  for (var in variable.names) {
    sample[[var]] <- sampled.value[[var]]
  }
  
  ans <- list(sample=sample, log.norm.const=log.norm.const)
  return(ans)
}


one.update.pimh.biips <- function(obj, variable.names,
                                  n.part, rs.thres=0.5, rs.type="stratified",
                                  sample, log.norm.const)
{
  ## SMC
  run.smc.forward(obj, n.part=n.part, rs.thres=rs.thres, rs.type=rs.type)

  ## Acceptance rate
  log.norm.const.prop <- .Call("get_log_norm_const", obj$ptr(), PACKAGE="RBiips")
  log.ar <- log.norm.const.prop - log.norm.const

  ## Accept/Reject step
  accepted <- (runif(1) < exp(log.ar))
  if (accepted) {
    log.norm.const <- log.norm.const.prop
    
    ## sample one particle
    rng.seed <- runif(1, 0, as.integer(Sys.time()))
    .Call("sample_smooth_tree_particle", obj$ptr(), as.integer(rng.seed), PACKAGE="RBiips")
    
    ## get sampled value
    sampled.value <- .Call("get_sampled_smooth_tree_particle", obj$ptr(), PACKAGE="RBiips")
    for (var in variable.names) {
      sample[[var]] <- sampled.value[[var]]
    }
  }
  
  ans <- list(sample=sample, log.norm.const=log.norm.const, accepted=accepted)
  return(ans)
}


update.pimh <- function(x, ...)
  UseMethod("update.pimh")


update.pimh.biips <- function(obj, variable.names, n.iter,
                       n.part, rs.thres=0.5, rs.type="stratified")
{
  if (!is.biips(obj))
    stop("Invalid BiiPS model")
  
  ## check variable.names
  if (!is.character(variable.names))
    stop("Invalid variable.names argument")
  parse.varnames(variable.names)
  
  if (!is.numeric(n.iter) || !is.atomic(n.iter) || n.iter < 1)
    stop("Invalid n.iter argument")
  n.iter <- as.integer(n.iter)
  
  ## stop biips verbosity
  set.biips.verbosity(0)
  
  ## initialize
  out <- init.pimh.biips(obj, variable.names=variable.names,
                         n.part=n.part, rs.thres=rs.thres, rs.type=rs.type)
  sample <- out$sample
  log.norm.const <- out$log.norm.const
  
  ## progress bar
  bar <- .Call("progress_bar", n.iter, '*', "iterations", PACKAGE="RBiips")
  
  n.samples <- 0
  
  ## Independant Metropolis-Hastings iterations
  ##-------------------------------------------
  for(i in 1:n.iter) {
    out <- one.update.pimh.biips(obj, variable.names=variable.names,
                                 n.part=n.part, rs.thres=rs.thres, rs.type=rs.type,
                                 sample=sample, log.norm.const=log.norm.const)
    sample <- out$sample
    log.norm.const <- out$log.norm.const
    accepted <- out$accepted
  
    ## advance progress bar
    .Call("advance_progress_bar", bar, 1, PACKAGE="RBiips")
  }
  
  clear.monitors.biips(obj, type="smoothing")
  
  ## reset log norm const and sampled value
  if (n.iter > 0 && !accepted) {
    .Call("set_log_norm_const", obj$ptr(), log.norm.const, PACKAGE="RBiips")
    .Call("set_sampled_smooth_tree_particle", obj$ptr(), sample, PACKAGE="RBiips")
  }
 
  ## start biips verbosity
  set.biips.verbosity(1)
  
  invisible(NULL)
}


variable.names.biips <- function(object, ...) {
    .Call("get_variable_names", object$ptr(), PACKAGE="RBiips")
}
