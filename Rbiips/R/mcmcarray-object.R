mcmcarray <- function(data = NA, dim = length(data), dimnames = NULL, iteration = length(dim),
                      chain = NA) {
  stopifnot(length(iteration) == 1, length(chain) == 1)
  if (is.na(iteration))
    stopifnot(is.na(chain)) else {
      stopifnot(is.numeric(iteration), iteration >= 1, iteration <= length(dim))
      if (!any(is.na(chain)))
        stopifnot(is.numeric(chain), chain >= 1, chain <= length(dim), chain !=
                    iteration)
    }

  x <- array(data, dim, dimnames)

  nd <- names(dim(x))
  nd[iteration] <- "iteration"
  nd[chain] <- "chain"
  names(dim(x)) <- nd

  class(x) <- "mcmcarray"
  return(x)
}


##' @export
is.mcmcarray <- function(object) {
  return(class(object) == "mcmcarray")
}

##' @export
is.mcmcarray.list <- function(object) {
  return(class(object) == "mcmcarray.list")
}



##' @export
print.mcmcarray <- function(x, ...) {
  print(summary(x), ...)
}

##' @export
print.mcmcarray.list <- function(x, ...) {
  print(summary(x), ...)
}

##' @export
summary.mcmcarray <- function(object, probs = c(), order, mode = all(object == as.integer(object)), ...) {

  ### TODO check arguments
  if (length(probs) > 0)
    stopifnot(is.numeric(probs), probs > 0, probs < 1)

  if (missing(order))
    order <- ifelse(mode, 0, 1)

  drop_dims <- names(dim(object)) %in% c("iteration", "chain")
  dim_array <- dim(object)[!drop_dims]
  marg <- which(!drop_dims)
  summ <- list()

  ### moment based statistics
  if (order >= 1)
    summ$mean <- apply(object, marg, FUN = mean)

  if (order >= 2)
    summ$var <- apply(object, marg, FUN = var)

  if (order >= 3) {
    mom2 <- apply(object^2, marg, FUN = mean)
    mom3 <- apply(object^3, marg, FUN = mean)
    summ$skew <- (mom3 - 3 * mom2 * summ$mean + 2 * summ$mean^3)/summ$var^(3/2)
  }

  if (order >= 4) {
    mom4 <- apply(object^4, marg, FUN = mean)
    summ$kurt <- (mom4 - 4 * mom3 * summ$mean + 6 * mom2 * summ$mean^2 - 3 *
                    summ$mean^4)/summ$var^2 - 3
  }

  ### quantiles
  if (length(probs) > 0) {
    summ$probs <- probs
    quant <- apply(object, marg, FUN = quantile, probs = probs, names = FALSE)
    ndim <- length(dim(quant))
    quant <- aperm(quant, c(2:ndim, 1))

    len <- prod(dim_array)
    summ$quant <- list()
    for (i in 1:length(probs)) {
      summ$quant[[as.character(probs[i])]] <- array(quant[((i - 1) * len +
                                                             1):(i * len)], dim = dim_array)
    }
  }

  ### mode
  if (mode) {
    summ$mode <- apply(object, marg, FUN = function(x) as.numeric(names(which.max(table(x)))))
  }

  summ$drop.dims <- dim(object)[drop_dims]

  class(summ) <- "summary.mcmcarray"

  return(summ)
}

##' @export
summary.mcmcarray.list <- function(object, ...) {
  ans <- list()
  for (n in names(object)) {
    if (!is.mcmcarray(object[[n]]))
      next
    ans[[n]] <- summary(object[[n]], ...)
  }

  class(ans) <- "summary.mcmcarray.list"

  return(ans)
}

##' @export
print.summary.mcmcarray <- function(x, ...) {
  cat("mcmcarray:\n")
  print(x[!(names(x) %in% c("drop.dims"))], ...)
  if (length(x$drop.dims) > 0) {
    cat("Marginalizing over:", paste(paste(names(x$drop.dims), "(", x$drop.dims,
                                           ")", sep = ""), collapse = ","), "\n")
  }
}


##' @export
print.summary.mcmcarray.list <- function(x, ...) {
  for (n in names(x)) {
    if (n %in% c("log_marg_like_pen", "log_marg_like", "info"))
      next
    cat(n, " ", sep="")
    print(x[[n]], ...)
    cat("\n")
  }
  invisible()
}


##' @export
table.mcmcarray <- function(x, ...) {
  out <- list()

  dimen <- dim(x)
  drop_dim <- names(dimen) %in% c("iteration", "chain")
  n_samples <- prod(dimen[drop_dim])
  len <- prod(dimen[!drop_dim])

  for (d in 1:len) {
    ind_vec <- seq(d, len * (n_samples - 1) + d, len)
    values <- x[ind_vec]

    out[[d]] <- table(values, ...)/n_samples
  }

  dim(out) <- dimen[!drop_dim]
  class(out) <- "table.mcmcarray"
  return(out)
}


