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
summary.mcmcarray <- function(object, probs = c(), order, ...) {

  ### TODO check arguments
  if (length(probs) > 0)
    stopifnot(is.numeric(probs), probs > 0, probs < 1)

  mode <- all(object == as.integer(object))
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


##' @importFrom stats density
##' @export
density.mcmcarray <- function(x, bw = "nrd0", adjust = 1, ...) {
  out <- list()
  bww <- bw

  mc_dim <- dim(x)
  drop_dim <- names(mc_dim) %in% c("iteration", "chain")

  n_iter <- mc_dim["iteration"]
  if (is.na(n_iter))
    n_iter <- 1
  n_chain <- mc_dim["chain"]
  if (is.na(n_chain))
    n_chain <- 1

  ndim <- sum(!drop_dim)
  lower <- rep(1, ndim)
  upper <- mc_dim[!drop_dim]
  len <- prod(upper)

  for (d in 1:len) {
    ind <- get_index(d, lower, upper)
    varname <- deparse_varname("Z", ind)
    varname <- substring(varname, 2)

    ind_vec <- seq(d, len * (n_iter*n_chain - 1) + d, len)
    values <- x[ind_vec]

    if (length(bw) > 1)
      bww <- bw[[d]]

    dens <- density(values, bw = bww, adjust = adjust, ...)

    out[[varname]] <- dens
  }

  class(out) <- "density.mcmcarray"
  return(out)
}


##' @export
density.mcmcarray.list <- function(x, bw = "nrd0", adjust = 1, ...) {
  out <- list()
  for (n in names(x)) {
    if (!is.mcmcarray(x[[n]]) || n %in% c("log_marg_like_pen", "log_marg_like", "info"))
      next
    out[[n]] <- density(x[[n]], bw = bw, adjust = adjust, ...)
  }

  class(out) <- "density.mcmcarray.list"

  return(out)
}