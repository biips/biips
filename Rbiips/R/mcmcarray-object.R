#' Objects for representing MCMC output.
#'
#' @description
#' A \code{mcmcarray} object is used by the \code{\link{biips_pimh_samples}} or
#'  \code{\link{biips_pmmh_samples}} functions to represent MCMC output of a given variable.
#'
#' A \code{mcmcarray.list} object is a named list of \code{mcmcarray} objects for
#' different monitored variables.
#'
#' Methods apply identically to \code{mcmcarray} or \code{mcmcarray.list} objects
#' and return a list with the same nested named members as the input object.
#'
#' @name mcmcarray-object
#' @aliases mcmcarray.list-object mcmcarray mcmcarray.list
#'   is.mcmcarray is.mcmcarray.list
#'   biips_summary.mcmcarray biips_summary.mcmcarray.list
#'   biips_density.mcmcarray biips_density.mcmcarray.list
#'   biips_table.mcmcarray biips_table.mcmcarray.list
#'   biips_hist.mcmcarray biips_hist.mcmcarray.list
#'   summary.mcmcarray summary.mcmcarray.list
#'   density.mcmcarray density.mcmcarray.list
#'   table.mcmcarray table.mcmcarray.list
#'   hist.mcmcarray hist.mcmcarray.list
#' @param object,x a \code{mcmcarray} or \code{mcmcarray.list} object.
#' @param ... additional arguments to be passed to the default methods.
#'
#' @return Methods apply identically to \code{mcmcarray} or \code{mcmcarray.list} objects
#'   and return a list with the same named members as the input object.
#'
#' @examples
#'
#' ## Should be DIRECTLY executable !!
#' ##-- ==>  Define data, use random,
#' ##--  or do  help(data=index)  for the standard data sets.
#'
NULL

#' @param data      numerical vector
#' @param dim       vector of integers
#' @param dimnames  character vector
#' @param iteration integer. index of the dimension corresponding to iterations of the MCMC.
#' @param chain     integer. index of the dimension corresponding to chain of the MCMC.
#' @keywords internal
mcmcarray <- function(data = NA, dim = length(data), dimnames = NULL,
    iteration = length(dim), chain = NA) {
    stopifnot(length(iteration) == 1, length(chain) == 1)
    if (is.na(iteration))
        stopifnot(is.na(chain)) else {
        stopifnot(is.numeric(iteration), iteration >= 1, iteration <=
            length(dim))
        if (!any(is.na(chain)))
            stopifnot(is.numeric(chain), chain >= 1, chain <=
                length(dim), chain != iteration)
    }

    x <- array(data, dim, dimnames)

    nd <- names(dim(x))
    nd[iteration] <- "iteration"
    nd[chain] <- "chain"
    names(dim(x)) <- nd

    class(x) <- "mcmcarray"
    return(x)
}


#' @export
#' @rdname mcmcarray-object
#' @return The function \code{is.mcmcarray} returns TRUE if the object is of class \code{mcmcarray}.
is.mcmcarray <- function(object) {
    return(class(object) == "mcmcarray")
}

#' @export
#' @rdname mcmcarray-object
#' @return The function \code{is.mcmcarray.list} returns TRUE if the object is of class \code{mcmcarray.list}.
is.mcmcarray.list <- function(object) {
    return(class(object) == "mcmcarray.list")
}

#' @export
print.mcmcarray <- function(x, ...) {
    print(summary(x), ...)
}

#' @export
print.mcmcarray.list <- function(x, ...) {
    print(summary(x), ...)
}

