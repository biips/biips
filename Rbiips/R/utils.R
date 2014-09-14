
get_seed <- function() {
  seed <- sample.int(.Machine$integer.max, size = 1)
  return(seed)
}

is_legal_vname <- function(name) {
  stopifnot(is.character(name), length(name) == 1, nchar(name) > 0)

  m <- regmatches("[[:alpha:]][[:alnum:]_\\.]*", name)
  is_legal <- (nchar(name) == attr(m, "match.length")) && (name != "log_norm_const")
  return(is_legal)
}

### FIXME transfrom variable name. eg: x[1, ] => x[1,1:100]
to_biips_vname <- function(var) {
  stopifnot(is.character(var), length(var) == 1, nchar(var) > 0)
  ## remove spaces
  v <- gsub(" ", "", var)
  return(v)
}


##' Try to parse string of form 'a' or 'a[n,p:q,r]' where 'a' is a
##' variable name and n,p,q,r are integers
##' @param varname string containing the name of the variable to sparse
parse_varname <- function(varname) {

  v <- try(parse(text = varname, n = 1), silent = TRUE)
  if (!is.expression(v) || length(v) != 1)
    return()

  v <- v[[1]]
  if (is.name(v)) {
    ## Full node array requested
    return(list(name = deparse(v)))
  } else if (is.call(v) && identical(deparse(v[[1]]), "[") && length(v) > 2) {
    ## Subset requested
    ndim <- length(v) - 2
    lower <- upper <- numeric(ndim)
    if (any(nchar(sapply(v, deparse)) == 0)) {
      ## We have to catch empty indices here or they will cause trouble below
      return()
    }
    for (i in 1:ndim) {
      index <- v[[i + 2]]
      if (is.numeric(index)) {
        ## Single index
        lower[i] <- upper[i] <- index
      } else if (is.call(index) && length(index) == 3 && identical(deparse(index[[1]]),
                                                                   ":") && is.numeric(index[[2]]) && is.numeric(index[[3]])) {
        ## Index range
        lower[i] <- index[[2]]
        upper[i] <- index[[3]]
      } else return()
    }
    if (any(upper < lower))
      return()
    return(list(name = deparse(v[[2]]), lower = lower, upper = upper))
  }
  return()
}


parse_varnames <- function(varnames) {
  names <- character(length(varnames))
  lower <- upper <- vector("list", length(varnames))
  for (i in seq(along = varnames)) {
    y <- parse_varname(varnames[i])
    if (is.null(y))
      stop("Invalid variable subset ", varnames[i])
    names[i] <- y$name
    if (!is.null(y$lower))
      lower[[i]] <- y$lower
    if (!is.null(y$upper))
      upper[[i]] <- y$upper
  }
  return(list(names = names, lower = lower, upper = upper))
}



deparse_varname <- function(name, lower = NULL, upper = lower) {
  stopifnot (is.character(name), length(name) == 1, nchar(name)>0)

  if (length(lower) == 0)
    return(name)

  stopifnot (is.numeric(lower), is.numeric(lower), length(upper) == length(lower), upper >= lower)

  varname <- paste(varname, "[", sep = "")
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


##' Preprocess the data (list or env) to list for stan
##' @param data  A list or an environment of objects.
##' @details stop if no-name lists; duplicate names.
##' stop if the objects given name is not found.
##' remove NULL, non-numeric elements
##' @author Adapted from rstan.
data_preprocess <- function(data) {

  if (is.environment(data)) {
    data <- as.list(data)
  } else if (is.list(data)) {
    v <- names(data)
    if (is.null(v))
      stop("data must be a named list")

    if (any(duplicated(v))) {
      stop("duplicated names in data list: ", paste(v[duplicated(v)], collapse = " "))
    }
  } else {
    stop("data must be a list or an environment")
  }

  names <- names(data)
  for (n in names) {
    if (!is_legal_vname(n))
      stop("data with name ", n, " is not allowed in Biips")
  }

  v <- names(data)
  data <- lapply(data, FUN = function(x) {
    ## change data.frame to array
    if (is.data.frame(x)) {
      x <- data.matrix(x)
    }

    # remove those not numeric data
    if (!is.numeric(x)) {
      x <- NULL
    }

    return(x)
  })

  ignored <- setdiff(v, names(data))
  if (length(ignored) > 0)
    warning("Ignored non numeric variables in data: ", paste(ignored, collapse = " "))

  return(data)
}


##' Make a list using names
##' @param names character strings of names of objects
##' @param env the environment to look for objects with names.
##' @note we use inherits = TRUE when calling mget
##' @author Adapted from rstan.
mklist <- function(names, env = parent.frame()) {
  d <- mget(names, env, ifnotfound = NA, inherits = TRUE, mode = "numeric")
  n <- which(is.na(d))
  if (length(n) > 0) {
    stop("objects ", paste("'", names[n], "'", collapse = ", ", sep = ""), " not found")
  }
  return(d)
}




check_type <- function(type, several.ok = TRUE) {
  stopifnot(is.character(type))
  type <- unlist(strsplit(type, NULL))
  type <- match.arg(type, c("f", "s", "b"), several.ok = several.ok)
  return(unique(type))
}
