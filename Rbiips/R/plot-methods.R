
#' Plot methods.
#' @name plot-methods
#' @importFrom graphics plot
#' @param x a density object.
#' @param main,xlab,ylab plotting parameters with useful defaults.
#' @param type,col,pch,lwd,lty plotting parameters with useful defaults.
#' @param xlim,ylim plotting parameters with useful defaults.
#' @param ... further plotting parameters. See
#'   \code{\link[stats]{plot.density}}, \code{\link[graphics]{plot.histogram}},
#'   \code{\link[graphics]{plot.table}}
#' @return \code{NULL}
#' @seealso \code{\link[stats]{plot.density}},
#'   \code{\link[graphics]{plot.histogram}}, \code{\link[graphics]{plot.table}}
#' @examples
#' #' # Compile model
#' modelfile <- system.file('extdata', 'hmm_f.bug', package = 'Rbiips')
#' stopifnot(nchar(modelfile) > 0)
#' cat(readLines(modelfile), sep = '\n')
#'
#' data <- list(tmax = 10, p = c(.5, .5), logtau_true = log(1), logtau = log(1))
#' model <- biips_model(modelfile, data, sample_data = TRUE)
#'
#' #' # SMC algorithm
#' n_part <- 100
#' out_smc <- biips_smc_samples(model, c('x', 'c[2:10]'), n_part, type = 'fs',
#'                              rs_thres = 0.5, rs_type = 'stratified')
#'
#' par(mfrow = c(2, 2))
#' plot(biips_density(out_smc$x, bw = 'nrd0', adjust = 1, n = 100))
#' plot(biips_table(out_smc[['c[2:10]']]))
#' plot(biips_density(out_smc$x$f))
#' plot(biips_table(out_smc[['c[2:10]']]$f))
#'
#' #' # PIMH algorithm
#' n_part <- 50
#' obj_pimh <- biips_pimh_init(model, c('x', 'c[2:10]'))  # Initialize
#' out_pimh_burn <- biips_pimh_update(obj_pimh, 100, n_part)  # Burn-in
#' out_pimh <- biips_pimh_samples(obj_pimh, 100, n_part)  # Samples
#'
#' par(mfrow = c(2, 2))
#' plot(biips_density(out_pimh$x))
#' plot(biips_table(out_pimh[['c[2:10]']]))
#'
#' #' # PMMH algorithm
#' data <- list(tmax = 10, p = c(.5, .5), logtau_true = log(1))
#' model <- biips_model(modelfile, data)
#'
#' n_part <- 50
#' obj_pmmh <- biips_pmmh_init(model, 'logtau', latent_names = c('x', 'c[2:10]'),
#'                             inits = list(logtau = -2))  # Initialize
#' out_pmmh_burn <- biips_pmmh_update(obj_pmmh, 100, n_part)  # Burn-in
#' out_pmmh <- biips_pmmh_samples(obj_pmmh, 100, n_part, thin = 1)  # Samples
#'
#' plot(biips_density(out_pmmh$logtau))
#' par(mfrow = c(2, 2))
#' plot(biips_density(out_pmmh$x))
#' plot(biips_table(out_pmmh[['c[2:10]']]))
NULL

#' @rdname plot-methods
#' @export
plot.density.smcarray.fsb.univariate <- function(x, type = "l", col = 1:6, pch = NULL,
                                                 lwd = NULL, lty = NULL, main = NULL, xlab = NULL, ylab = "Density", xlim = range(unlist(lapply(x,
                                                                                                                                                function(x) x$x))), ylim = c(0, max(unlist(lapply(x, function(x) x$y)))),
                                                 ...) {

  if (is.null(main)) {
    main <- x[[1]]$data.name
  }
  if (is.null(xlab)) {
    xlab <- paste("N =", x[[1]]$n, "  Bandwidth =", format(x[[1]]$bw, digits = 4))
  }

  plot(NULL, type = "n", main = main, xlab = xlab, ylab = ylab, xlim = xlim, ylim = ylim,
       ...)
  for (fsb in 1:length(x)) {
    lines(x[[fsb]], type = rec(type, fsb), col = rec(col, fsb), pch = rec(pch,
                                                                          fsb), lwd = rec(lwd, fsb), lty = rec(lty, fsb), ...)  # recycle arguments
  }
  invisible(NULL)
}


#' @rdname plot-methods
#' @export
#' @param width real. width of spacing for bars at the same value.
plot.table.smcarray.fsb.univariate <- function(x, type = "h", col = 1:6, pch = NULL,
                                               lwd = 2, lty = NULL, main = NULL, xlab = NULL, ylab = "Probability", xlim,
                                               ylim = c(0, max(unlist(x))), width = 0.1, ...) {

  if (is.null(xlab)) {
    xlab <- names(dimnames(x[[1]]))
  }
  if (missing(xlim)) {
    xlim <- range(as.numeric(unlist(lapply(x, dimnames)))) + .5*width*c(-1, 1)
  }

  plot(NULL, type = "n", main = main, xlab = xlab, ylab = ylab, xlim = xlim, ylim = ylim,
       xaxt = "n", ...)
  at <- sort(unique(as.numeric(unlist(lapply(x, dimnames)))))
  axis(1, at=at)

  for (fsb in 1:length(x)) {
    # separate the lines
    shift <- (fsb - (length(x) + 1)/2) * width/max(length(x) - 1, 2)

    xval <- as.numeric(unlist(dimnames(x[[fsb]])))
    lines(xval + shift, x[[fsb]], type = type, col = rec(col,
                                                        fsb), pch = rec(pch, fsb), lwd = rec(lwd, fsb), lty = rec(lty, fsb),
          ...)  # recycle arguments
  }
  invisible(NULL)
}


