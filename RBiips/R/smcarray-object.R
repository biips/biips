
##' Objects for representing SMC output
##' 
##' A \code{smcarray} object is used by the \code{smc_samples} function to
##' represent SMC output from a BiiPS model. It is an array with named
##' dimensions, for which the dimension \dQuote{smcarray} has a special
##' status.
##' 
##' A \code{smcarray.list} object is a list of \code{smcarray} objects with
##' different types of monitoring for the same variable.
##' 
##' Functions applying to \code{smcarray} objects apply identically to
##' \code{smcarray.list} objects by a call to each element of the list.
##' 
##' @name smcarray-object
##' @aliases smcarray-object smcarray.list-object diagnostic
##' diagnostic.smcarray diagnostic.smcarray.list summary.smcarray
##' summary.smcarray.list density.smcarray density.smcarray.list
##' plot.smcarray plot.smcarray.list
##' @param object,x a \code{smcarray} or \code{smcarray.list} object
##' @param fun a character vector indicating the functions to be used to
##' generate summary statistics
##' @param probs a numerical vector containing the desired quantile
##' probabilities
##' @param bw the smoothing bandwidth to be used. See \code{\link{density}} for
##' more details
##' @param adjust the bandwidth used is actually \code{adjust*bw}. This makes
##' it easy to specify values like \sQuote{half the default} bandwidth.
##' @param subset
##' @param ess_thres
##' @param ... additional arguments to be passed to the default methods
##' @return The \code{summary} method for \code{smcarray} objects applies the
##' given function to the array, marginalizing the \dQuote{smcarray}
##' dimension.
##' @author Adrien Todeschini, Francois Caron
##' @seealso \link{density}
##' @keywords models
##' @examples
##' 
##' ## Should be DIRECTLY executable !! 
##' ##-- ==>  Define data, use random,
##' ##--  or do  help(data=index)  for the standard data sets.
##' 
NULL


##' @export
is.smcarray <- function(object) {
  return(class(object) == "smcarray")
}


##' @S3method print smcarray
print.smcarray <- function(x, fun = c("mean", "mode"), probs = c(0.25, 0.5, 0.75), 
  ...) {
  print(summary(x, fun, probs), ...)
}


##' @S3method print smcarray.list
print.smcarray.list <- function(x, fun = c("mean", "mode"), probs = c(0.25, 0.5, 
  0.75), ...) {
  print(summary(x, fun, probs), ...)
}


stat.smcarray <- function(x, fun = "mean", probs = c(0.25, 0.5, 0.75)) {
  if (!is.character(fun)) 
    stop("invalid fun argument.")
  
  fun <- match.arg(fun, c("mean", "var", "skew", "kurt", "median", "quantiles", 
    "mode"), several.ok = TRUE)
  if (any(fun == "quantiles")) {
    if (!is.numeric(probs) || any(probs <= 0) || any(probs >= 1)) 
      stop("invalid probs argument.")
    probs <- sort(probs)
  }
  
  n_part <- dim(x$values)["particle"]
  drop_dims <- names(dim(x$values)) %in% c("particle")
  dim.stat <- dim(x$values)[!drop_dims]
  len <- prod(dim.stat)
  
  stat <- list()
  for (d in 1:len) {
    indvec <- seq(d, len * (n_part - 1) + d, len)
    for (f in fun) {
      if (f == "quantiles") 
        d.stat <- RBiips("weighted_quantiles", x$values[indvec], n_part * 
          x$weights[indvec], probs) else d.stat <- .Call(paste("weighted_", f, sep = ""), x$values[indvec], 
        n_part * x$weights[indvec], PACKAGE = "RBiips")
      stat.names <- names(d.stat)[names(d.stat) != "Table"]
      if (d == 1) {
        for (n in stat.names) {
          if (is.null(stat[[n]])) 
          stat[[n]] <- array(dim = dim.stat)
        }
      }
      for (n in stat.names) stat[[n]][d] <- d.stat[[n]]
    }
  }
  return(stat)
}


