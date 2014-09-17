
##' Objects for representing SMC output
##'
##' A \code{smcarray} object is used by the \code{smc_samples} function to
##' represent SMC output from a Biips model. It is an array with named
##' dimensions, for which the dimension \dQuote{smcarray} has a special
##' status.
##'
##' A \code{smcarray.fsb} object is a list of \code{smcarray} objects with
##' different types of monitoring for the same variable.
##'
##' A \code{smcarray.fsb.list} object is a list of \code{smcarray.fsb} objects with
##' different variables.
##'
##' Functions applying to \code{smcarray} objects apply identically to
##' \code{smcarray.fsb} and \code{smcarray.fsb.list} objects by a call to each element of the list.
##'
##' @name smcarray-object
##' @aliases smcarray-object smcarray.fsb-object smcarray.fsb.list-object diagnosis
##' diagnosis.smcarray diagnosis.smcarray.fsb summary.smcarray
##' summary.smcarray.fsb summary.smcarray.fsb.list density.smcarray density.smcarray.fsb
##' plot.smcarray plot.smcarray.fsb
##' @param object,x a \code{smcarray} or \code{smcarray.fsb} or\code{smcarray.fsb.list} object
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
is.smcarray.fsb <- function(object) {
  return(class(object) == "smcarray.fsb")
}

##' @export
is.smcarray.fsb.list <- function(object) {
  return(class(object) == "smcarray.fsb.list")
}


##' @export
print.smcarray <- function(x, ...) {
  print(summary(x), ...)
}


##' @export
print.smcarray.fsb <- function(x, ...) {
  print(summary(x), ...)
}

##' @export
print.smcarray.fsb.list <- function(x, ...) {
  print(summary(x), ...)
  if ("log_marg_like" %in% names(x))
    cat("Log-marginal likelihood: ", x$log_marg_like, "\n")
}


##' @export
summary.smcarray <- function(object, probs = c(), order, ...) {

  ### TODO check arguments
  if (length(probs) > 0)
    stopifnot(is.numeric(probs), probs > 0, probs < 1)

  mode <- all(object$discrete)
  if (missing(order))
    order <- ifelse(mode, 0, 1)

  drop_dims <- names(dim(object$values)) %in% c("particle")
  n_part <- dim(object$values)["particle"]
  dim_array <- dim(object$values)[!drop_dims]
  len <- prod(dim_array)
  m <- which(!drop_dims)
  summ <- list()

  ### moment based statistics
  if (order >= 1)
    summ$mean <- apply(object$values * object$weights, m, FUN = sum)

  if (order >= 2)
    summ$var <- apply(mapply("-", object$values, summ$mean)^2 * object$weights,
                      m, FUN = sum)

  if (order >= 3) {
    mom2 <- apply(object$values^2 * object$weights, m, FUN = sum)
    mom3 <- apply(object$values^3 * object$weights, m, FUN = sum)
    summ$skew <- (mom3 - 3 * mom2 * summ$mean + 2 * summ$mean^3)/summ$var^(3/2)
  }

  if (order >= 4) {
    mom4 <- apply(object$values^4 * object$weights, m, FUN = sum)
    summ$kurt <- (mom4 - 4 * mom3 * summ$mean + 6 * mom2 * summ$mean^2 - 3 *
                    summ$mean^4)/summ$var^2 - 3
  }

  ### quantiles
  if (length(probs) > 0) {
    summ$probs <- probs
    summ$quant <- list()

    for (d in 1:len) {
      indvec <- seq(d, len * (n_part - 1) + d, len)
      stat_d <- Rbiips("wtd_quantile", object$values[indvec], n_part * object$weights[indvec],
                       probs)
      stat_names <- names(stat_d)
      if (d == 1) {
        for (n in stat_names) {
          if (is.null(summ[[n]]))
            summ$quant[[n]] <- array(dim = dim_array)
        }
      }
      for (n in stat_names) summ$quant[[n]][d] <- stat_d[[n]]
    }
  }

  ### mode
  if (mode) {
    summ$mode <- array(dim = dim_array)

    for (d in 1:len) {
      indvec <- seq(d, len * (n_part - 1) + d, len)
      stat_d <- Rbiips("wtd_mode", object$values[indvec], n_part * object$weights[indvec])
      summ$mode[d] <- stat_d[[n]]
    }
  }

  summ$drop_dims <- dim(object$values)[drop_dims]

  class(summ) <- "summary.smcarray"

  return(summ)
}

##' @export
mean.smcarray <- function(x, ...) {
  s <- summary(x, order = 1)
  return(s$mean)
}


##' @export
summary.smcarray.fsb <- function(object, ...) {
  out <- list()
  for (n in names(object)) out[[n]] <- summary(object[[n]], ...)

  class(out) <- "summary.smcarray.fsb"

  return(out)
}

