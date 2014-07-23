
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
##' @aliases smcarray-object smcarray.list-object diagnosis
##' diagnosis.smcarray diagnosis.smcarray.list summary.smcarray
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

##' @export
is.smcarray.list <- function(object) {
  return(class(object) == "smcarray.list")
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


##' @S3method smcarray mean
mean.smcarray <- function(x, ...) {
  s <- stat.smcarray(x, fun = "mean")
  return(s$Mean)
}

stat.smcarray <- function(x, fun = "mean", probs = c(0.25, 0.5, 0.75)) {
  stopifnot(is.character(fun))
  stopifnot (is.numeric(probs), probs >0, probs < 1)
  
  fun <- match.arg(fun, c("mean", "var", "skew", "kurt", "median", "quantile", 
                          "mode"), several.ok = TRUE)
  
  n_part <- dim(x$values)["particle"]
  drop_dims <- names(dim(x$values)) %in% c("particle")
  dim_stat <- dim(x$values)[!drop_dims]
  len <- prod(dim_stat)
  
  stat <- list()
  for (d in 1:len) {
    indvec <- seq(d, len * (n_part - 1) + d, len)
    for (f in fun) {
      if (f == "quantile") {
        stat_d <- RBiips("weighted_quantile", x$values[indvec], n_part * 
                           x$weights[indvec], probs)
      } else {
        stat_d <- RBiips(paste("weighted_", f, sep = ""), x$values[indvec], 
                         n_part * x$weights[indvec])
      }
      stat_names <- names(stat_d)
      if (d == 1) {
        for (n in stat_names) {
          if (is.null(stat[[n]])) 
            stat[[n]] <- array(dim = dim_stat)
        }
      }
      for (n in stat_names) stat[[n]][d] <- stat_d[[n]]
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
      "quantile"), several.ok = TRUE)
  } else {
    if (missing(fun)) 
      fun <- "mean" else fun <- match.arg(fun, c("mean", "var", "skew", "kurt", "median", "quantile"), 
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
diagnosis <- function(object, ...) UseMethod("diagnosis")

##' @S3method diagnosis list
diagnosis.list <- function(object, type="fsb", quiet=FALSE, ...) {
  stopifnot(is.list(object))
  
  out = list()
  for (n in names(object)) {
    if (!is.smcarray.list(object[[n]]))
      next
    out <- diagnosis(object[[n]], type=type, quiet=quiet, ...)
  }
  
  return(invisible(out))
}

##' @S3method diagnosis smcarray.list
diagnosis.smcarray.list <- function(object, type="fsb", quiet=FALSE, ...) {
  stopifnot(is.smcarray.list(object))
  type = check_type(type)
  
  if (!quiet) {
    varname = deparse_varname(object[[1]]$name, object[[1]]$lower,
                              object[[1]]$upper)
    cat("* Diagnosis of variable:", varname, "\n")      
  }
  
  out = list()
  for (n in names(object)) {
    if (! (n %in% type))
      next
    
    if (!quiet) {
      switch(n,
             f = {
               cat("  Filtering: ")
             },
             s = {
               cat("  Smoothing: ")
             },
             b = {
               cat("  Backward smoothing: ")
             })
    }
    out[[n]] <-  diagnosis(object[[n]], quiet=quiet, ...)
  }
  
  return(invisible(out))
}

##' @S3method diagnosis smcarray
diagnosis.smcarray <- function(object, ess_thres = 30, quiet=FALSE, ...) {
  stopifnot(is.smcarray(object))
  stopifnot(is.numeric(ess_thres), length(ess_thres)==1, is.finite(ess_thres), ess_thres>=0)
  stopifnot(is.logical(quiet), length(quiet)==1)
  
  ess_min <- min(object$ess)
  out <- list(ess_min = ess_min, valid = ess_min > ess_thres)
  
  if (!quiet) {    
    if (out$valid) {
      cat("GOOD\n") 
    } else {
      cat("POOR\n")
      cat("    The minimum effective sample size is too low:", x$ess_min, "\n")
      cat("    Estimates may be poor for some variables.\n")
      cat("    You should increase the number of particles.")
    }
  }
  
  return(invisible(out))
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
    class(ans[[varname]]) <- "density.smcarray.single"
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
  
  for (n in names(ans)) class(ans[[n]]) <- "density.smcarray.single.list"
  
  class(ans) <- "density.smcarray.list"
  return(ans)
}


##' @S3method plot density.smcarray.single
plot.density.smcarray.single <- function(x, type = "l", lwd = 1, col = 1:6, xlab = "value", 
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


##' @S3method plot density.smcarray.single.list
plot.density.smcarray.single.list <- function(x, type = "l", lwd = 1, col = 1:6, 
  xlab = "value", ylab, main, sub, legend.text = NULL, args.legend = NULL, ...) {
  lwdd <- lwd
  coll <- col
  
  legend.density.smcarray <- function(x = "topright", y = NULL, lwd = lwdd, col = coll, 
    ...) {
    return(legend(x = x, y = y, lwd = lwd, col = col, ...))
  }
  
  leg.flag <- TRUE
  if (is.logical(legend.text)) {
    stopifnot(is.single(legend.text))
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