##' @S3method summary smcarray
summary.smcarray <- function(object, fun, probs = c(0.25, 0.5, 0.75), ...) {
  if (is.null(object$values) || is.null(object$weights) || is.null(object$discrete)) {
    NextMethod()
  }
  
  if (all(object$discrete)) {
    if (missing(fun)) 
      fun <- "mode" else fun <- match.arg(fun, c("mode", "mean", "var", "skew", "kurt", "median", 
      "quantiles"), several.ok = TRUE)
  } else {
    if (missing(fun)) 
      fun <- "mean" else fun <- match.arg(fun, c("mean", "var", "skew", "kurt", "median", "quantiles"), 
      several.ok = TRUE)
  }
  
  drop_dims <- names(dim(object$values)) %in% c("particle")
  
  stat <- stat.smcarray(object, fun, probs)
  stat[["drop_dims"]] <- dim(object$values)[drop_dims]
  stat[["name"]] <- deparse.varname(object$name, object$lower, object$upper)
  stat[["type"]] <- object$type
  class(stat) <- "summary.smcarray"
  
  return(stat)
}


##' @S3method summary smcarray.list
summary.smcarray.list <- function(object, ...) {
  ans <- list()
  for (n in names(object)) ans[[n]] <- summary(object[[n]], ...)
  
  class(ans) <- "summary.smcarray.list"
  
  return(ans)
}


##' @S3method print summary.smcarray
print.summary.smcarray <- function(x, ...) {
  cat(x$name, x$type, "smcarray:\n")
  print(x[!(names(x) %in% c("drop_dims", "name", "type"))], ...)
  if (length(x$drop_dims) > 0) {
    cat("Marginalizing over:", paste(paste(names(x$drop_dims), "(", x$drop_dims, 
      ")", sep = ""), collapse = ","), "\n")
  }
  invisible()
}


##' @S3method print summary.smcarray.list
print.summary.smcarray.list <- function(x, ...) {
  for (n in names(x)) {
    print(x[[n]], ...)
    cat("\n")
  }
  invisible()
}


##' @S3method plot summary.smcarray
plot.summary.smcarray <- function(x, type = "l", lty = 1:5, lwd = 2, col = 1:6, xlab = "offset", 
  ylab = "value", main, sub, args.legend = list(), ...) {
  ltyy <- lty
  lwdd <- lwd
  coll <- col
  
  legend.summary.smcarray <- function(x = "topright", y = NULL, lty = ltyy, lwd = lwdd, 
    col = coll, bty = "n", inset = c(0.01, 0.01), ...) {
    return(legend(x = x, y = y, lty = lty, lwd = lwd, col = col, bty = bty, inset = inset, 
      ...))
  }
  
  stat.names <- names(x)[!(names(x) %in% c("2nd mt.", "Var.", "SD", "3rd mt.", 
    "Skew.", "4th mt.", "Kurt.", "drop_dims", "name", "type"))]
  
  values <- c()
  for (n in stat.names) {
    values <- c(values, x[[n]])
  }
  
  mat <- matrix(values, ncol = length(stat.names))
  n_part <- x$drop_dims[["particle"]]
  if (missing(main)) 
    main <- paste(x$name, x$type, "particle estimates")
  if (missing(sub)) 
    sub <- paste("Marginalizing over:", paste(paste(names(x$drop_dims), "(", 
      x$drop_dims, ")", sep = ""), collapse = ","))
  matplot(mat, type = type, xlab = xlab, ylab = ylab, main = main, sub = sub, lty = lty, 
    lwd = lwd, col = col, ...)
  
  args.legend[["legend"]] <- stat.names
  do.call("legend.summary.smcarray", args.legend)
  invisible()
}


##' @S3method plot summary.smcarray.list
plot.summary.smcarray.list <- function(x, ...) {
  for (n in names(x)) plot(x[[n]], ...)
  invisible()
}


##' @S3method plot smcarray
plot.smcarray <- function(x, fun = c("mean", "mode"), probs = c(0.25, 0.5, 0.75), 
  ...) {
  plot(summary(x, fun, probs), ...)
  invisible()
}


##' @S3method plot smcarray.list
plot.smcarray.list <- function(x, fun = c("mean", "mode"), probs = c(0.25, 0.5, 0.75), 
  ...) {
  for (n in names(x)) plot(x[[n]], fun, probs, ...)
  invisible()
}


