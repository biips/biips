
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
summary.smcarray <- function(object, ...) {
  return(biips_summary(object, ...))
}
##' @export
summary.smcarray.fsb <- function(object, ...) {
  return(biips_summary(object, ...))
}
##' @export
summary.smcarray.fsb.list <- function(object, ...) {
  return(biips_summary(object, ...))
}

##' @export
biips_summary.smcarray <- function(object, probs = c(), order, mode = all(object$discrete), ...) {
  stopifnot(is.smcarray(object))

  ### TODO check arguments
  if (length(probs) > 0)
    stopifnot(is.numeric(probs), probs > 0, probs < 1)

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
      quant_names <- names(stat_d)
      if (d == 1) {
        for (n in quant_names) {
          if (is.null(summ$quant[[n]]))
            summ$quant[[n]] <- array(dim = dim_array)
        }
      }
      for (n in quant_names) summ$quant[[n]][d] <- stat_d[[n]]
    }
  }

  ### mode
  if (mode) {
    summ$mode <- array(dim = dim_array)

    for (d in 1:len) {
      indvec <- seq(d, len * (n_part - 1) + d, len)
      stat_d <- Rbiips("wtd_mode", object$values[indvec], n_part * object$weights[indvec])
      summ$mode[d] <- stat_d
    }
  }

  summ$drop_dims <- dim(object$values)[drop_dims]

  class(summ) <- "summary.smcarray"

  return(summ)
}

##' @export
mean.smcarray <- function(x, ...) {
  stopifnot(is.smcarray(x))
  s <- summary(x, order = 1)
  return(s$mean)
}

##' @export
biips_summary.smcarray.fsb <- function(object, ...) {
  stopifnot(is.smcarray.fsb(object))
  out <- list()
  for (n in names(object)) out[[n]] <- biips_summary(object[[n]], ...)

  class(out) <- "summary.smcarray.fsb"

  return(out)
}

##' @export
biips_summary.smcarray.fsb.list <- function(object, ...) {
  stopifnot(is.smcarray.fsb.list(object))
  out <- list()

  for (n in names(object)) {
    if (!is.smcarray.fsb(object[[n]]))
      next
    out[[n]] <- biips_summary(object[[n]], ...)
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
biips_diagnosis <- function(object, ...) UseMethod("biips_diagnosis")

##' @export
biips_diagnosis.smcarray <- function(object, ess_thres = 30, quiet = FALSE, ...) {
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
biips_diagnosis.smcarray.fsb <- function(object, type = "fsb", quiet = FALSE, ...) {
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
    out[[n]] <- biips_diagnosis(object[[n]], quiet = TRUE, ...)
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
biips_diagnosis.smcarray.fsb.list <- function(object, type = "fsb", quiet = FALSE, ...) {
  stopifnot(is.smcarray.fsb.list(object))

  out <- list()
  for (n in names(object)) {
    if (!is.smcarray.fsb(object[[n]]))
      next
    out <- biips_diagnosis(object[[n]], type = type, quiet = quiet, ...)
  }

  return(invisible(out))
}

##' @export
table.smcarray <- function(x, ...) {
  return(biips_table(x, ...))
}
##' @export
table.smcarray.fsb <- function(x, ...) {
  return(biips_table(x, ...))
}
##' @export
table.smcarray.fsb.list <- function(x, ...) {
  return(biips_table(x, ...))
}
##' @importFrom stats density
##' @export
density.smcarray <- function(x, ...) {
  return(biips_density(x, ...))
}
##' @export
density.smcarray.fsb <- function(x, ...) {
  return(biips_density(x, ...))
}
##' @export
density.smcarray.fsb.list <- function(x, ...) {
  return(biips_density(x, ...))
}

##' @export
biips_table.smcarray <- function(x, ...) {
  stopifnot(is.smcarray(x))
  out <- list()

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

    out[[d]] <- Rbiips("wtd_table", values, weights) # recycle bw
  }

  dim(out) <- dimen[!drop_dim]
  class(out) <- "table.smcarray"
  return(out)
}

##' @export
biips_density.smcarray <- function(x, bw="nrd0", ...) {
  stopifnot(is.smcarray(x))
  out <- list()

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

    out[[d]] <- density(values, weights=weights, bw=rec(bw,d), ...) # recycle bw
  }

  dim(out) <- dimen[!drop_dim]
  class(out) <- "density.smcarray"
  return(out)
}

##' @export
biips_table.smcarray.fsb <- function(x, ...) {
  stopifnot(is.smcarray.fsb(x))
  out <- list()
  for (fsb in names(x)) {
    if (!is.smcarray(x[[fsb]]))
      next
    out[[fsb]] <- biips_table(x[[fsb]],...)
  }

  class(out) <- "table.smcarray.fsb"
  return(out)
}

##' @export
biips_density.smcarray.fsb <- function(x, bw="nrd0", adjust=1, ...) {
  stopifnot(is.smcarray.fsb(x))
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
biips_table.smcarray.fsb.list <- function(x, ...) {
  stopifnot(is.smcarray.fsb.list(x))
  out <- list()
  for (i in 1:length(x)) {
    if (!is.smcarray.fsb(x[[i]]))
      next
    name <- names(x)[i]
    out[[name]] <- biips_table(x[[i]], ...)
  }

  class(out) <- "table.smcarray.fsb.list"
  return(out)
}


##' @export
biips_density.smcarray.fsb.list <- function(x, bw="nrd0", ...) {
  stopifnot(is.smcarray.fsb.list(x))
  out <- list()
  for (i in 1:length(x)) {
    if (!is.smcarray.fsb(x[[i]]))
      next
    name <- names(x)[i]
    out[[name]] <- biips_density(x[[i]], bw=rec(bw,i), ...) # recycle bw
  }

  class(out) <- "density.smcarray.fsb.list"

  return(out)
}

##' @importFrom graphics plot
##' @export
plot.density.smcarray <- function(x, main=NULL, xlab=NULL, ylab="Density", ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], main=rec(main,i), xlab=rec(xlab,i), ...) # recycle arguments
  }
  invisible(NULL)
}

##' @export
plot.density.smcarray.fsb.univariate <- function(x, type="l", col=1:6, pch=NULL, lwd=NULL, lty=NULL,
                                                 main=NULL, xlab=NULL, xlim, ylim, ...) {

  # compute axis limits
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
       main=main, xlab=xlab,
       xlim=xlim, ylim=ylim, ...)
  for (fsb in 1:length(x)) {
    lines(x[[fsb]], type=rec(type,fsb), col=rec(col,fsb),
          pch=rec(pch,fsb), lwd=rec(lwd,fsb),
          lty=rec(lty,fsb), ...) # recycle arguments
  }
  invisible(NULL)
}

##' @export
plot.density.smcarray.fsb <- function(x, main=NULL, xlab=NULL, ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], main=rec(main,i), xlab=rec(xlab,i), ...) # recycle arguments
  }
  invisible(NULL)
}

##' @export
plot.density.smcarray.fsb.list <- function(x, main=NULL, xlab=NULL, ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], main=rec(main,i), xlab=rec(xlab,i), ...) # recycle arguments
  }
  invisible(NULL)
}