##' @export
summary.smcarray.fsb.list <- function(object, ...) {
  out <- list()

  for (n in names(object)) {
    if (!is.smcarray.fsb(object[[n]]))
      next
    out[[n]] <- summary(object[[n]], ...)
  }

  class(out) <- "summary.smcarray.fsb.list"

  return(out)
}


##' @export
print.summary.smcarray <- function(x, ...) {
  cat("smcarray:\n")
  print(x[!(names(x) %in% c("drop_dims"))], ...)
  if (length(x$drop_dims) > 0) {
    cat("Marginalizing over:", paste(paste(names(x$drop_dims), "(", x$drop_dims,
                                           ")", sep = ""), collapse = ","), "\n")
  }
  invisible()
}


##' @export
print.summary.smcarray.fsb <- function(x, prefix=NULL, ...) {
  for (n in names(x)) {
    if (!is.null(prefix))
      cat(prefix, " ", sep="")
    switch(n, f=cat("filtering "), s=cat("smoothing "), f=cat("backward smoothing "))
    print(x[[n]], ...)
    cat("\n")
  }
  invisible()
}

##' @export
print.summary.smcarray.fsb.list <- function(x, ...) {
  for (n in names(x)) {
    print(x[[n]], prefix=n, ...)
    cat("\n")
  }
  invisible()
}


##' @export
print.diagnosis.smcarray <- function(x, ...) {
  if (x$valid) {
    cat("GOOD\n")
  } else {
    cat("POOR\n")
    cat("    The minimum effective sample size is too low:", x$ess_min,
        "\n")
    cat("    Estimates may be poor for some variables.\n")
    cat("    You should increase the number of particles.")
  }
  invisible()
}


##' @export
diagnosis <- function(object, ...) UseMethod("diagnosis")

##' @export
diagnosis.smcarray <- function(object, ess_thres = 30, quiet = FALSE, ...) {
  stopifnot(is.smcarray(object))
  stopifnot(is.numeric(ess_thres), length(ess_thres) == 1, is.finite(ess_thres),
            ess_thres >= 0)
  stopifnot(is.logical(quiet), length(quiet) == 1)

  ess_min <- min(object$ess)
  out <- list(ess_min = ess_min, valid = ess_min > ess_thres)
  class(out) <- "diagnosis.smcarray"

  if (!quiet) {
    varname <- deparse_varname(object$name, object$lower, object$upper)
    cat("* Diagnosis of variable:", varname, "\n")
    switch(object$type, filtering = {
      cat("  Filtering: ")
    }, smoothing = {
      cat("  Smoothing: ")
    }, backward_smoothing = {
      cat("  Backward smoothing: ")
    })
    print(out)
  }

  return(invisible(out))
}

##' @export
diagnosis.smcarray.fsb <- function(object, type = "fsb", quiet = FALSE, ...) {
  stopifnot(is.smcarray.fsb(object))
  type <- check_type(type)

  if (!quiet) {
    varname <- deparse_varname(object[[1]]$name, object[[1]]$lower, object[[1]]$upper)
    cat("* Diagnosis of variable:", varname, "\n")
  }

  out <- list()
  for (n in names(object)) {
    if (!(n %in% type))
      next
    out[[n]] <- diagnosis(object[[n]], quiet = TRUE, ...)
    if (!quiet) {
      switch(n, f = {
        cat("  Filtering: ")
      }, s = {
        cat("  Smoothing: ")
      }, b = {
        cat("  Backward smoothing: ")
      })
      print(out[[n]])
    }
  }

  return(invisible(out))
}

##' @export
diagnosis.smcarray.fsb.list <- function(object, type = "fsb", quiet = FALSE, ...) {
  stopifnot(is.smcarray.fsb.list(object))

  out <- list()
  for (n in names(object)) {
    if (!is.smcarray.fsb(object[[n]]))
      next
    out <- diagnosis(object[[n]], type = type, quiet = quiet, ...)
  }

  return(invisible(out))
}


##' @importFrom stats density
##' @export
density.smcarray <- function(x, bw = "nrd0", ...) {
  out <- list()
  bww <- bw

  dimen <- dim(x$values)
  drop_dim <- names(dimen) %in% c("particle")

  n_part <- dimen["particle"]
  if (is.na(n_part))
    n_part <- 1

  len <- prod(dimen[!drop_dim])

  for (d in 1:len) {
    ind_vec <- seq(d, len * (n_part - 1) + d, len)
    values <- x$values[ind_vec]
    weights <- x$weights[ind_vec]

    if (length(bw) > 1)
      bww <- bw[[d]]

    if (x$discrete[d]) {
      out[[d]] <- Rbiips("wtd_table", values, weights)
    } else {
      out[[d]] <- density(values, weights = weights, bw = bww, ...)
    }
  }

  dim(out) <- dimen[!drop_dim]
  class(out) <- "density.smcarray"
  return(out)
}