##' @export
diagnostic <- function(object, ...) UseMethod("diagnostic")

##' @S3method diagnostic list
diagnostic.list <- function(object, ...) {
  stopifnot(is.list(object))
  
  flag <- sapply(object, is.smcarray)
  out <- lapply(object[flag], function(x) diagnostic(x,...))
  
  return(out)
}

##' @S3method diagnostic smcarray
diagnostic.smcarray <- function(object, ess_thres = 30, ...) {
  ess_min <- min(object$ess)
  ans <- list(`ESS min.` = ess_min, valid = ess_min > ess_thres)
  class(ans) <- "diagnostic.smcarray"
  return(ans)
}


##' @S3method print diagnostic.smcarray
print.diagnostic.smcarray <- function(x, ...) {
  if (x$valid) 
    cat("diagnostic: GOOD\n") else {
    cat("diagnostic: POOR\n")
    cat("    The minimum effective sample size is too low: ", x$ESS, "\n", sep = "")
    cat("    Estimates may be poor for some variables.\n")
    cat("    You should increase n_part.")
  }
}


##' @S3method diagnostic smcarray.list
diagnostic.smcarray.list <- function(object, ...) {
  ans <- list()
  for (n in names(object)) ans[[n]] <- diagnostic(object[[n]], ...)
  return(ans)
}


get.index <- function(offset, lower, upper) {
  if (length(lower) != length(upper)) 
    stop("lower and upper dimension mismatch.")
  dim <- upper - lower + 1
  if (any(dim <= 0)) 
    stop("incorrect lower and upper values.")
  ind <- lower
  offset <- offset - 1
  for (i in 1:length(dim)) {
    ind[i] <- ind[i] + (offset)%%dim[i]
    offset <- offset%/%dim[i]
  }
  return(ind)
}


deparse.varname <- function(name, lower = NULL, upper = lower) {
  v <- parse(text = name, n = 1)
  if (!is.expression(v) || length(v) != 1 || !is.name(v[[1]])) 
    stop("invalid variable name.")
  varname <- deparse(v[[1]])
  
  if (length(lower) == 0) 
    return(varname)
  
  varname <- paste(varname, "[", sep = "")
  if (length(upper) != length(lower)) 
    stop("lower and upper lengths mismatch.")
  if (any(upper < lower)) 
    stop("incorrect lower and upper values.")
  sep <- ""
  for (i in seq(along = lower)) {
    varname <- paste(varname, lower[[i]], sep = sep)
    if (upper[[i]] > lower[[i]]) 
      varname <- paste(varname, upper[[i]], sep = ":")
    sep <- ","
  }
  varname <- paste(varname, "]", sep = "")
  return(varname)
}


##' @importFrom stats density
##' @S3method density smcarray
density.smcarray <- function(x, bw = "nrd0", adjust = 1, subset, ...) {
  ans <- list()
  bww <- bw
  
  n_part <- dim(x$values)["particle"]
  
  if (!missing(subset)) {
    if (!is.character(subset) || length(subset) != 1) 
      stop("invalid subset argument.")
    pn <- parse.varname(subset)
    if (pn$name != x$name) 
      stop("invalid subset argument: wrong variable name.")
    if (any(pn$lower < x$lower) || any(pn$upper > x$upper)) 
      stop("invalid subset argument: index out of range.")
    lower <- pn$lower
    upper <- pn$upper
    subset.dim <- upper - lower + 1
    len <- prod(subset.dim)
  } else {
    lower <- x$lower
    upper <- x$upper
    drop.dim <- names(dim(x$values)) %in% c("particle")
    subset.dim <- dim(x$values)[!drop.dim]
    len <- prod(subset.dim)
  }
  
  for (d in 1:len) {
    ind <- get.index(d, lower, upper)
    varname <- deparse.varname(x$name, ind)
    
    if (!missing(subset)) {
      ind.mat <- array(c(rep(ind, each = n_part), 1:n_part), dim = c(n_part, 
        length(dim(x$values))))
      values <- x$values[ind.mat]
      weights <- x$weights[ind.mat]
      ind2 <- array(ind, dim = c(1, length(ind)))
      ess <- x$ess[ind2]
      discrete <- x$discrete[ind2]
    } else {
      ind.vec <- seq(d, len * (n_part - 1) + d, len)
      values <- x$values[ind.vec]
      weights <- x$weights[ind.vec]
      ess <- x$ess[d]
      discrete <- x$discrete[d]
    }
    
    if (length(bw) != 1) 
      bww <- bw[[d]]
    
    if (discrete) {
      table <- RBiips("weighted_table", values, weights)
      dens <- list(x = table[["Table"]]$x, y = table[["Table"]]$y)
    } else {
      dens <- density(values, weights = weights, bw = bww, adjust = adjust, 
        ...)
    }
    
    ans[[varname]] <- list(density = dens, name = varname, type = x$type, n_part = n_part, 
      ess = ess, discrete = discrete)
    class(ans[[varname]]) <- "density.smcarray.atomic"
  }
  
  class(ans) <- "density.smcarray"
  return(ans)
}