#' @rdname plot-methods
#' @export
plot.density.smcarray <- function(x, main = NULL, xlab = NULL, ylab = "Density",
                                  ...) {

  if (is.null(main)) {
    main <- list()
    for (i in 1:length(x)) {
      main[[i]] <- x[[i]]$data.name
    }
  }

  for (i in 1:length(x)) {
    plot(x[[i]], main = rec(main, i), xlab = rec(xlab, i), ylab = rec(ylab, i),
         ...)  # recycle arguments
  }
  invisible(NULL)
}

#' @rdname plot-methods
#' @export
plot.table.smcarray <- function(x, main = NULL, xlab = NULL, ylab = "Probability",
                                ...) {
  for (d in 1:length(x)) {
    plot(x[[d]], main = rec(main, d), xlab = rec(xlab, d), ylab = rec(ylab, d),
         ...)  # recycle arguments
  }
  invisible(NULL)
}

#' @rdname plot-methods
#' @export
plot.density.smcarray.fsb <- function(x, main = NULL, xlab = NULL, ylab = "Density",
                                      ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], main = rec(main, i), xlab = rec(xlab, i), ylab = rec(ylab, i),
         ...)  # recycle arguments
  }
  invisible(NULL)
}

#' @rdname plot-methods
#' @export
plot.table.smcarray.fsb <- function(x, main = NULL, xlab = NULL, ylab = "Probability",
                                    ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], main = rec(main, i), xlab = rec(xlab, i), ylab = rec(ylab, i),
         ...)  # recycle arguments
  }
  invisible(NULL)
}

#' @rdname plot-methods
#' @export
plot.density.smcarray.fsb.list <- function(x, main = NULL, xlab = NULL, ylab = "Density",
                                           ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], main = rec(main, i), xlab = rec(xlab, i), ylab = rec(ylab, i),
         ...)  # recycle arguments
  }
  invisible(NULL)
}

#' @rdname plot-methods
#' @export
plot.table.smcarray.fsb.list <- function(x, main = NULL, xlab = NULL, ylab = "Probability",
                                         ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], main = rec(main, i), xlab = rec(xlab, i), ylab = rec(ylab, i),
         ...)  # recycle arguments
  }
  invisible(NULL)
}


#' @name plot-methods
#' @rdname plot-methods
#' @export
plot.table.mcmcarray <- function(x, main = NULL, xlab = NULL, ylab = "Probability",
                                 ...) {
  for (d in 1:length(x)) {
    plot(x[[d]], main = rec(main, d), xlab = rec(xlab, d), ylab = rec(ylab, d),
         ...)  # recycle arguments
  }
  invisible(NULL)
}


#' @rdname plot-methods
#' @export
plot.density.mcmcarray.univariate <- function(x, type = "l", main = x$data.name, ...) {
  class(x) <- "density"
  plot(x, main = main, ...)
}


#' @rdname plot-methods
#' @export
plot.density.mcmcarray <- function(x, main = NULL, xlab = NULL, ...) {
  if (is.null(main)) {
    main <- list()
    for (i in 1:length(x)) {
      main[[i]] <- x[[i]]$data.name
    }
  }

  for (d in 1:length(x)) {
    plot(x[[d]], main = rec(main, d), xlab = rec(xlab, d), ...)  # recycle arguments
  }
  invisible(NULL)
}

#' @rdname plot-methods
#' @export
plot.histogram.mcmcarray <- function(x, main = NULL, xlab = NULL, ...) {
  if (is.null(main)) {
    main <- list()
    for (i in 1:length(x)) {
      main[[i]] <- paste("Histogram of", x[[i]]$xname)
    }
  }
  for (d in 1:length(x)) {
    plot(x[[d]], main = rec(main, d), xlab = rec(xlab, d), ...)  # recycle arguments
  }
  invisible(NULL)
}

#' @rdname plot-methods
#' @export
plot.table.mcmcarray.list <- function(x, main = NULL, xlab = NULL, ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], main = rec(main, i), xlab = rec(xlab, i), ...)  # recycle arguments
  }
  invisible(NULL)
}

#' @rdname plot-methods
#' @export
plot.density.mcmcarray.list <- function(x, main = NULL, xlab = NULL, ylab = "Density",
                                        ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], main = rec(main, i), xlab = rec(xlab, i), ylab = rec(ylab, i),
         ...)  # recycle arguments
  }
  invisible(NULL)
}

#' @rdname plot-methods
#' @export
plot.histogram.mcmcarray.list <- function(x, main = NULL, xlab = NULL, ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], main = rec(main, i), xlab = rec(xlab, i), ...)  # recycle arguments
  }
  invisible(NULL)
}