##' @export
density.smcarray.fsb <- function(x, bw = "nrd0", adjust = 1, ...) {
  out <- list()
  bw_s <- bw

  # first treat filtering and backward.smoothing
  if (!is.null(x$f)) {
    dimen <- dim(x$f$values)
    dens <- density(x$f, bw, adjust=adjust, ...)
    bw_s <- list()
    for (d in 1:length(dens)) {
      if (length(out)<d)
        out[[d]] <- list()
      out[[d]]$f <- dens[[d]]
      bw_s[[d]] <- dens[[d]]$bw
    }
  }
  if (!is.null(x$b)) {
    dimen <- dim(x$b$values)
    dens <- density(x$b, bw, adjust=adjust, ...)
    bw_s <- list()
    for (d in 1:length(dens)) {
      if (length(out)<d)
        out[[d]] <- list()
      out[[d]]$b <- dens[[d]]
      bw_s[[d]] <- dens[[d]]$bw
    }
  }

  # then treat smoothing (applying previous bandwidth if any)
  if (!is.null(x$s)) {
    dimen <- dim(x$s$values)
    if (is.list(bw_s))
      adjust <- 1
    dens <- density(x$s, bw_s, adjust=adjust, ...)
    for (d in 1:length(dens)) {
      if (length(out)<d)
        out[[d]] <- list()
      out[[d]]$s <- dens[[d]]
    }
  }

  for (d in 1:length(dens)) class(out[[d]]) <- "density.smcarray.fsb.univariate"

  drop_dim <- names(dimen) %in% c("particle")
  dim(out) <- dimen[!drop_dim]

  class(out) <- "density.smcarray.fsb"
  return(out)
}


##' @export
density.smcarray.fsb.list <- function(x, bw = "nrd0", ...) {
  out <- list()
  for (n in names(x)) {
    if (!is.smcarray.fsb(x[[n]]))
      next
    out[[n]] <- density(x[[n]], bw = bw, ...)
  }

  class(out) <- "density.smcarray.fsb.list"

  return(out)
}

##' @export
plot.density.smcarray <- function(x, ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], ...)
  }
}

##' @export
plot.density.smcarray.fsb.univariate <- function(x, type="l", col=1:6,
                                                 main = NULL, xlab = NULL, ylab = "Density", xlim, ylim, ...) {

  if (missing(xlim)) {
    xmin <- +Inf
    xmax <- -Inf

    for (fsb in 1:length(x)) {
      xmin <- min(xmin, x[[fsb]]$x)
      xmax <- max(xmin, x[[fsb]]$x)
    }
    xlim <- c(xmin,xmax)
  }
  if (missing(ylim)) {
    ymax <- -Inf
    for (fsb in 1:length(x))
      ymax <- max(ymax, x[[fsb]]$y)
    ylim <- c(0,ymax)
  }

  plot(NA, type="n",
       main=main, xlab=xlab, ylab=ylab,
       xlim=xlim, ylim=ylim, ...)
  for (fsb in 1:length(x)) {
    lines(x[[fsb]], type=type, col=col[fsb], ...)
  }
}

##' @export
plot.density.smcarray.fsb <- function(x, ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], ...)
  }
}

##' @export
plot.density.smcarray.fsb.list <- function(x, ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], ...)
  }
}