##' @S3method density smcarray.list
density.smcarray.list <- function(x, bw = "nrd0", adjust = 1, subset, ...) {
  ans <- list()
  bw.s <- bw
  
  # first treat filtering and backward.smoothing
  if (!is.null(x[["filtering"]])) {
    dens <- density.smcarray(x[["filtering"]], bw, adjust, subset, ...)
    bw.s <- list()
    for (n in names(dens)) {
      ans[[n]][["filtering"]] <- dens[[n]]
      bw.s[[n]] <- dens[[n]]$density$bw
    }
  }
  if (!is.null(x[["backward.smoothing"]])) {
    dens <- density.smcarray(x[["backward.smoothing"]], bw, adjust, subset, ...)
    bw.s <- list()
    for (n in names(dens)) {
      ans[[n]][["backward.smoothing"]] <- dens[[n]]
      bw.s[[n]] <- dens[[n]]$density$bw
    }
  }
  
  # then treat smoothing (applying previous bandwidth if any)
  if (!is.null(x[["smoothing"]])) {
    if (is.list(bw.s)) 
      adjust <- 1
    dens <- density.smcarray(x[["smoothing"]], bw.s, adjust, subset, ...)
    for (n in names(dens)) ans[[n]][["smoothing"]] <- dens[[n]]
  }
  
  for (n in names(ans)) class(ans[[n]]) <- "density.smcarray.atomic.list"
  
  class(ans) <- "density.smcarray.list"
  return(ans)
}


##' @S3method plot density.smcarray.atomic
plot.density.smcarray.atomic <- function(x, type = "l", lwd = 1, col = 1:6, xlab = "value", 
  ylab, main, sub, legend.text = NULL, args.legend = NULL, ...) {
  lwdd <- lwd
  coll <- col
  
  legend.density.smcarray <- function(x = "topright", y = NULL, lwd = lwdd, col = coll, 
    ...) {
    return(legend(x = x, y = y, lwd = lwd, col = col, ...))
  }
  
  leg.flag <- TRUE
  if (is.logical(legend.text)) {
    stopifnot(length(legend.text) == 1)
    leg.flag <- legend.text
  }
  
  if (leg.flag && (is.logical(legend.text) || is.null(legend.text))) {
    legend.text <- c()
    for (t in names(x)) {
      legend.text <- c(legend.text, paste(x[[t]]$type, ", ess=", round(x$ess), 
        sep = ""))
    }
  }
  
  
  if (x$discrete) {
    if (missing(ylab)) 
      ylab <- "probability"
    if (missing(main)) 
      main <- paste(x$name, "discrete law histograms")
    if (missing(sub)) 
      sub <- paste("n_part=", x$n_part)
    
    barplot(x$density$y, names.arg = x$density$x, col = col, lwd = lwd, xlab = xlab, 
      ylab = ylab, main = main.title, sub = sub, legend.text = legend.text, 
      args.legend = args.legend, ...)
  } else {
    if (missing(ylab)) 
      ylab <- "density"
    if (missing(main)) 
      main <- paste(x$name, "kernel density estimates")
    if (missing(sub)) {
      bw <- x$density$bw
      sub <- paste("n_part=", x$n_part, ", bw=", signif(bw, digits = 2), sep = "")
    }
    
    plot(x$density$x, x$density$y, type = type, lwd = lwd, col = col, xlab = xlab, 
      ylab = ylab, main = main, sub = sub, ...)
    
    if (leg.flag) 
      do.call(legend.density.smcarray, c(legend = legend.text, args.legend))
  }
  
  invisible()
}


