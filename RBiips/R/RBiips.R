# COPY: Adapted from rjags module file: jags.R

set.verbosity <- function(level=1)
{
  .Call("set_verbosity", level, PACKAGE="RBiips")
	invisible(NULL)
}


load.biips.module <- function(name, quiet=FALSE)
{    
  if (!is.character(name) || !is.atomic(name))
    stop("invalid name")

  ok <- .Call("load_module", name, PACKAGE="RBiips")
  if (!ok) {
    stop("module", name, "not found\n", sep=" ")
  }
  else if (!quiet) {
    cat("module", name, "loaded\n", sep=" ")
  }
	invisible(NULL)
}


biips.model <- function(file, data=sys.frame(sys.parent()), sample.data=TRUE, data.rng.seed)
{
  if (missing(file)) {
    stop("Model file name missing")
  }
  if (!is.atomic(file)) {
    stop(paste("Only one model file allowed."))
  }
  if (!file.exists(file)) {
    stop(paste("Model file \"", file, "\" not found", sep=""))
  }
  
  p <- .Call("make_console", PACKAGE="RBiips")
  .Call("check_model", p, file, PACKAGE="RBiips")

  varnames <- .Call("get_variable_names", p, PACKAGE="RBiips")
  if (is.environment(data)) {
    ##Get a list of numeric objects from the supplied environment
    data <- mget(varnames, envir=data, mode="numeric",
                 ifnotfound=list(NULL))
    ##Strip null entries
    data <- data[!sapply(data, is.null)]
  }
  else if (is.list(data)) {
    v <- names(data)
    if (is.null(v)) {
      stop("data must be a named list")
    }
    if (any(nchar(v)==0)) {
      stop("unnamed variables in data list")
    }
    if (any(duplicated(v))) {
      stop("Duplicated names in data list: ",
         paste(v[duplicated(v)], collapse=" "))
    }
    relevant.variables <- v %in% varnames
    data <- data[relevant.variables]
    unused.variables <- setdiff(v, varnames)
    for (i in seq(along=unused.variables)) {
      warning("Unused variable \"", unused.variables[i], "\" in data")
    }
  }
  else {
    stop("data must be a list or environment")
  }
  
  if(!is.logical(sample.data) || !is.atomic(sample.data)) {
    stop("Invalid sample.data argument.");
  }
  if(missing("data.rng.seed")) {
    data.rng.seed <- runif(1, 0, as.integer(Sys.time()));
  }
  else if(!is.integer(data.rng.seed) || !is.atomic(data.rng.seed)) {
    stop("Invalid data.rng.seed argument.");
  }
  
  .Call("compile_model", p, data, sample.data, as.integer(data.rng.seed), PACKAGE="RBiips")

  model.data <- .Call("get_data", p, PACKAGE="RBiips")
  model.code <- readLines(file, warn=FALSE)
  model <- list("ptr" = function() {p},
                "model" = function() {model.code},
                "dot" = function(dot.file) {
                  .Call("print_graphviz", p, dot.file, PACKAGE="RBiips")
                  invisible(NULL)
                },
                "data" = function() {model.data},
                "nodes" = function(type, observed) {
                   sorted.nodes <- data.frame(.Call("get_sorted_nodes", p, PACKAGE="RBiips"))
                   
                   if(.Call("is_sampler_built", p, PACKAGE="RBiips")) {
                     samplers <- data.frame(.Call("get_node_samplers", p, PACKAGE="RBiips"))
                     sorted.nodes <- cbind(sorted.nodes, samplers)
                   }
                   if(!missing(type)) {
                     if(!is.character(type) || !is.atomic(type)) {
                       stop("Invalid type argument.");
                     }
                     type <- match.arg(type, c("Constant", "Logical", "Stochastic"))
                     sorted.nodes <- sorted.nodes[sorted.nodes["type"]==type,]
                   }
                   if(!missing(observed)) {
                     if(!is.logical(observed) || !is.atomic(observed)) {
                       stop("Invalid observed argument.");
                     }
                     sorted.nodes <- sorted.nodes[sorted.nodes["observed"]==observed,]
                   }
                   return(sorted.nodes);
                  },
                "recompile" = function() {
                  ## Clear the console
                  .Call("clear_console", p, PACKAGE="RBiips")
                  p <<- .Call("make_console", PACKAGE="RBiips")
                  ## Write the model to a temporary file so we can re-read it
                  mf <- tempfile()
                  writeLines(model.code, mf)
                  .Call("check_model", p, mf, PACKAGE="RBiips")
                  unlink(mf)
                  ## Re-compile
                  .Call("compile_model", p, data, FALSE, as.integer(data.rng.seed), PACKAGE="RBiips")
                  invisible(NULL)
                })
  class(model) <- "biips"

  return(model)
}

  
is.biips <- function(obj)
{
  return(class(obj) == "biips")
}


print.biips <- function(x, ...)
{
  if (!is.biips(x))
    stop("Invalid BIIPS model.")
  
  cat("BIIPS model:\n\n")
  
  model <- x$model()
  for (i in 1:length(model)) {
    cat(model[i],"\n",sep="")
  }

  data <- x$data()
  full <- !sapply(lapply(data, is.na), any)
  if (any(full)) {
    cat("\n\nFully observed variables:\n", names(data)[full], "\n")
  }
  part <- !full & !sapply(lapply(data, is.na), all)
  if (any(part)) {
    cat("\nPartially observed variables:\n", names(data)[part], "\n")
  }
}