# ##' @export
# plot.density.smcarray.univariate <- function(x, type = "l", lwd = 1, col = 1:6, xlab = "value",
#   ylab, main, sub, legend.text = NULL, args_legend = NULL, ...) {
#   lwdd <- lwd
#   coll <- col
#
#   legend.density.smcarray <- function(x = "topright", y = NULL, lwd = lwdd, col = coll,
#     ...) {
#     return(legend(x = x, y = y, lwd = lwd, col = col, ...))
#   }
#
#   leg_flag <- TRUE
#   if (is.logical(legend.text)) {
#     stopifnot(length(legend.text) == 1)
#     leg_flag <- legend.text
#   }
#
#   if (leg_flag && (is.logical(legend.text) || is.null(legend.text))) {
#     legend.text <- c()
#     for (t in names(x)) {
#       legend.text <- c(legend.text, paste(x[[t]]$type, ", ess=", round(x$ess),
#         sep = ""))
#     }
#   }
#
#
#   if (x$discrete) {
#     if (missing(ylab))
#       ylab <- "probability"
#     if (missing(main))
#       main <- paste(x$name, "discrete law histograms")
#     if (missing(sub))
#       sub <- paste("n_part=", x$n_part)
#
#     barplot(x$density$y, names.arg = x$density$x, col = col, lwd = lwd, xlab = xlab,
#       ylab = ylab, main = main.title, sub = sub, legend.text = legend.text,
#       args_legend = args_legend, ...)
#   } else {
#     if (missing(ylab))
#       ylab <- "density"
#     if (missing(main))
#       main <- paste(x$name, "kernel density estimates")
#     if (missing(sub)) {
#       bw <- x$density$bw
#       sub <- paste("n_part=", x$n_part, ", bw=", signif(bw, digits = 2), sep = "")
#     }
#
#     plot(x$density$x, x$density$y, type = type, lwd = lwd, col = col, xlab = xlab,
#       ylab = ylab, main = main, sub = sub, ...)
#
#     if (leg_flag)
#       do.call(legend.density.smcarray, c(legend = legend.text, args_legend))
#   }
#
#   invisible()
# }
#
#
# ##' @export
# plot.density.smcarray <- function(x, type = "l", lwd = lwd, col = 1:6, ...) {
#   for (n in names(x)) {
#     plot(x[[n]], type = type, lwd = lwd, col = col, ...)
#   }
#   invisible()
# }
#
#
# ##' @export
# plot.density.smcarray.univariate.list <- function(x, type = "l", lwd = 1, col = 1:6,
#   xlab = "value", ylab, main, sub, legend.text = NULL, args_legend = NULL, ...) {
#   lwdd <- lwd
#   coll <- col
#
#   legend.density.smcarray <- function(x = "topright", y = NULL, lwd = lwdd, col = coll,
#     ...) {
#     return(legend(x = x, y = y, lwd = lwd, col = col, ...))
#   }
#
#   leg_flag <- TRUE
#   if (is.logical(legend.text)) {
#     stopifnot(length(legend.text) > 0)
#     leg_flag <- legend.text
#   }
#
#   if (leg_flag && (is.logical(legend.text) || is.null(legend.text))) {
#     legend.text <- c()
#     for (t in names(x)) {
#       legend.text <- c(legend.text, paste(x[[t]]$type, ", ess=", round(x$ess),
#         sep = ""))
#     }
#   }
#
#   if (x[[1]]$discrete) {
#     # get table locations
#     xx <- c()
#     for (t in names(x)) xx <- c(xx, x[[t]]$density$x)
#     xx <- sort(unique(xx))
#
#     yy <- c()
#     for (t in names(x)) {
#       # resize values with missing 0 to empty locations
#       y <- rep(0, length(xx))
#       y[xx %in% x[[t]]$density$x] <- x[[t]]$density$y
#       yy <- c(yy, y)
#     }
#     yy <- matrix(yy, nrow = length(x), byrow = TRUE)
#
#     if (missing(ylab))
#       ylab <- "probability"
#     if (missing(main))
#       main <- paste(x[[1]]$name, "discrete law histograms")
#     if (missing(sub))
#       sub <- paste("n_part=", x[[1]]$n_part, sep = "")
#
#     if (length(col) > length(x))
#       col <- col[1:length(x)]
#     if (length(density) > length(x))
#       density <- density[1:length(x)]
#     if (length(angle) > length(x))
#       angle <- angle[1:length(x)]
#     barplot(yy, names.arg = xx, col = col, xlab = xlab, ylab = ylab, main = main,
#       sub = sub, legend.text = legend.text, args_legend = args_legend, beside = TRUE,
#       ...)
#   } else {
#     xx <- c()
#     yy <- c()
#
#     for (t in names(x)) {
#       xx <- c(xx, x[[t]]$density$x)
#       yy <- c(yy, x[[t]]$density$y)
#     }
#     xx <- matrix(xx, ncol = length(x))
#     yy <- matrix(yy, ncol = length(x))
#
#     if (missing(ylab))
#       ylab <- "density"
#     if (missing(main))
#       main <- paste(x[[1]]$name, "kernel density estimates")
#     if (missing(sub)) {
#       bw <- x[[1]]$density$bw
#       sub <- paste("n_part=", x[[1]]$n_part, ", bw=", signif(bw, digits = 2),
#         sep = "")
#     }
#
#     matplot(xx, yy, type = type, lwd = lwd, col = col, xlab = xlab, ylab = ylab,
#       main = main, sub = sub, ...)
#
#     if (leg_flag)
#       do.call(legend.density.smcarray, c(legend = legend.text, args_legend))
#   }
#
#   invisible()
# }
#
#
# ##' @export
# plot.density.smcarray.fsb <- function(x, type = "l", lwd = 1, col = 1:6, ...) {
#   for (n in names(x)) {
#     plot(x[[n]], type = type, lwd = lwd, col = col, ...)
#   }
#   invisible()
# }
