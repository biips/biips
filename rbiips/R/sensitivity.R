##' Sensitivity analysis
##' 
##' %% ~~ A concise (1-5 lines) description of what the function does. ~~
##' 
##' %% ~~ If necessary, more details than the description above ~~
##' 
##' @param model a biips model object
##' @param params
##' @param n.part number of particles
##' @param ... additional arguments to be passed to the SMC algorithm
##' @return %% ~Describe the value returned
##'  %% If it is a LIST, use
##'  %% \item{comp1 }{Description of 'comp1'}
##' %% \item{comp2 }{Description of 'comp2'}
##' %% ...
##' @author Adrien Todeschini, Francois Caron
##' @keywords models
##' @export
##' @examples
##' 
##' ## Should be DIRECTLY executable !!
##' ##-- ==>  Define data, use random,
##' ##--	or do  help(data=index)  for the standard data sets.
##' 
smc.sensitivity <- function(object, params,
                            n.part, ...)
{
  if (!is.biips(object))
    stop("Invalid BiiPS model")
  
  if (missing(n.part))
    stop("missing n.part argument")
  
  ## check params
  if (!is.list(params) || length(params) == 0)
    stop("Invalid params argument")
  variable.names <- names(params)
  pn <- parse.varnames(variable.names)
  
  ## get parameters array length
  dim <- list()
  len <- list()
  d <- dim(params[[1]])
  if (is.null(d)) {
    n.params <- length(params[[1]])
    dim[[names(params)[1]]] <- 1
    len[[names(params)[1]]] <- 1
  } else {
    n.params <- d[[length(d)]]
    d <- d[1:(length(d)-1)]
    dim[[names(params)[1]]] <- d
    len[[names(params)[1]]] <- prod(d)
  }
  if (length(params) > 1) {
    for (i in 2:length(params)) {
      d <- dim(params[[i]])
      if (is.null(d)) {
        n <- length(params[[i]])
        dim[[names(params)[i]]] <- 1
        len[[names(params)[i]]] <- 1
      } else {
        n <- d[[length(d)]]
        d <- d[1:(length(d)-1)]
        dim[[names(params)[i]]] <- d
        len[[names(params)[i]]] <- prod(d)
      }
      # check parameters array length
      if (n != n.params)
        stop("Invalid params argument: different number of parameters.")
    }
  }
  
  ## stop biips verbosity
  verb <- .Call("verbosity", 0, PACKAGE="RBiips")
  on.exit(.Call("verbosity", verb, PACKAGE="RBiips"))
  
  ## initialize
  ##-----------
  log.marg.like <- vector(length=n.params)
  log.marg.like.pen <- vector(length=n.params)
  max.log.marg.like <- -Inf
  max.log.marg.like.pen <- -Inf
  
  .Call("message", paste("Analyzing sensitivity with", n.part, "particles"), PACKAGE="RBiips")
  ## progress bar
  bar <- .Call("progress_bar", n.params, '*', "iterations", PACKAGE="RBiips")
  
  ## Iterate 
  ##--------
  for (k in 1:n.params) {
    
    ## make one param
    param <- list()
    for (v in seq(along=variable.names)) {
      var <- variable.names[[v]]
      ind <- seq(from=(k-1)*len[[v]]+1, to=k*len[[v]])
      param[[var]] <- params[[v]][ind]
      dim(param[[var]]) <- dim[[v]]
      
      ## change param value
      if(!.Call("change_data", object$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]],
                param[[v]], FALSE, PACKAGE="RBiips"))
        stop("data change failed: invalid parameter. variable = ", var,". value = ", param[v])
    }
    
    log.prior <- 0
    for (v in seq(along=variable.names)) {
      log.p <- .Call("get_log_prior_density", object$ptr(), 
                     pn$names[[v]], pn$lower[[v]], pn$upper[[v]], PACKAGE="RBiips")
      
      if (is.na(log.p)) {
        next
      }
      
      if (is.nan(log.p) || (is.infinite(log.p) && log.p<0))
        stop("Failure evaluating parameter log prior for variable ",
             variable.names[[v]], ". value = ", param[v])
      
      log.prior <- log.prior + log.p
    }
    
    ## run smc sampler
    ok <- run.smc.forward(object, n.part=n.part, ...)
    
    if (!ok)
      stop("Failure running smc forward sampler. param: ", paste(names(param),"=", param, sep="", collapse=";"))
    
    ## log marginal likelihood
    log.marg.like[k] <- .Call("get_log_norm_const", object$ptr(), PACKAGE="RBiips")
    if (log.marg.like[k] > max.log.marg.like) {
      max.log.marg.like <- log.marg.like[k]
      max.param <- param
    }
    log.marg.like.pen[k] <- log.marg.like[k] + log.prior
    if (log.marg.like.pen[k] > max.log.marg.like.pen) {
      max.log.marg.like.pen <- log.marg.like.pen[k]
      max.param.pen <- param
    }
    
    ## advance progress bar
    .Call("advance_progress_bar", bar, 1, PACKAGE="RBiips")
  }
  
  ## restore data
  ##-------------
  object$recompile()
  
  # FIXME: Tentative of removing data without recompiling the model
  #   for (v in seq(along=variable.names)) {
  #     if (!(pn$names[[v]] %in% names(data))) {
  #       ok <- .Call("remove_data", object$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]], PACKAGE="RBiips")
  #       if (!ok)
  #         stop("Failure restoring data")
  #       next
  #     }
  #     
  #     if (is.null(pn$lower[[v]])) {
  #       data.sub <- data[[v]]
  #     } else {
  #       ## compute offsets
  #       offsets <- NULL
  #       dim <- dim(data[[v]])
  #       if (is.null(dim))
  #         dim <- length(data[[v]])
  #       ind <- vector(len=length(dim))
  #       
  #       for (i in seq(along=data[[v]])) {
  #         r <- i
  #         for (d in seq(along=dim)) {
  #           ind[d] <- ((r-1) %% dim[d]) +1
  #           r <- ceiling(r/dim[d])
  #         }
  #         print(ind)
  #         if (any(ind<pn$lower[[v]]) || any(ind>pn$upper[[v]]))
  #           next
  #         offsets <- c(offsets, i)
  #       }
  #       
  #       data.sub <- array(data[[v]][offsets], dim=pn$upper[[v]]-pn$lower[[v]]+1)
  #     }
  #     
  #     if (all(is.na(data.sub))) {
  #       ok <- .Call("remove_data", object$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]], PACKAGE="RBiips")
  #       if (!ok)
  #         stop("Failure restoring data")
  #       next
  #     }
  #     
  #     if (any(is.na(data.sub)))
  #       stop("Failure restoring data")
  #     
  #     ok <- .Call("change_data", object$ptr(), pn$names[[v]], pn$lower[[v]], pn$upper[[v]],
  #                 data.sub, FALSE, PACKAGE="RBiips")
  #     if (!ok)
  #       stop("Failure restoring data")
  #   }
  
  ## output
  ##--------
  ans <- list(log.marg.like=log.marg.like, max.param=max.param, max.log.marg.like=max.log.marg.like,
              log.marg.like.pen=log.marg.like.pen, max.param.pen=max.param.pen, max.log.marg.like.pen=max.log.marg.like.pen)
  
  return(ans)
}