##' @importFrom stats density
##' @export
density.mcmcarray <- function(x, bw = "nrd0", ...) {
  out <- list()

  dimen <- dim(x)
  drop_dim <- names(dimen) %in% c("iteration", "chain")
  n_samples <- prod(dimen[drop_dim])
  len <- prod(dimen[!drop_dim])

  for (d in 1:len) {
    ind_vec <- seq(d, len * (n_samples - 1) + d, len)
    values <- x[ind_vec]

    out[[d]] <- density(values, bw = rec(bw,d), ...) # recycle
  }

  dim(out) <- dimen[!drop_dim]
  class(out) <- "density.mcmcarray"
  return(out)
}

##' @export
hist.mcmcarray <- function(x, main=NULL, xlab=NULL, ...) {
  out <- list()

  dimen <- dim(x)
  drop_dim <- names(dimen) %in% c("iteration", "chain")
  n_samples <- prod(dimen[drop_dim])
  len <- prod(dimen[!drop_dim])

  for (d in 1:len) {
    ind_vec <- seq(d, len * (n_samples - 1) + d, len)
    values <- x[ind_vec]

    out[[d]] <- hist(values, main=rec(main, d), xlab=rec(xlab, d), ...) # recycle arguments
  }

  dim(out) <- dimen[!drop_dim]
  class(out) <- "histogram.mcmcarray"
  return(invisible(out))
}

##' @export
table.mcmcarray.list <- function(x, ...) {
  out <- list()
  for (i in 1:length(x)) {
    name <- names(x)[i]
    if (!is.mcmcarray(x[[i]]) || name %in% c("log_marg_like_pen", "log_marg_like", "info"))
      next
    out[[name]] <- table(x[[i]], ...)
  }
  class(out) <- "table.mcmcarray.list"
  return(out)
}

##' @export
density.mcmcarray.list <- function(x, bw = "nrd0", ...) {
  out <- list()
  for (i in 1:length(x)) {
    name <- names(x)[i]
    if (!is.mcmcarray(x[[i]]) || name %in% c("log_marg_like_pen", "log_marg_like", "info"))
      next
    out[[name]] <- density(x[[i]], bw = rec(bw,i), ...) # recycle bw
  }
  class(out) <- "density.mcmcarray.list"
  return(out)
}

##' @export
hist.mcmcarray.list <- function(x, main=NULL, xlab=NULL, ...) {
  out <- list()
  for (i in 1:length(x)) {
    name <- names(x)[i]
    if (!is.mcmcarray(x[[i]]) || name %in% c("log_marg_like_pen", "log_marg_like", "info"))
      next
    out[[name]] <- hist(x[[i]], main=rec(main,i), xlab=rec(xlab,i), ...) # recycle arguments
  }
  class(out) <- "histogram.mcmcarray.list"
  return(invisible(out))
}

##' @export
plot.table.mcmcarray <- function(x, main=NULL, xlab=NULL, ylab="Probability", ...) {
  for (d in 1:length(x)) {
    plot(x[[d]], main=rec(main,d), xlab=rec(xlab,d), ylab=ylab, ...) # recycle arguments
  }
  invisible(NULL)
}

##' @export
plot.density.mcmcarray <- function(x, main=NULL, xlab=NULL, ...) {
  for (d in 1:length(x)) {
    plot(x[[d]], main=rec(main,d), xlab=rec(xlab,d), ...) # recycle arguments
  }
  invisible(NULL)
}

##' @export
plot.histogram.mcmcarray <- function(x, main=NULL, xlab=NULL, ...) {
  for (d in 1:length(x)) {
    plot(x[[d]], main=rec(main,d), xlab=rec(xlab,d), ...) # recycle arguments
  }
  invisible(NULL)
}

##' @export
plot.table.mcmcarray.list <- function(x, main=NULL, xlab=NULL, ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], main=rec(main,i), xlab=rec(xlab,i), ...) # recycle arguments
  }
  invisible(NULL)
}

##' @export
plot.density.mcmcarray.list <- function(x, main=NULL, xlab=NULL, ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], main=rec(main,i), xlab=rec(xlab,i), ...) # recycle arguments
  }
  invisible(NULL)
}

##' @export
plot.histogram.mcmcarray.list <- function(x, main=NULL, xlab=NULL, ...) {
  for (i in 1:length(x)) {
    plot(x[[i]], main=rec(main,i), xlab=rec(xlab,i), ...) # recycle arguments
  }
  invisible(NULL)
}
