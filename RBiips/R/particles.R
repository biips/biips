which.type <- function(x, type=NULL)
{
  if (!is.null(type) && !is.character(type))
    stop("type must be a character vector or NULL.")
  
  type <- match.arg(type, c("filtering", "smoothing", "backward.smoothing"), several.ok = TRUE)
  
  ind <- NULL
  for (p in seq(along=x))
  {      
    if (is.null(type) || x[[p]]$type %in% type)
      ind <- c(ind, p)
  }
  
  return(ind)
}


print.particles <- function(x, ...)
{
  print(summary(x, ...))
}


mean.particles <- function(x, ...)
{
  ans <- list()
  for (t in seq(along=x))
  {
    ndim <- length(dim(x[[t]]$values))
    n.part <- dim(x[[t]]$values)[ndim]
    len <- length(x[[t]]$values) / n.part
    
    m <- array(dim=dim(x[[t]]$values)[-ndim])
    
    for (d in 1:len)
    {
      from <- d
      by <- len
      to <- len*(n.part-1)+d
      indvec <- seq(from,to,by)
#       if (x[[t]]$discrete) {
#         m[d] <- NA
#       } else {
        m[d] <- weighted.mean(x[[t]]$values[indvec], x[[t]]$weights[indvec], ...)
#       }
    }
    ans[[names(x)[t]]] <- list("Mean"=m)
  }
  return(ans)
}


weighted.var <- function(x, w = NULL, norm.w = FALSE, na.rm = FALSE)
{
  if (!length(w)) {
    if (na.rm) 
      x <- x[!is.na(x)]
    return(var(x))
  }
  if (na.rm) {
    w <- w[i <- !is.na(x)]
    x <- x[i]
  }
  if (norm.w) {
    sum.w <- sum(w)
  } else {
    sum.w <- 1
  }
  sum.w2 <- sum(w^2)
  mean.w <- sum(x * w) / sum.w
  
  var.w <- (sum.w / (sum.w^2 - sum.w2)) * sum(w * (x - mean.w)^2, na.rm = na.rm)
  return(list("Mean"=mean.w, "Var."=var.w))
}


var.particles <- function(x, ...)
{
  ans <- list()
  for (t in seq(along=x))
  {
    ndim <- length(dim(x[[t]]$values))
    n.part <- dim(x[[t]]$values)[ndim]
    len <- length(x[[t]]$values) / n.part
    
    m <- array(dim=dim(x[[t]]$values)[-ndim])
    v <- array(dim=dim(x[[t]]$values)[-ndim])
    
    for (d in 1:len)
    {
      from <- d
      by <- len
      to <- len*(n.part-1)+d
      indvec <- seq(from,to,by)
      mean.var <- weighted.var(x[[t]]$values[indvec], x[[t]]$weights[indvec], norm.w = FALSE, ...)
      m[d] <- mean.var[["Mean"]]
      v[d] <- mean.var[["Var."]]
    }
    ans[[names(x)[t]]] <- list("Mean"=m, "Var."=v)
  }
  return(ans)
}


summary.particles <- function(x, ...)
{
  ans <- var.particles(x);
  
  class(ans) <- "summary.particles"
  return(ans)
}


print.summary.particles <- function(x)
{
  tab <- list()
  for (t in names(x))
  {
    values <- NULL
    varnames <- NULL
    for (i in seq(along=x[[t]]))
    {
      if (i == 1)
      {
        len <- length(x[[t]][[i]])
        for (d in 1:len)
        {
          varname <- deparse.varname("", get.index(d, dim(x[[t]][[i]])))
          varnames <- c(varnames, varname)
        }
      }
      values <- c(values, x[[t]][[i]])
    }
    tab[[t]] <- matrix(values, nrow=len, ncol=length(x[[t]]), dimnames=list(varnames, names(x[[t]])))
  }
  print(tab)
}


plot.summary.particles <- function(x, type="l", lty=1:5, lwd=2, col=1:6, xlab="linear array index",
                                   ylab="estimates", main="estimates", leg.args=list(), ...)
{
  ltyy <- lty
  lwdd <- lwd
  coll <- col
  
  legend.summary.particles <- function(x="topright", y=NULL,
                                       lty=ltyy, lwd=lwdd, col=coll, bty="n", inset=c(0.01,0.01), ...)
  {
    return(legend(x=x, y=y, lty=lty, lwd=lwd, col=col, bty=bty, inset=inset, ...))
  }
  
  tab <- list()
  for (t in names(x))
  {
    values <- NULL
    varnames <- NULL
    have.var = 0
    for (i in seq(along=x[[t]]))
    {
      if (names(x[[t]])[i] == "Var.")
      {
        have.var = 1
        next
      }
      if (i == 1)
      {
        len <- length(x[[t]][[i]])
        for (d in 1:len)
        {
          varname <- deparse.varname("", get.index(d, dim(x[[t]][[i]])))
          varnames <- c(varnames, varname)
        }
      }
      values <- c(values, x[[t]][[i]])
    }
    
    leg <- names(x[[t]])[names(x[[t]]) != "Var."]
    tab[[t]] <- matrix(values, nrow=len, ncol=length(x[[t]])-have.var, dimnames=list(varnames, leg))
    main.title <- paste(t, main, sep=" ")
    matplot(tab[[t]], type=type, xlab=xlab, ylab=ylab, main=main.title, lty=lty, lwd=lwd, col=col, ...)
    
    leg.args[["legend"]] <- leg
    do.call("legend.summary.particles", leg.args)
  }
}


