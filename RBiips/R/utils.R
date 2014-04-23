
get_seed <- function() {
  seed <- sample.int(.Machine$integer.max, size = 1)
  return(seed)
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


check_type <- function(type, several.ok = TRUE) {
  stopifnot(is.character(type))
  type <- unlist(strsplit(type, NULL))
  type <- match.arg(type, c("f", "s", "b"), several.ok = several.ok)
  type <- unique(type)
  return(type)
} 