#' @export
#' @rdname mcmcarray-object
#' @inheritParams biips_summary.smcarray
#' @return The method \code{biips_summary} returns univariate marginal statistics.
#'   The output innermost members are objects of class \code{summary.mcmcarray},
#'   \emph{i.e.} lists with members:
#'   \item{mean}{if \code{order>=1}.}
#'   \item{var}{variance, if \code{order>=2}.}
#'   \item{skew}{skewness, if \code{order>=3}.}
#'   \item{kurt}{kurtosis, if \code{order>=4}.}
#'   \item{probs}{vector of quantile probabilities.}
#'   \item{quant}{list of quantile values, if \code{probs} is not empty.}
#'   \item{mode}{most frequent values for discrete components.}
biips_summary.mcmcarray <- function(object, probs = c(), order,
    mode = all(object == as.integer(object)), ...) {
    stopifnot(is.mcmcarray(object))

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
        summ$kurt <- (mom4 - 4 * mom3 * summ$mean + 6 * mom2 *
            summ$mean^2 - 3 * summ$mean^4)/summ$var^2 - 3
    }

    ### quantiles
    if (length(probs) > 0) {
        summ$probs <- probs
        quant <- apply(object, marg, FUN = quantile, probs = probs,
            names = FALSE)
        ndim <- length(dim(quant))
        quant <- aperm(quant, c(2:ndim, 1))

        len <- prod(dim_array)
        summ$quant <- list()
        for (i in 1:length(probs)) {
            summ$quant[[as.character(probs[i])]] <- array(quant[((i -
                1) * len + 1):(i * len)], dim = dim_array)
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

#' @export
#' @rdname mcmcarray-object
biips_summary.mcmcarray.list <- function(object, ...) {
    stopifnot(is.mcmcarray.list(object))
    ans <- list()
    for (n in names(object)) {
        if (!is.mcmcarray(object[[n]]))
            next
        ans[[n]] <- biips_summary(object[[n]], ...)
    }

    class(ans) <- "summary.mcmcarray.list"

    return(ans)
}

#' @export
print.summary.mcmcarray <- function(x, ...) {
    cat("mcmcarray:\n")
    print(x[!(names(x) %in% c("drop.dims"))], ...)
    if (length(x$drop.dims) > 0) {
        cat("Marginalizing over:", paste(paste(names(x$drop.dims),
            "(", x$drop.dims, ")", sep = ""), collapse = ","),
            "\n")
    }
}


#' @export
print.summary.mcmcarray.list <- function(x, ...) {
    for (n in names(x)) {
        if (n %in% c("log_marg_like_pen", "log_marg_like", "info"))
            next
        cat(n, " ", sep = "")
        print(x[[n]], ...)
        cat("\n")
    }
    invisible()
}

#' @export
#' @rdname mcmcarray-object
#' @return The method \code{biips_table} returns univariate marginal frequency tables or probability
#'   mass estimates of discrete variables. The output innermost members are objects
#'   of class \code{table.mcmcarray}.
biips_table.mcmcarray <- function(x, ...) {
    stopifnot(is.mcmcarray(x))
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


#' @export
#' @rdname mcmcarray-object
#' @inheritParams biips_density.smcarray
#' @return The method \code{biips_density} returns univariate marginal kernel density estimates.
#'   The output innermost members are objects of class \code{density.mcmcarray}.
biips_density.mcmcarray <- function(x, bw = "nrd0", ...) {
    stopifnot(is.mcmcarray(x))
    out <- list()

    dimen <- dim(x)
    drop_dim <- names(dimen) %in% c("iteration", "chain")
    n_samples <- prod(dimen[drop_dim])
    len <- prod(dimen[!drop_dim])

    for (d in 1:len) {
        ind_vec <- seq(d, len * (n_samples - 1) + d, len)
        values <- x[ind_vec]

        out[[d]] <- density(values, bw = rec(bw, d), ...)  # recycle
    }

    dim(out) <- dimen[!drop_dim]
    class(out) <- "density.mcmcarray"
    return(out)
}


biips_hist <- function(x, ...) UseMethod("biips_hist")

#' @export
#' @rdname mcmcarray-object
biips_hist.mcmcarray <- function(x, main = NULL, xlab = NULL,
    ...) {
    stopifnot(is.mcmcarray(x))
    out <- list()

    dimen <- dim(x)
    drop_dim <- names(dimen) %in% c("iteration", "chain")
    n_samples <- prod(dimen[drop_dim])
    len <- prod(dimen[!drop_dim])

    for (d in 1:len) {
        ind_vec <- seq(d, len * (n_samples - 1) + d, len)
        values <- x[ind_vec]

        out[[d]] <- hist(values, main = rec(main, d), xlab = rec(xlab,
            d), ...)  # recycle arguments
    }

    dim(out) <- dimen[!drop_dim]
    class(out) <- "histogram.mcmcarray"
    return(invisible(out))
}

#' @export
#' @rdname mcmcarray-object
biips_table.mcmcarray.list <- function(x, ...) {
    stopifnot(is.mcmcarray.list(x))
    out <- list()
    for (i in 1:length(x)) {
        name <- names(x)[i]
        if (!is.mcmcarray(x[[i]]) || name %in% c("log_marg_like_pen",
            "log_marg_like", "info"))
            next
        out[[name]] <- biips_table(x[[i]], ...)
    }
    class(out) <- "table.mcmcarray.list"
    return(out)
}

#' @export
#' @rdname mcmcarray-object
biips_density.mcmcarray.list <- function(x, bw = "nrd0", ...) {
    stopifnot(is.mcmcarray.list(x))
    out <- list()
    for (i in 1:length(x)) {
        name <- names(x)[i]
        if (!is.mcmcarray(x[[i]]) || name %in% c("log_marg_like_pen",
            "log_marg_like", "info"))
            next
        out[[name]] <- biips_density(x[[i]], bw = rec(bw, i),
            ...)  # recycle bw
    }
    class(out) <- "density.mcmcarray.list"
    return(out)
}

#' @export
#' @rdname mcmcarray-object
biips_hist.mcmcarray.list <- function(x, main = NULL, xlab = NULL,
    ...) {
    stopifnot(is.mcmcarray.list(x))
    out <- list()
    for (i in 1:length(x)) {
        name <- names(x)[i]
        if (!is.mcmcarray(x[[i]]) || name %in% c("log_marg_like_pen",
            "log_marg_like", "info"))
            next
        out[[name]] <- biips_hist(x[[i]], main = rec(main, i),
            xlab = rec(xlab, i), ...)  # recycle arguments
    }
    class(out) <- "histogram.mcmcarray.list"
    return(invisible(out))
}

#' @importFrom graphics plot
#' @export
#' @seealso \code{\link{plot.table}}
plot.table.mcmcarray <- function(x, main = NULL, xlab = NULL,
    ylab = "Probability", ...) {
    for (d in 1:length(x)) {
        plot(x[[d]], main = rec(main, d), xlab = rec(xlab, d),
            ylab = ylab, ...)  # recycle arguments
    }
    invisible(NULL)
}

#' @export
#' @seealso \code{\link{plot.density}}
plot.density.mcmcarray <- function(x, main = NULL, xlab = NULL,
    ...) {
    for (d in 1:length(x)) {
        plot(x[[d]], main = rec(main, d), xlab = rec(xlab, d),
            ...)  # recycle arguments
    }
    invisible(NULL)
}

#' @export
#' @seealso \code{\link{plot.histogram}}
plot.histogram.mcmcarray <- function(x, main = NULL, xlab = NULL,
    ...) {
    for (d in 1:length(x)) {
        plot(x[[d]], main = rec(main, d), xlab = rec(xlab, d),
            ...)  # recycle arguments
    }
    invisible(NULL)
}

#' @export
plot.table.mcmcarray.list <- function(x, main = NULL, xlab = NULL,
    ...) {
    for (i in 1:length(x)) {
        plot(x[[i]], main = rec(main, i), xlab = rec(xlab, i),
            ...)  # recycle arguments
    }
    invisible(NULL)
}

#' @export
plot.density.mcmcarray.list <- function(x, main = NULL, xlab = NULL,
    ...) {
    for (i in 1:length(x)) {
        plot(x[[i]], main = rec(main, i), xlab = rec(xlab, i),
            ...)  # recycle arguments
    }
    invisible(NULL)
}

#' @export
plot.histogram.mcmcarray.list <- function(x, main = NULL, xlab = NULL,
    ...) {
    for (i in 1:length(x)) {
        plot(x[[i]], main = rec(main, i), xlab = rec(xlab, i),
            ...)  # recycle arguments
    }
    invisible(NULL)
}


#' @export
#' @rdname mcmcarray-object
#' @return The method \code{summary} is an alias for \code{biips_summary}.
summary.mcmcarray <- function(object, ...) {
  return(biips_summary(object, ...))
}
#' @export
#' @rdname mcmcarray-object
summary.mcmcarray.list <- function(object, ...) {
  return(biips_summary(object, ...))
}

#' @importFrom stats density
#' @export
#' @rdname mcmcarray-object
#' @return The method \code{density} is an alias for \code{biips_density}.
density.mcmcarray <- function(x, ...) {
  return(biips_density(x, ...))
}
#' @export
#' @rdname mcmcarray-object
density.mcmcarray.list <- function(x, ...) {
  return(biips_density(x, ...))
}
#' @importFrom graphics hist
#' @export
#' @rdname mcmcarray-object
#' @return The method \code{hist} is an alias for \code{biips_hist}.
hist.mcmcarray <- function(x, ...) {
  return(biips_hist(x, ...))
}
#' @export
#' @rdname mcmcarray-object
hist.mcmcarray.list <- function(x, ...) {
  return(biips_hist(x, ...))
}