diagnostic <- function(x, ...)
  UseMethod("diagnostic")


diagnostic.particles <- function(x, ess.min=30)
{
  ans <- list()
  for (t in names(x))
  {
    ess.min <- min(x[[t]]$ess)
    ans[[t]] <- list("ESS min."=ess.min)
  }
  return(ans)
}

 
get.index <- function(offset, dim)
{
  ind <- rep(1, length(dim))
  offset <- offset-1
  for (i in 1:length(dim))
  {
    ind[i] <- ind[i] + (offset) %% dim[i];
    offset <- offset %/% dim[i];
  }
  return(ind)
}


deparse.varname <- function(name, indices)
{
  varname <- name
  if (length(indices) > 0)
  {
    varname <- paste(varname, "[", sep="")
    for (i in seq(along=indices))
    {
      if (i>1)
        varname <- paste(varname, ", ", sep="")
      varname <- paste(varname, indices[[i]] , sep="")
    }
    varname <- paste(varname, "]", sep="")
  }
  return(varname)
}

 
density.particles <- function(x, bw="nrd0", adjust=1, ...)
{
  densities <- list()
  
  density.particles.atomic <- function(xx, bw, adjust, ...)
  {
    densities <- list()
  
    ndim <- length(dim(xx$values))
    n.part <- dim(xx$values)[ndim]
    len <- length(xx$values) / n.part
    
    bww <- bw
    adj <- adjust
    for (d in 1:len)
    {
      from <- d
      by <- len
      to <- len*(n.part-1)+d
      indvec <- seq(from,to,by)
      varname <- deparse.varname(xx$variable.name, get.index(d, dim(xx$values)[-ndim]))
      
      if (xx$type == "smoothing")
      {
        if (!is.null(parent.env(environment())$densities[[varname]]))
        {
          bww <- parent.env(environment())$densities[[varname]][[1]]$density$bw
          adj <- 1
        }
      }
      
      dens <- density(xx$values[indvec], weights=xx$weights[indvec], bw=bww, adjust=adj, ...)
      
      densities[[varname]] <- list(list(density=dens, variable.name=varname, type=xx$type, n.part=n.part, ess=xx$ess[d]))
      names(densities[[varname]]) <- c(xx$type)
    }
    return(densities)
  }
  
  # first treat filtering and backward.smoothing
  if (!is.null(x[["filtering"]]))
  {
    densities <- density.particles.atomic(x[["filtering"]], bw, adjust, ...)
  }
  if (!is.null(x[["backward.smoothing"]]))
  {
    dens <- density.particles.atomic(x[["backward.smoothing"]], bw, adjust, ...)
    for (n in names(dens))
      densities[[n]] <- c(densities[[n]], dens[[n]])
  }

  
  # then treat smoothing
  if (!is.null(x[["smoothing"]]))
  {
    dens <- density.particles.atomic(x[["smoothing"]], bw, adjust, ...)
    for (n in names(dens))
      densities[[n]] <- c(densities[[n]], dens[[n]])
  }

  class(densities) <- "density.particles"
  return(densities)
}



plot.density.particles <- function(x, leg.args=list(),
                                   type="l", lty=1:5, lwd=2,
                                   col=1:6,
                                   ylab="density", main="kernel density estimates", ...)
{
  ltyy <- lty
  lwdd <- lwd
  coll <- col
  
  legend.density.particles <- function(x="topright", y=NULL,
                                       lty=ltyy, lwd=lwdd, col=coll, bty="n", inset=c(0.01,0.01), ...)
  {
    return(legend(x=x, y=y, lty=lty, lwd=lwd, col=col, bty=bty, inset=inset, ...))
  }
  
  for (n in seq(along=x))
  {
    xx <- NULL
    yy <- NULL
    ncol <- 0
    leg <- NULL
    
    varname <- names(x)[n]
    
    for (t in seq(along=x[[n]]))
    { 
      xx <- c(xx, x[[n]][[t]]$density$x)
      yy <- c(yy, x[[n]][[t]]$density$y)
      ncol <- ncol+1
      leg <- c(leg, paste(x[[n]][[t]]$type, ", ess=", round(x[[n]][[t]]$ess)))
      bw <- x[[n]][[t]]$density$bw
    }
    
    xx <- matrix(xx, ncol=ncol)
    yy <- matrix(yy, ncol=ncol)
    
    xlab <- paste("n.part = ", x[[n]][[1]]$n.part, ", bw = ", signif(bw, digits = 2))
    main.title <- paste(varname, " ", main)
  
    matplot(xx, yy, type=type, lty=lty, lwd=lwd, col=col, xlab=xlab, ylab=ylab, main=main.title, ...)
    
    leg.args[["legend"]] <- leg

    do.call(legend.density.particles, leg.args)
  }
      
  invisible(NULL)
}