parse.varname <- function(varname) {

  ## Try to parse string of form "a" or "a[n,p:q,r]" where "a" is a
  ## variable name and n,p,q,r are integers

  v <- try(parse(text=varname, n=1), silent=TRUE)
  if (!is.expression(v) || length(v) != 1)
    return(NULL)

  v <- v[[1]]
  if (is.name(v)) {
    ##Full node array requested
    return(list(name=deparse(v)))
  }
  else if (is.call(v) && identical(deparse(v[[1]]), "[") && length(v) > 2) {
    ##Subset requested
    ndim <- length(v) - 2
    lower <- upper <- numeric(ndim)
    if (any(nchar(sapply(v, deparse)) == 0)) {
      ##We have to catch empty indices here or they will cause trouble
      ##below
      return(NULL)
    }
    for (i in 1:ndim) {
      index <- v[[i+2]]
      if (is.numeric(index)) {
        ##Single index
        lower[i] <- upper[i] <- index
      }
      else if (is.call(index) && length(index) == 3 &&
               identical(deparse(index[[1]]), ":") &&
               is.numeric(index[[2]]) && is.numeric(index[[3]]))
        {
          ##Index range
          lower[i] <- index[[2]]
          upper[i] <- index[[3]]
        }
      else return(NULL)
    }
    if (any(upper < lower))
      return (NULL)
    return(list(name = deparse(v[[2]]), lower=lower, upper=upper))
  }
  return(NULL)
}


parse.varnames <- function(varnames)
{
  names <- character(length(varnames))
  lower <- upper <- vector("list", length(varnames))
  for (i in seq(along=varnames)) {
    y <- parse.varname(varnames[i])
    if (is.null(y)) {
      stop(paste("Invalid variable subset", varnames[i]))
    }
    names[i] <- y$name
    if (!is.null(y$lower)) {
      lower[[i]] <- y$lower
    }
    if (!is.null(y$upper)) {
      upper[[i]] <- y$upper
    }
  }
  return(list(names=names, lower=lower, upper=upper))
}


monitor.biips <- function(obj, variable.names, type="backward.smoothing")
{
  if (!is.biips(obj))
    stop("Invalid BIIPS model.")
  
  if (!is.character(variable.names) || length(variable.names) == 0)
    stop("variable.names must be a character vector")
    
  pn <- parse.varnames(variable.names)
  
  type <- match.arg(type, c("filtering", "smoothing", "backward.smoothing"), several.ok = TRUE)
  if ("filtering" %in% type)
  {
#     .Call("set_filter_monitors", obj$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
    .Call("set_filter_monitors", obj$ptr(), pn$names, PACKAGE="RBiips")
  }
  if ("smoothing" %in% type)
  {
#     .Call("set_smooth_tree_monitors", obj$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
    .Call("set_smooth_tree_monitors", obj$ptr(), pn$names, PACKAGE="RBiips")
  }
  if ("backward.smoothing" %in% type)
  {
#     .Call("set_smooth_monitors", obj$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
    .Call("set_smooth_monitors", obj$ptr(), pn$names, PACKAGE="RBiips")
  }
  invisible(NULL)
}


build.biips <- function(obj, proposal= "auto")
{
  if (!is.biips(obj))
    stop("Invalid BIIPS model")
      
  if (!is.character(proposal) || !is.atomic(proposal)) {
    stop("Invalid proposal argument")
  }
  proposal <- match.arg(proposal, c("auto",
                                  "prior"))
    
  ## build smc sampler
  .Call("build_smc_sampler", obj$ptr(), proposal=="prior", PACKAGE="RBiips")
  
  invisible(NULL)
}


run.biips <- function(obj, n.part, backward=TRUE,
                      rs.thres = 0.5, rs.type = "stratified",
                      smc.rng.seed)
{
  if (!is.biips(obj))
    stop("Invalid BIIPS model")
      
  if (!is.numeric(n.part) || !is.atomic(n.part) || n.part < 1) {
    stop("Invalid n.part argument")
  } 
  if (!is.logical(backward) || !is.atomic(backward)) {
    stop("Invalid backward argument")
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
    smc.rng.seed <- as.integer(runif(1, 0, as.integer(Sys.time())));
  }
  if (!is.integer(smc.rng.seed) || !is.atomic(smc.rng.seed) || smc.rng.seed < 0) {
    stop("Invalid smc.rng.seed argument")
  }
                                    
  if (backward)
  {
    .Call("set_default_monitors", obj$ptr(), PACKAGE="RBiips")
  }
  ## build smc sampler
  if (!.Call("is_sampler_built", obj$ptr(), PACKAGE="RBiips")) {
    .Call("build_smc_sampler", obj$ptr(), "auto", PACKAGE="RBiips")
  }
  
  ## run smc sampler
  log.norm.const <- .Call("run_smc_sampler", obj$ptr(), as.integer(n.part), smc.rng.seed, rs.thres, rs.type, PACKAGE="RBiips")
  
  results <- .Call("get_filter_monitors", obj$ptr(), PACKAGE="RBiips")
  
  results[["log.norm.const"]] <- log.norm.const
  
  mon <- .Call("get_smooth_tree_monitors", obj$ptr(), PACKAGE="RBiips")
  for (n in names(mon))
    results[[n]] <- c(results[[n]], mon[[n]])
  
  if (backward)
  {
    ## run backward smoother
    .Call("run_backward_smoother", obj$ptr(), PACKAGE="RBiips")
    mon <- .Call("get_smooth_monitors", obj$ptr(), PACKAGE="RBiips")
    for (n in names(mon))
      results[[n]] <- c(results[[n]], mon[[n]])
  }
  
#   for (i in seq(along=variable.names)) {
#     .Call("clear_monitor", obj$ptr(), pn$names[i], pn$lower[[i]],
#           pn$upper[[i]], type, PACKAGE="RBiips")
#   }

  for (i in seq(alog=results))
    class(results[[i]]) <- "particles"

  return(results)
}