##' @S3method plot density.smcarray
plot.density.smcarray <- function(x, type = "l", lwd = lwd, col = 1:6, ...) {
  for (n in names(x)) {
    plot(x[[n]], type = type, lwd = lwd, col = col, ...)
  }
  invisible()
}


##' @S3method plot density.smcarray.atomic.list
plot.density.smcarray.atomic.list <- function(x, type = "l", lwd = 1, col = 1:6, 
  xlab = "value", ylab, main, sub, legend.text = NULL, args.legend = NULL, ...) {
  lwdd <- lwd
  coll <- col
  
  legend.density.smcarray <- function(x = "topright", y = NULL, lwd = lwdd, col = coll, 
    ...) {
    return(legend(x = x, y = y, lwd = lwd, col = col, ...))
  }
  
  leg.flag <- TRUE
  if (is.logical(legend.text)) {
    stopifnot(is.atomic(legend.text))
    leg.flag <- legend.text
  }
  
  if (leg.flag && (is.logical(legend.text) || is.null(legend.text))) {
    legend.text <- c()
    for (t in names(x)) {
      legend.text <- c(legend.text, paste(x[[t]]$type, ", ess=", round(x$ess), 
        sep = ""))
    }
  }
  
  if (x[[1]]$discrete) {
    # get table locations
    xx <- c()
    for (t in names(x)) xx <- c(xx, x[[t]]$density$x)
    xx <- sort(unique(xx))
    
    yy <- c()
    for (t in names(x)) {
      # resize values with missing 0 to empty locations
      y <- rep(0, length(xx))
      y[xx %in% x[[t]]$density$x] <- x[[t]]$density$y
      yy <- c(yy, y)
    }
    yy <- matrix(yy, nrow = length(x), byrow = TRUE)
    
    if (missing(ylab)) 
      ylab <- "probability"
    if (missing(main)) 
      main <- paste(x[[1]]$name, "discrete law histograms")
    if (missing(sub)) 
      sub <- paste("n_part=", x[[1]]$n_part, sep = "")
    
    if (length(col) > length(x)) 
      col <- col[1:length(x)]
    if (length(density) > length(x)) 
      density <- density[1:length(x)]
    if (length(angle) > length(x)) 
      angle <- angle[1:length(x)]
    barplot(yy, names.arg = xx, col = col, xlab = xlab, ylab = ylab, main = main, 
      sub = sub, legend.text = legend.text, args.legend = args.legend, beside = TRUE, 
      ...)
  } else {
    xx <- c()
    yy <- c()
    
    for (t in names(x)) {
      xx <- c(xx, x[[t]]$density$x)
      yy <- c(yy, x[[t]]$density$y)
    }
    xx <- matrix(xx, ncol = length(x))
    yy <- matrix(yy, ncol = length(x))
    
    if (missing(ylab)) 
      ylab <- "density"
    if (missing(main)) 
      main <- paste(x[[1]]$name, "kernel density estimates")
    if (missing(sub)) {
      bw <- x[[1]]$density$bw
      sub <- paste("n_part=", x[[1]]$n_part, ", bw=", signif(bw, digits = 2), 
        sep = "")
    }
    
    matplot(xx, yy, type = type, lwd = lwd, col = col, xlab = xlab, ylab = ylab, 
      main = main, sub = sub, ...)
    
    if (leg.flag) 
      do.call(legend.density.smcarray, c(legend = legend.text, args.legend))
  }
  
  invisible()
}


##' @S3method plot density.smcarray.list
plot.density.smcarray.list <- function(x, type = "l", lwd = 1, col = 1:6, ...) {
  for (n in names(x)) {
    plot(x[[n]], type = type, lwd = lwd, col = col, ...)
  }
  invisible()
} 
